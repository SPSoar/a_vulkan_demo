#include "SPCommandBuffer.h"

#include "SPCommandPool.h"
#include "../Buffer/SPBuffer.h"
#include "../Driver/SPDevice.h"
#include "../Pipeline/SPRenderPass.h"
#include "../RenderContext/SPFramebuffer.h"
#include "../RenderContext/SPImageView.h"
#include "../RenderContext/SPImage.h"

SPCommandBuffer::SPCommandBuffer(SPCommandPool& command_pool, VkCommandBufferLevel level)
	: command_pool_{ command_pool }
    , max_push_constants_size_ { command_pool.get_device().get_gpu().get_properties().limits.maxPushConstantsSize }
    , level_{ level }
	, handle_{ VK_NULL_HANDLE }
	, state_{ State::Initial }
	, update_after_bind_{ false }
{
	VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };

	allocate_info.commandPool = command_pool.get_handle();
	allocate_info.commandBufferCount = 1;
	allocate_info.level = level;

	VkResult result = vkAllocateCommandBuffers(command_pool.get_device().get_handle(), &allocate_info, &handle_);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Failed to allocate command buffer" };
	}
}

SPCommandBuffer::SPCommandBuffer(SPCommandBuffer&& other)
	: command_pool_ { other.command_pool_ }
	, level_{ other.level_ }
	, handle_{ other.handle_ }
	, state_{ other.state_ }
	, update_after_bind_{ other.update_after_bind_ }
	, max_push_constants_size_ { other .max_push_constants_size_}
{
	other.handle_ = VK_NULL_HANDLE;
	other.state_ = State::Invalid;
}

SPCommandBuffer::~SPCommandBuffer()
{	
	// Destroy command buffer
	if (handle_ != VK_NULL_HANDLE)
	{
		vkFreeCommandBuffers(command_pool_.get_device().get_handle(), command_pool_.get_handle(), 1, &handle_);
	}
}

SPDevice& SPCommandBuffer::get_device()
{
	return command_pool_.get_device();
}

const VkCommandBuffer& SPCommandBuffer::get_handle() const
{
	return handle_;
}

VkResult SPCommandBuffer::begin(VkCommandBufferUsageFlags flags, SPCommandBuffer* primary_cmd_buf)
{
	if (level_ == VK_COMMAND_BUFFER_LEVEL_SECONDARY)
	{
		assert(primary_cmd_buf && "A primary command buffer pointer must be provided when calling begin from a secondary one");
		auto render_pass_binding = primary_cmd_buf->get_current_render_pass();

		return begin(flags, render_pass_binding.render_pass_, render_pass_binding.framebuffer_, primary_cmd_buf->get_current_subpass_index());
	}

	return begin(flags, nullptr, nullptr, 0);
}

VkResult SPCommandBuffer::begin(VkCommandBufferUsageFlags flags, const SPRenderPass* render_pass, const SPFramebuffer* framebuffer, uint32_t subpass_index)
{
	assert(!is_recording() && "Command buffer is already recording, please call end before beginning again");

	if (is_recording())
	{
		return VK_NOT_READY;
	}

	state_ = State::Recording;

	// Reset state
	pipeline_state_.reset();
	resource_binding_state_.reset();
	descriptor_set_layout_binding_state_.clear();
	stored_push_constants_.clear();

	VkCommandBufferBeginInfo       begin_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
	VkCommandBufferInheritanceInfo inheritance = { VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO };
	begin_info.flags = flags;

	if (level_ == VK_COMMAND_BUFFER_LEVEL_SECONDARY)
	{
		assert((render_pass && framebuffer) && "Render pass and framebuffer must be provided when calling begin from a secondary one");

		current_render_pass_.render_pass_ = render_pass;
		current_render_pass_.framebuffer_ = framebuffer;

		inheritance.renderPass = current_render_pass_.render_pass_->get_handle();
		inheritance.framebuffer = current_render_pass_.framebuffer_->get_handle();
		inheritance.subpass = subpass_index;

		begin_info.pInheritanceInfo = &inheritance;
	}

	return vkBeginCommandBuffer(get_handle(), &begin_info);
}

