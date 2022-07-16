#ifndef S_SPFENCEPOOL_H_
#define S_SPFENCEPOOL_H_

#include "../vulkan_common.h"

class SPDevice;
class SPFencePool
{
public:
	SPFencePool(SPDevice& device);
	SPFencePool(const SPFencePool&) = delete;
	SPFencePool(SPFencePool&& other) = delete;
	~SPFencePool();
	SPFencePool& operator=(const SPFencePool&) = delete;
	SPFencePool& operator=(SPFencePool&&) = delete;
	void init();
public:
	VkResult wait(uint32_t timeout = std::numeric_limits<uint32_t>::max()) const;
	VkResult reset();
	VkFence request_fence();
protected:
	SPDevice& device_;
	std::vector<VkFence> fences_;
	uint32_t active_fence_count_;
};

#endif