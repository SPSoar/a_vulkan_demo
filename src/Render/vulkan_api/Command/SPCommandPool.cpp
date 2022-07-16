#include "SPCommandPool.h"
#include "../Driver/SPDevice.h"

SPCommandPool::SPCommandPool(SPDevice& device, uint32_t queue_family_index, SPRenderFrame* render_frame, size_t thread_index, SPCommandBuffer::ResetMode reset_mode)
    : device_(device)
    , render_frame_(render_frame)
    , thread_index_(thread_index)
    , reset_mode_(reset_mode)
    , queue_family_index_(queue_family_index)
	, active_primary_command_buffer_count_(0)
	, active_secondary_command_buffer_count_(0)
{
	VkCommandPoolCreateFlags flags;
	switch (reset_mode)
	{
	case SPCommandBuffer::ResetMode::ResetIndividually:
	case SPCommandBuffer::ResetMode::AlwaysAllocate:
		flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		break;
	case SPCommandBuffer::ResetMode::ResetPool:
	default:
		flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		break;
	}

	VkCommandPoolCreateInfo create_info{ VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO };

	create_info.queueFamilyIndex = queue_family_index;
	create_info.flags = flags;

	auto result = vkCreateCommandPool(device_.get_handle(), &create_info, nullptr, &handle_);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Failed to create command pool" };
	}
}

SPCommandPool::SPCommandPool(SPCommandPool&& other) 
	: handle_(other.handle_) 
	, device_(other.device_)
	, render_frame_(other.render_frame_)
	, thread_index_(other.thread_index_)
	, reset_mode_(other.reset_mode_)
	, queue_family_index_(other.queue_family_index_)
	, active_primary_command_buffer_count_(other.active_primary_command_buffer_count_)
	, active_secondary_command_buffer_count_(other.active_secondary_command_buffer_count_)
	, primary_command_buffers_(std::move(other.primary_command_buffers_))
	, secondary_command_buffers_(std::move(other.secondary_command_buffers_))
{
	other.handle_ = VK_NULL_HANDLE;

	other.queue_family_index_ = 0;

	other.active_primary_command_buffer_count_ = 0;

	other.active_secondary_command_buffer_count_ = 0;
}

SPCommandPool::~SPCommandPool()
{
	primary_command_buffers_.clear();
	secondary_command_buffers_.clear();

	// Destroy command pool
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyCommandPool(device_.get_handle(), handle_, nullptr);
	}
}

SPDevice& SPCommandPool::get_device()
{
	return device_;
}

VkCommandPool SPCommandPool::get_handle() const
{
	return handle_;
}


SPCommandBuffer& SPCommandPool::request_command_buffer(VkCommandBufferLevel level)
{
	if (level == VK_COMMAND_BUFFER_LEVEL_PRIMARY)
	{
		if (active_primary_command_buffer_count_ < primary_command_buffers_.size())
		{
			return *primary_command_buffers_.at(active_primary_command_buffer_count_++);
		}

		primary_command_buffers_.emplace_back(std::make_unique<SPCommandBuffer>(*this, level));

		active_primary_command_buffer_count_++;

		return *primary_command_buffers_.back();
	}
	else
	{
		if (active_secondary_command_buffer_count_ < secondary_command_buffers_.size())
		{
			return *secondary_command_buffers_.at(active_secondary_command_buffer_count_++);
		}

		secondary_command_buffers_.emplace_back(std::make_unique<SPCommandBuffer>(*this, level));

		active_secondary_command_buffer_count_++;

		return *secondary_command_buffers_.back();
	}
}

VkResult SPCommandPool::reset_pool()
{
	VkResult result = VK_SUCCESS;

	switch (reset_mode_)
	{
	case SPCommandBuffer::ResetMode::ResetIndividually:
	{
		result = reset_command_buffers();

		break;
	}
	case SPCommandBuffer::ResetMode::ResetPool:
	{
		result = vkResetCommandPool(device_.get_handle(), handle_, 0);

		if (result != VK_SUCCESS)
		{
			return result;
		}

		result = reset_command_buffers();

		break;
	}
	case SPCommandBuffer::ResetMode::AlwaysAllocate:
	{
		primary_command_buffers_.clear();
		active_primary_command_buffer_count_ = 0;

		secondary_command_buffers_.clear();
		active_secondary_command_buffer_count_ = 0;

		break;
	}
	default:
		throw std::runtime_error("Unknown reset mode for command pools");
	}

	return result;
}

VkResult SPCommandPool::reset_command_buffers()
{
	VkResult result = VK_SUCCESS;

	for (auto& cmd_buf : primary_command_buffers_)
	{
		result = cmd_buf->reset(reset_mode_);

		if (result != VK_SUCCESS)
		{
			return result;
		}
	}

	active_primary_command_buffer_count_ = 0;

	for (auto& cmd_buf : secondary_command_buffers_)
	{
		result = cmd_buf->reset(reset_mode_);

		if (result != VK_SUCCESS)
		{
			return result;
		}
	}

	active_secondary_command_buffer_count_ = 0;

	return result;
}

const SPCommandBuffer::ResetMode SPCommandPool::get_reset_mode() const
{
	return reset_mode_;
}