VkResult SPCommandBuffer::end()
{
	assert(is_recording() && "Command buffer is not recording, please call begin before end");

	if (!is_recording())
	{
		return VK_NOT_READY;
	}

	vkEndCommandBuffer(get_handle());

	state_ = State::Executable;

	return VK_SUCCESS;
}

const SPCommandBuffer::RenderPassBinding& SPCommandBuffer::get_current_render_pass() const
{
	return current_render_pass_;
}

const uint32_t SPCommandBuffer::get_current_subpass_index() const
{
	return pipeline_state_.get_subpass_index();
}

bool SPCommandBuffer::is_recording() const
{
	return state_ == State::Recording;
}

VkResult SPCommandBuffer::reset(ResetMode reset_mode)
{
	VkResult result = VK_SUCCESS;

	assert(reset_mode == command_pool_.get_reset_mode() && "Command buffer reset mode must match the one used by the pool to allocate it");

	state_ = State::Initial;

	if (reset_mode == ResetMode::ResetIndividually)
	{
		result = vkResetCommandBuffer(handle_, VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT);
	}

	return result;
}

void SPCommandBuffer::image_memory_barrier(const SPImageView& image_view, const ImageMemoryBarrier& memory_barrier) const
{

	// Adjust barrier's subresource range for depth images
	auto subresource_range = image_view.get_subresource_range();
	auto format = image_view.get_format();
	if (vkTool::is_depth_only_format(format))
	{
		subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else if (vkTool::is_depth_stencil_format(format))
	{
		subresource_range.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	}

	VkImageMemoryBarrier image_memory_barrier{ VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER };
	image_memory_barrier.oldLayout = memory_barrier.old_layout_;
	image_memory_barrier.newLayout = memory_barrier.new_layout_;
	image_memory_barrier.image = image_view.get_image().get_handle();
	image_memory_barrier.subresourceRange = subresource_range;
	image_memory_barrier.srcAccessMask = memory_barrier.src_access_mask_;
	image_memory_barrier.dstAccessMask = memory_barrier.dst_access_mask_;
	image_memory_barrier.srcQueueFamilyIndex = memory_barrier.old_queue_family_;
	image_memory_barrier.dstQueueFamilyIndex = memory_barrier.new_queue_family_;

	VkPipelineStageFlags src_stage_mask = memory_barrier.src_stage_mask_;
	VkPipelineStageFlags dst_stage_mask = memory_barrier.dst_stage_mask_;

	vkCmdPipelineBarrier(
		get_handle(),
		src_stage_mask,
		dst_stage_mask,
		0,
		0, nullptr,
		0, nullptr,
		1,
		&image_memory_barrier);
}

void SPCommandBuffer::buffer_memory_barrier(const SPBuffer& buffer, VkDeviceSize offset, VkDeviceSize size, const BufferMemoryBarrier& memory_barrier)
{
	VkBufferMemoryBarrier buffer_memory_barrier{ VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER };
	buffer_memory_barrier.srcAccessMask = memory_barrier.src_access_mask_;
	buffer_memory_barrier.dstAccessMask = memory_barrier.dst_access_mask_;
	buffer_memory_barrier.buffer = buffer.get_handle();
	buffer_memory_barrier.offset = offset;
	buffer_memory_barrier.size = size;

	VkPipelineStageFlags src_stage_mask = memory_barrier.src_stage_mask_;
	VkPipelineStageFlags dst_stage_mask = memory_barrier.dst_stage_mask_;

	vkCmdPipelineBarrier(
		get_handle(),
		src_stage_mask,
		dst_stage_mask,
		0,
		0, nullptr,
		1, &buffer_memory_barrier,
		0, nullptr);
}

void SPCommandBuffer::copy_buffer_to_image(const SPBuffer& buffer, const SPImage& image, const std::vector<VkBufferImageCopy>& regions)
{
	vkCmdCopyBufferToImage(get_handle(), buffer.get_handle(),
		image.get_handle(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		sp::to_u32(regions.size()), regions.data());
}

void SPCommandBuffer::copy_buffer(const SPBuffer& src_buffer, const SPBuffer& dst_buffer, VkDeviceSize size)
{
	VkBufferCopy copy_region = {};
	copy_region.size = size;
	vkCmdCopyBuffer(get_handle(), src_buffer.get_handle(), dst_buffer.get_handle(), 1, &copy_region);
}

