#ifndef S_SPQUEUE_H_
#define S_SPQUEUE_H_

#include "../vulkan_common.h"
#include "SPCommandBuffer.h"

class SPDevice;
class SPQueue
{
public:
	SPQueue(SPDevice& device, uint32_t family_index, VkQueueFamilyProperties properties, VkBool32 can_present, uint32_t index);
	~SPQueue() = default;
	SPQueue(const SPQueue&) = default;
	SPQueue(SPQueue&& other);
	SPQueue& operator=(const SPQueue&) = delete;
	SPQueue& operator=(SPQueue&&) = delete;
	void init();
public:
	VkQueueFamilyProperties get_properties() const;
	uint32_t get_family_index() const;
	VkBool32 support_present() const;
	const SPDevice& get_device() const;
	VkQueue get_handle() const;
public:
	VkResult submit(const SPCommandBuffer& command_buffer, VkFence fence) const;
	VkResult submit(const std::vector<VkSubmitInfo>& submit_infos, VkFence fence) const;
	VkResult present(const VkPresentInfoKHR& present_infos) const;
	VkResult wait_idle() const;
protected:
	SPDevice& device_;
	uint32_t family_index_;
	VkQueueFamilyProperties properties_;
	VkBool32 can_present_;
	uint32_t index_;
	VkQueue handle_;
};

#endif