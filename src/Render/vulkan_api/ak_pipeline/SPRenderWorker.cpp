#include "SPRenderWorker.h"

#include "../ak_engine/SPEngine.h"
#include "../ak_render/SPRenderContext.h"


#include "../ak_scene/SPScene.h"

#include "../Driver/SPDevice.h"
#include "../Buffer/SPBuffer.h"
#include "../Command/SPQueue.h"
#include "../Command/SPCommandPool.h"

#include <random>

#include "../vk_tool/glsl_compiler.h"

#include "../RenderContext/SPSwapchain.h"

inline uint32_t aligned_size(uint32_t value, uint32_t alignment)
{
	return (value + alignment - 1) & ~(alignment - 1);
}

SPRenderWorker::SPRenderWorker(SPRenderContext& render_context, SPScene& scene, SPEngine& engine)
	: render_context_{ render_context }
	, engine_ { engine }
	, scene_{ scene }
{
}

SPRenderWorker::~SPRenderWorker()
{
}

void SPRenderWorker::init()
{
	create_command_buffers();
	create_synchronization_primitives();
	setup_depth_stencil();
	setup_render_pass();
	setup_framebuffer();

	scene_.create_camera(get_with(), get_height());

	//---------------------------------------
	// This sample copies the ray traced output to the swap chain image, so we need to enable the required image usage flags
	const std::set<VkImageUsageFlagBits> image_usage_flags = { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_DST_BIT };
	update_swapchain_image_usage_flags(image_usage_flags);
	
	// This sample renders the UI overlay on top of the ray tracing output, so we need to disable color attachment clears
	update_render_pass_flags(RenderPassCreateFlags::ColorAttachmentLoad);

	// Get the ray tracing pipeline properties, which we'll need later on in the sample
	ray_tracing_pipeline_properties_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
	VkPhysicalDeviceProperties2 device_properties{};
	device_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
	device_properties.pNext = &ray_tracing_pipeline_properties_;
	vkGetPhysicalDeviceProperties2(engine_.get_device().get_gpu().get_handle(), &device_properties);

	// Get the acceleration structure features, which we'll need later on in the sample
	acceleration_structure_features_.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
	VkPhysicalDeviceFeatures2 device_features{};
	device_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	device_features.pNext = &acceleration_structure_features_;
	vkGetPhysicalDeviceFeatures2(engine_.get_device().get_gpu().get_handle(), &device_features);

	create_storage_image();

	scene_.create_scene();
	scene_.create_uniform_buffer();

	create_ray_tracing_pipeline();
	create_shader_binding_tables();

	create_descriptor_sets();
}

void SPRenderWorker::resize(const sp_uint_32& width, const sp_uint_32& height)
{
	// Recreate the frame buffers
	vkDestroyImageView(engine_.get_device().get_handle(), depth_stencil_.view_, nullptr);
	vkDestroyImage(engine_.get_device().get_handle(), depth_stencil_.image_, nullptr);
	vkFreeMemory(engine_.get_device().get_handle(), depth_stencil_.mem_, nullptr);
	setup_depth_stencil();

	setup_framebuffer();

	// Command buffers need to be recreated as they may store
	// references to the recreated frame buffer
	destroy_command_buffers();
	create_command_buffers();
	build_command_buffers();

	engine_.get_device().wait_idle();

	if ((width > 0.0f) && (height > 0.0f))
	{
		scene_.camera_.update_aspect_ratio((float)width / (float)height);
	}
}

uint16_t SPRenderWorker::get_with()
{
	return render_context_.get_surface_extent().width;
}

uint16_t SPRenderWorker::get_height()
{
	return render_context_.get_surface_extent().height;
}

