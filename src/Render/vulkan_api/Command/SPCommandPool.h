#ifndef S_SPCOMMANDPOOL_H_
#define S_SPCOMMANDPOOL_H_

#include "../vulkan_common.h"
#include "SPCommandBuffer.h"

class SPDevice;
class SPRenderFrame;
class SPCommandPool
{
public:
	SPCommandPool(SPDevice& device, uint32_t queue_family_index,SPRenderFrame* render_frame = nullptr,
		size_t                   thread_index = 0,
		SPCommandBuffer::ResetMode reset_mode = SPCommandBuffer::ResetMode::ResetPool);
	SPCommandPool(const SPCommandPool&) = delete;
	SPCommandPool(SPCommandPool&& other);
	~SPCommandPool();
	SPCommandPool& operator=(const SPCommandPool&) = delete;

	SPCommandPool& operator=(SPCommandPool&&) = delete;
public:
	SPDevice& get_device();
	VkCommandPool get_handle() const;
	SPCommandBuffer& request_command_buffer(VkCommandBufferLevel level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
	VkResult reset_pool();
	VkResult reset_command_buffers();
	const SPCommandBuffer::ResetMode get_reset_mode() const;
protected:

protected:
	SPDevice& device_;
	SPRenderFrame* render_frame_;
	size_t thread_index_;
	SPCommandBuffer::ResetMode reset_mode_;
	uint32_t queue_family_index_;

	VkCommandPool handle_;

	std::vector<std::unique_ptr<SPCommandBuffer>> primary_command_buffers_;
	uint32_t active_primary_command_buffer_count_;

	std::vector<std::unique_ptr<SPCommandBuffer>> secondary_command_buffers_;
	uint32_t active_secondary_command_buffer_count_;
};

#endif