void SPRenderWorker::build_command_buffers()
{
	if (get_with() != storage_image_.width_ || get_height() != storage_image_.height_)
	{
		// If the view port size has changed, we need to recreate the storage image
		vkDestroyImageView(engine_.get_device().get_handle(), storage_image_.view_, nullptr);
		vkDestroyImage(engine_.get_device().get_handle(), storage_image_.image_, nullptr);
		vkFreeMemory(engine_.get_device().get_handle(), storage_image_.memory_, nullptr);
		create_storage_image();
		// The descriptor also needs to be updated to reference the new image
		VkDescriptorImageInfo image_descriptor{};
		image_descriptor.imageView = storage_image_.view_;
		image_descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
		VkWriteDescriptorSet result_image_write = vkTool::write_descriptor_set(descriptor_set_, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, &image_descriptor);
		vkUpdateDescriptorSets(engine_.get_device().get_handle(), 1, &result_image_write, 0, VK_NULL_HANDLE);
		build_command_buffers();
	}

	VkCommandBufferBeginInfo command_buffer_begin_info = vkTool::command_buffer_begin_info();

	VkImageSubresourceRange subresource_range = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	for (int32_t i = 0; i < draw_cmd_buffers_.size(); ++i)
	{
		VK_CHECK(vkBeginCommandBuffer(draw_cmd_buffers_[i], &command_buffer_begin_info));

		/*
			Setup the strided device address regions pointing at the shader identifiers in the shader binding table
		*/

		const uint32_t handle_size_aligned = aligned_size(ray_tracing_pipeline_properties_.shaderGroupHandleSize, ray_tracing_pipeline_properties_.shaderGroupHandleAlignment);

		VkStridedDeviceAddressRegionKHR raygen_shader_sbt_entry{};
		raygen_shader_sbt_entry.deviceAddress = get_buffer_device_address(raygen_shader_binding_table_->get_handle());
		raygen_shader_sbt_entry.stride = handle_size_aligned;
		raygen_shader_sbt_entry.size = handle_size_aligned;

		VkStridedDeviceAddressRegionKHR miss_shader_sbt_entry{};
		miss_shader_sbt_entry.deviceAddress = get_buffer_device_address(miss_shader_binding_table_->get_handle());
		miss_shader_sbt_entry.stride = handle_size_aligned;
		miss_shader_sbt_entry.size = handle_size_aligned;

		VkStridedDeviceAddressRegionKHR hit_shader_sbt_entry{};
		hit_shader_sbt_entry.deviceAddress = get_buffer_device_address(hit_shader_binding_table_->get_handle());
		hit_shader_sbt_entry.stride = handle_size_aligned;
		hit_shader_sbt_entry.size = handle_size_aligned;

		VkStridedDeviceAddressRegionKHR callable_shader_sbt_entry{};

		/*
			Dispatch the ray tracing commands
		*/
		vkCmdBindPipeline(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline_);
		vkCmdBindDescriptorSets(draw_cmd_buffers_[i], VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, pipeline_layout_, 0, 1, &descriptor_set_, 0, 0);

		vkCmdTraceRaysKHR(
			draw_cmd_buffers_[i],
			&raygen_shader_sbt_entry,
			&miss_shader_sbt_entry,
			&hit_shader_sbt_entry,
			&callable_shader_sbt_entry,
			get_with(),
			get_height(),
			1);

		/*
			Copy ray tracing output to swap chain image
		*/

		// Prepare current swap chain image as transfer destination
		vkTool::set_image_layout(
			draw_cmd_buffers_[i],
			render_context_.get_swapchain().get_images()[i],
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			subresource_range);

		// Prepare ray tracing output image as transfer source
		vkTool::set_image_layout(
			draw_cmd_buffers_[i],
			storage_image_.image_,
			VK_IMAGE_LAYOUT_GENERAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			subresource_range);

		VkImageCopy copy_region{};
		copy_region.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		copy_region.srcOffset = { 0, 0, 0 };
		copy_region.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		copy_region.dstOffset = { 0, 0, 0 };
		copy_region.extent = { get_with(), get_height(), 1};
		vkCmdCopyImage(draw_cmd_buffers_[i], storage_image_.image_, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			render_context_.get_swapchain().get_images()[i], VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy_region);

		// Transition swap chain image back for presentation
		vkTool::set_image_layout(
			draw_cmd_buffers_[i],
			render_context_.get_swapchain().get_images()[i],
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			subresource_range);

		// Transition ray tracing output image back to general layout
		vkTool::set_image_layout(
			draw_cmd_buffers_[i],
			storage_image_.image_,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_GENERAL,
			subresource_range);

		/*
			Start a new render pass to draw the UI overlay on top of the ray traced image
		*/
		VkClearValue clear_values[2];
		clear_values[0].color = { {0.0f, 0.0f, 0.2f, 0.0f} };
		clear_values[1].depthStencil = { 0.0f, 0 };

		VkRenderPassBeginInfo render_pass_begin_info = vkTool::render_pass_begin_info();
		render_pass_begin_info.renderPass = render_pass_;
		render_pass_begin_info.framebuffer = framebuffers_[i];
		render_pass_begin_info.renderArea.extent.width = get_with();
		render_pass_begin_info.renderArea.extent.height = get_height();
		render_pass_begin_info.clearValueCount = 2;
		render_pass_begin_info.pClearValues = clear_values;

		vkCmdBeginRenderPass(draw_cmd_buffers_[i], &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
		//draw_ui(draw_cmd_buffers_[i]);
		vkCmdEndRenderPass(draw_cmd_buffers_[i]);

		VK_CHECK(vkEndCommandBuffer(draw_cmd_buffers_[i]));
	}
}

void SPRenderWorker::update_uniform_buffers(sp_decimal_d d)
{
	scene_.update_uniform_buffers();
}

void SPRenderWorker::update_render_pass_flags(uint32_t flags)
{
	vkDestroyRenderPass(engine_.get_device().get_handle(), render_pass_, nullptr);

	VkAttachmentLoadOp  color_attachment_load_op = VK_ATTACHMENT_LOAD_OP_CLEAR;
	VkAttachmentStoreOp color_attachment_store_op = VK_ATTACHMENT_STORE_OP_STORE;
	VkImageLayout       color_attachment_image_layout = VK_IMAGE_LAYOUT_UNDEFINED;

	// Samples can keep the color attachment contents, e.g. if they have previously written to the swap chain images
	if (flags & RenderPassCreateFlags::ColorAttachmentLoad)
	{
		color_attachment_load_op = VK_ATTACHMENT_LOAD_OP_LOAD;
		color_attachment_image_layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	}

	std::array<VkAttachmentDescription, 2> attachments = {};
	// Color attachment
	attachments[0].format = render_context_.get_format();
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = color_attachment_load_op;
	attachments[0].storeOp = color_attachment_store_op;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = color_attachment_image_layout;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// Depth attachment
	attachments[1].format = render_context_.get_depth_format();
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_reference = {};
	color_reference.attachment = 0;
	color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_reference = {};
	depth_reference.attachment = 1;
	depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_description = {};
	subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description.colorAttachmentCount = 1;
	subpass_description.pColorAttachments = &color_reference;
	subpass_description.pDepthStencilAttachment = &depth_reference;
	subpass_description.inputAttachmentCount = 0;
	subpass_description.pInputAttachments = nullptr;
	subpass_description.preserveAttachmentCount = 0;
	subpass_description.pPreserveAttachments = nullptr;
	subpass_description.pResolveAttachments = nullptr;

	// Subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
	render_pass_create_info.pAttachments = attachments.data();
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pSubpasses = &subpass_description;
	render_pass_create_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
	render_pass_create_info.pDependencies = dependencies.data();

	VK_CHECK(vkCreateRenderPass(engine_.get_device().get_handle(), &render_pass_create_info, nullptr, &render_pass_))
}
				 
void SPRenderWorker::draw_model(std::unique_ptr<SPSSubMesh>& model, VkCommandBuffer command_buffer)
{
	VkDeviceSize offsets[1] = { 0 };

	const auto& vertex_buffer = model->vertex_buffers_.at("vertex_buffer");
	auto& index_buffer = model->index_buffer_;

	vkCmdBindVertexBuffers(command_buffer, 0, 1, vertex_buffer.get(), offsets);
	vkCmdBindIndexBuffer(command_buffer, index_buffer->get_handle(), 0, model->index_type_);
	vkCmdDrawIndexed(command_buffer, model->vertex_indices_, 1, 0, 0, 0);
}

VkPipelineShaderStageCreateInfo SPRenderWorker::load_shader(const std::string& file, VkShaderStageFlagBits stage)
{
	VkPipelineShaderStageCreateInfo shader_stage = {};
	shader_stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shader_stage.stage = stage;
	shader_stage.module = vkTool::load_shader(file.c_str(), engine_.get_device().get_handle(), stage);
	shader_stage.pName = "main";
	assert(shader_stage.module != VK_NULL_HANDLE);
	//shader_modules_.push_back(shader_stage.module);
	return shader_stage;
}

VkDescriptorBufferInfo SPRenderWorker::create_descriptor(SPBuffer& buffer, VkDeviceSize size, VkDeviceSize offset)
{
	VkDescriptorBufferInfo descriptor{};
	descriptor.buffer = buffer.get_handle();
	descriptor.range = size;
	descriptor.offset = offset;
	return descriptor;
}

uint64_t SPRenderWorker::get_buffer_device_address(VkBuffer buffer)
{
	VkBufferDeviceAddressInfoKHR buffer_device_address_info{};
	buffer_device_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	buffer_device_address_info.buffer = buffer;
	return vkGetBufferDeviceAddressKHR(engine_.get_device().get_handle(), &buffer_device_address_info);

}

void SPRenderWorker::create_command_buffers()
{
	// Create one command buffer for each swap chain image and reuse for rendering
	draw_cmd_buffers_.resize(render_context_.get_render_frames().size());

	VkCommandBufferAllocateInfo allocate_info =
		vkTool::command_buffer_allocate_info(
			engine_.get_command_pool().get_handle(),
			VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			static_cast<uint32_t>(draw_cmd_buffers_.size()));

	VK_CHECK(vkAllocateCommandBuffers(engine_.get_device().get_handle(), &allocate_info, draw_cmd_buffers_.data()));

}
void SPRenderWorker::destroy_command_buffers()
{
	vkFreeCommandBuffers(engine_.get_device().get_handle(), engine_.get_command_pool().get_handle(), static_cast<uint32_t>(draw_cmd_buffers_.size()), draw_cmd_buffers_.data());

}
void SPRenderWorker::create_synchronization_primitives()
{
	// Wait fences to sync command buffer access
	VkFenceCreateInfo fence_create_info = vkTool::fence_create_info(VK_FENCE_CREATE_SIGNALED_BIT);
	wait_fences_.resize(draw_cmd_buffers_.size());
	for (auto& fence : wait_fences_)
	{
		VK_CHECK(vkCreateFence(engine_.get_device().get_handle(), &fence_create_info, nullptr, &fence));
	}
}

void SPRenderWorker::setup_depth_stencil()
{
	VkImageCreateInfo image_create_info{};
	image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	image_create_info.imageType = VK_IMAGE_TYPE_2D;
	image_create_info.format = render_context_.get_depth_format();
	image_create_info.extent = { get_with(), get_height(), 1};
	image_create_info.mipLevels = 1;
	image_create_info.arrayLayers = 1;
	image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	image_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

	VK_CHECK(vkCreateImage(engine_.get_device().get_handle(), &image_create_info, nullptr, &depth_stencil_.image_));
	VkMemoryRequirements memReqs{};
	vkGetImageMemoryRequirements(engine_.get_device().get_handle(), depth_stencil_.image_, &memReqs);

	VkMemoryAllocateInfo memory_allocation{};
	memory_allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocation.allocationSize = memReqs.size;
	memory_allocation.memoryTypeIndex = engine_.get_device().get_memory_type(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(engine_.get_device().get_handle(), &memory_allocation, nullptr, &depth_stencil_.mem_));
	VK_CHECK(vkBindImageMemory(engine_.get_device().get_handle(), depth_stencil_.image_, depth_stencil_.mem_, 0));

	VkImageViewCreateInfo image_view_create_info{};
	image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	image_view_create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	image_view_create_info.image = depth_stencil_.image_;
	image_view_create_info.format = render_context_.get_depth_format();
	image_view_create_info.subresourceRange.baseMipLevel = 0;
	image_view_create_info.subresourceRange.levelCount = 1;
	image_view_create_info.subresourceRange.baseArrayLayer = 0;
	image_view_create_info.subresourceRange.layerCount = 1;
	image_view_create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
	if (render_context_.get_depth_format() >= VK_FORMAT_D16_UNORM_S8_UINT)
	{
		image_view_create_info.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
	}
	VK_CHECK(vkCreateImageView(engine_.get_device().get_handle(), &image_view_create_info, nullptr, &depth_stencil_.view_));

}

void SPRenderWorker::setup_render_pass()
{
	std::array<VkAttachmentDescription, 2> attachments = {};
	// Color attachment
	attachments[0].format = render_context_.get_format();
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// Depth attachment
	attachments[1].format = render_context_.get_depth_format();
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_reference = {};
	color_reference.attachment = 0;
	color_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depth_reference = {};
	depth_reference.attachment = 1;
	depth_reference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_description = {};
	subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_description.colorAttachmentCount = 1;
	subpass_description.pColorAttachments = &color_reference;
	subpass_description.pDepthStencilAttachment = &depth_reference;
	subpass_description.inputAttachmentCount = 0;
	subpass_description.pInputAttachments = nullptr;
	subpass_description.preserveAttachmentCount = 0;
	subpass_description.pPreserveAttachments = nullptr;
	subpass_description.pResolveAttachments = nullptr;

	// Subpass dependencies for layout transitions
	std::array<VkSubpassDependency, 2> dependencies;

	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo render_pass_create_info = {};
	render_pass_create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
	render_pass_create_info.pAttachments = attachments.data();
	render_pass_create_info.subpassCount = 1;
	render_pass_create_info.pSubpasses = &subpass_description;
	render_pass_create_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
	render_pass_create_info.pDependencies = dependencies.data();

	VK_CHECK(vkCreateRenderPass(engine_.get_device().get_handle(), &render_pass_create_info, nullptr, &render_pass_));

}

void SPRenderWorker::setup_framebuffer()
{
	VkImageView attachments[2];

	// Depth/Stencil attachment is the same for all frame buffers
	attachments[1] = depth_stencil_.view_;

	VkFramebufferCreateInfo framebuffer_create_info = {};
	framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	framebuffer_create_info.pNext = NULL;
	framebuffer_create_info.renderPass = render_pass_;
	framebuffer_create_info.attachmentCount = 2;
	framebuffer_create_info.pAttachments = attachments;
	framebuffer_create_info.width = get_with();
	framebuffer_create_info.height = get_height();
	framebuffer_create_info.layers = 1;

	// Delete existing frame buffers
	if (framebuffers_.size() > 0)
	{
		for (uint32_t i = 0; i < framebuffers_.size(); i++)
		{
			if (framebuffers_[i] != VK_NULL_HANDLE)
			{
				vkDestroyFramebuffer(engine_.get_device().get_handle(), framebuffers_[i], nullptr);
			}
		}
	}

	// Create frame buffers for every swap chain image
	framebuffers_.resize(render_context_.get_render_frames().size());
	for (uint32_t i = 0; i < framebuffers_.size(); i++)
	{
		attachments[0] = render_context_.get_swapchain_buffers()[i].view_;
		VK_CHECK(vkCreateFramebuffer(engine_.get_device().get_handle(), &framebuffer_create_info, nullptr, &framebuffers_[i]));
	}
}

void SPRenderWorker::update_swapchain_image_usage_flags(std::set<VkImageUsageFlagBits> image_usage_flags)
{
	render_context_.update_swapchain(image_usage_flags);
	render_context_.create_swapchain_buffers();
	setup_framebuffer();
}

void SPRenderWorker::create_storage_image()
{
	storage_image_.width_ = get_with();
	storage_image_.height_ = get_height();

	VkImageCreateInfo image = vkTool::image_create_info();
	image.imageType = VK_IMAGE_TYPE_2D;
	image.format = VK_FORMAT_B8G8R8A8_UNORM;
	image.extent.width = storage_image_.width_;
	image.extent.height = storage_image_.height_;
	image.extent.depth = 1;
	image.mipLevels = 1;
	image.arrayLayers = 1;
	image.samples = VK_SAMPLE_COUNT_1_BIT;
	image.tiling = VK_IMAGE_TILING_OPTIMAL;
	image.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_STORAGE_BIT;
	image.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	VK_CHECK(vkCreateImage(engine_.get_device().get_handle(), &image, nullptr, &storage_image_.image_));

	VkMemoryRequirements memory_requirements;
	vkGetImageMemoryRequirements(engine_.get_device().get_handle(), storage_image_.image_, &memory_requirements);
	VkMemoryAllocateInfo memory_allocate_info = vkTool::memory_allocate_info();
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = engine_.get_device().get_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(engine_.get_device().get_handle(), &memory_allocate_info, nullptr, &storage_image_.memory_));
	VK_CHECK(vkBindImageMemory(engine_.get_device().get_handle(), storage_image_.image_, storage_image_.memory_, 0));

	VkImageViewCreateInfo color_image_view = vkTool::image_view_create_info();
	color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
	color_image_view.format = VK_FORMAT_B8G8R8A8_UNORM;
	color_image_view.subresourceRange = {};
	color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	color_image_view.subresourceRange.baseMipLevel = 0;
	color_image_view.subresourceRange.levelCount = 1;
	color_image_view.subresourceRange.baseArrayLayer = 0;
	color_image_view.subresourceRange.layerCount = 1;
	color_image_view.image = storage_image_.image_;
	VK_CHECK(vkCreateImageView(engine_.get_device().get_handle(), &color_image_view, nullptr, &storage_image_.view_));

	VkCommandBuffer command_buffer = engine_.get_device().create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	vkTool::set_image_layout(command_buffer, storage_image_.image_,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_GENERAL,
		{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

	auto queue = engine_.get_device().get_suitable_graphics_queue();

	engine_.get_device().flush_command_buffer(command_buffer, queue.get_handle());
}

void SPRenderWorker::create_ray_tracing_pipeline()
{
	// Slot for binding top level acceleration structures to the ray generation shader
	VkDescriptorSetLayoutBinding acceleration_structure_layout_binding{};
	acceleration_structure_layout_binding.binding = 0;
	acceleration_structure_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	acceleration_structure_layout_binding.descriptorCount = 1;
	acceleration_structure_layout_binding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

	VkDescriptorSetLayoutBinding result_image_layout_binding{};
	result_image_layout_binding.binding = 1;
	result_image_layout_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
	result_image_layout_binding.descriptorCount = 1;
	result_image_layout_binding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

	VkDescriptorSetLayoutBinding uniform_buffer_binding{};
	uniform_buffer_binding.binding = 2;
	uniform_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uniform_buffer_binding.descriptorCount = 1;
	uniform_buffer_binding.stageFlags = VK_SHADER_STAGE_RAYGEN_BIT_KHR;

	std::vector<VkDescriptorSetLayoutBinding> bindings = {
		acceleration_structure_layout_binding,
		result_image_layout_binding,
		uniform_buffer_binding };

	VkDescriptorSetLayoutCreateInfo layout_info{};
	layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
	layout_info.pBindings = bindings.data();
	VK_CHECK(vkCreateDescriptorSetLayout(engine_.get_device().get_handle(), &layout_info, nullptr, &descriptor_set_layout_));

	VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = 1;
	pipeline_layout_create_info.pSetLayouts = &descriptor_set_layout_;

	VK_CHECK(vkCreatePipelineLayout(engine_.get_device().get_handle(), &pipeline_layout_create_info, nullptr, &pipeline_layout_));

	// Ray tracing shaders require SPIR-V 1.4, so we need to set the appropriate target envinroment for the glslang compiler
	vkb::GLSLCompiler::set_target_environment(glslang::EShTargetSpv, glslang::EShTargetSpv_1_4);

	/*
		Setup ray tracing shader groups
		Each shader group points at the corresponding shader in the pipeline
	*/
	std::vector<VkPipelineShaderStageCreateInfo> shader_stages;

	// Ray generation group
	{
		shader_stages.push_back(load_shader("../../shaders/khr_ray_tracing_basic/raygen.rgen", VK_SHADER_STAGE_RAYGEN_BIT_KHR));
		VkRayTracingShaderGroupCreateInfoKHR raygen_group_ci{};
		raygen_group_ci.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		raygen_group_ci.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		raygen_group_ci.generalShader = static_cast<uint32_t>(shader_stages.size()) - 1;
		raygen_group_ci.closestHitShader = VK_SHADER_UNUSED_KHR;
		raygen_group_ci.anyHitShader = VK_SHADER_UNUSED_KHR;
		raygen_group_ci.intersectionShader = VK_SHADER_UNUSED_KHR;
		shader_groups_.push_back(raygen_group_ci);
	}

	// Ray miss group
	{
		shader_stages.push_back(load_shader("../../shaders/khr_ray_tracing_basic/miss.rmiss", VK_SHADER_STAGE_MISS_BIT_KHR));
		VkRayTracingShaderGroupCreateInfoKHR miss_group_ci{};
		miss_group_ci.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		miss_group_ci.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
		miss_group_ci.generalShader = static_cast<uint32_t>(shader_stages.size()) - 1;
		miss_group_ci.closestHitShader = VK_SHADER_UNUSED_KHR;
		miss_group_ci.anyHitShader = VK_SHADER_UNUSED_KHR;
		miss_group_ci.intersectionShader = VK_SHADER_UNUSED_KHR;
		shader_groups_.push_back(miss_group_ci);
	}

	// Ray closest hit group
	{
		shader_stages.push_back(load_shader("../../shaders/khr_ray_tracing_basic/closesthit.rchit", VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR));
		VkRayTracingShaderGroupCreateInfoKHR closes_hit_group_ci{};
		closes_hit_group_ci.sType = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
		closes_hit_group_ci.type = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
		closes_hit_group_ci.generalShader = VK_SHADER_UNUSED_KHR;
		closes_hit_group_ci.closestHitShader = static_cast<uint32_t>(shader_stages.size()) - 1;
		closes_hit_group_ci.anyHitShader = VK_SHADER_UNUSED_KHR;
		closes_hit_group_ci.intersectionShader = VK_SHADER_UNUSED_KHR;
		shader_groups_.push_back(closes_hit_group_ci);
	}

	/*
		Create the ray tracing pipeline
	*/
	VkRayTracingPipelineCreateInfoKHR raytracing_pipeline_create_info{};
	raytracing_pipeline_create_info.sType = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
	raytracing_pipeline_create_info.stageCount = static_cast<uint32_t>(shader_stages.size());
	raytracing_pipeline_create_info.pStages = shader_stages.data();
	raytracing_pipeline_create_info.groupCount = static_cast<uint32_t>(shader_groups_.size());
	raytracing_pipeline_create_info.pGroups = shader_groups_.data();
	raytracing_pipeline_create_info.maxPipelineRayRecursionDepth = 1;
	raytracing_pipeline_create_info.layout = pipeline_layout_;
	VK_CHECK(vkCreateRayTracingPipelinesKHR(engine_.get_device().get_handle(), VK_NULL_HANDLE, VK_NULL_HANDLE, 1, &raytracing_pipeline_create_info, nullptr, &pipeline_));
}

void SPRenderWorker::create_shader_binding_tables()
{
	const uint32_t           handle_size = ray_tracing_pipeline_properties_.shaderGroupHandleSize;
	const uint32_t           handle_size_aligned = aligned_size(ray_tracing_pipeline_properties_.shaderGroupHandleSize, ray_tracing_pipeline_properties_.shaderGroupHandleAlignment);
	const uint32_t           handle_alignment = ray_tracing_pipeline_properties_.shaderGroupHandleAlignment;
	const uint32_t           group_count = static_cast<uint32_t>(shader_groups_.size());
	const uint32_t           sbt_size = group_count * handle_size_aligned;
	const VkBufferUsageFlags sbt_buffer_usafge_flags = VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	const VmaMemoryUsage     sbt_memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

	// Raygen
	// Create binding table buffers for each shader type
	raygen_shader_binding_table_ = std::make_unique<SPBuffer>(engine_.get_device(), handle_size, sbt_buffer_usafge_flags, sbt_memory_usage, 0);
	miss_shader_binding_table_ = std::make_unique<SPBuffer>(engine_.get_device(), handle_size, sbt_buffer_usafge_flags, sbt_memory_usage, 0);
	hit_shader_binding_table_ = std::make_unique<SPBuffer>(engine_.get_device(), handle_size, sbt_buffer_usafge_flags, sbt_memory_usage, 0);

	// Copy the pipeline's shader handles into a host buffer
	std::vector<uint8_t> shader_handle_storage(sbt_size);
	VK_CHECK(vkGetRayTracingShaderGroupHandlesKHR(engine_.get_device().get_handle(), pipeline_, 0, group_count, sbt_size, shader_handle_storage.data()));

	// Copy the shader handles from the host buffer to the binding tables
	uint8_t* data = static_cast<uint8_t*>(raygen_shader_binding_table_->map());
	memcpy(data, shader_handle_storage.data(), handle_size);
	data = static_cast<uint8_t*>(miss_shader_binding_table_->map());
	memcpy(data, shader_handle_storage.data() + handle_size_aligned, handle_size);
	data = static_cast<uint8_t*>(hit_shader_binding_table_->map());
	memcpy(data, shader_handle_storage.data() + handle_size_aligned * 2, handle_size);
	raygen_shader_binding_table_->unmap();
	miss_shader_binding_table_->unmap();
	hit_shader_binding_table_->unmap();
}

void SPRenderWorker::create_descriptor_sets()
{
	std::vector<VkDescriptorPoolSize> pool_sizes = {
		{VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, 1},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1} };
	VkDescriptorPoolCreateInfo descriptor_pool_create_info = vkTool::descriptor_pool_create_info(pool_sizes, 1);
	VK_CHECK(vkCreateDescriptorPool(engine_.get_device().get_handle(), &descriptor_pool_create_info, nullptr, &descriptor_pool_));

	VkDescriptorSetAllocateInfo descriptor_set_allocate_info = vkTool::descriptor_set_allocate_info(descriptor_pool_, &descriptor_set_layout_, 1);
	VK_CHECK(vkAllocateDescriptorSets(engine_.get_device().get_handle(), &descriptor_set_allocate_info, &descriptor_set_));

	// Setup the descriptor for binding our top level acceleration structure to the ray tracing shaders
	VkWriteDescriptorSetAccelerationStructureKHR descriptor_acceleration_structure_info{};
	descriptor_acceleration_structure_info.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
	descriptor_acceleration_structure_info.accelerationStructureCount = 1;
	descriptor_acceleration_structure_info.pAccelerationStructures = &scene_.top_level_acceleration_structure_.handle_;

	VkWriteDescriptorSet acceleration_structure_write{};
	acceleration_structure_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	acceleration_structure_write.dstSet = descriptor_set_;
	acceleration_structure_write.dstBinding = 0;
	acceleration_structure_write.descriptorCount = 1;
	acceleration_structure_write.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
	// The acceleration structure descriptor has to be chained via pNext
	acceleration_structure_write.pNext = &descriptor_acceleration_structure_info;

	VkDescriptorImageInfo image_descriptor{};
	image_descriptor.imageView = storage_image_.view_;
	image_descriptor.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkDescriptorBufferInfo buffer_descriptor = create_descriptor(*scene_.ubo_);

	VkWriteDescriptorSet result_image_write = vkTool::write_descriptor_set(descriptor_set_, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1, &image_descriptor);
	VkWriteDescriptorSet uniform_buffer_write = vkTool::write_descriptor_set(descriptor_set_, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2, &buffer_descriptor);

	std::vector<VkWriteDescriptorSet> write_descriptor_sets = {
		acceleration_structure_write,
		result_image_write,
		uniform_buffer_write };
	vkUpdateDescriptorSets(engine_.get_device().get_handle(), static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, VK_NULL_HANDLE);

}
