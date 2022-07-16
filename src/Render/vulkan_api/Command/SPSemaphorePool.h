#ifndef S_SPSEMAPHOREPOOL_H_
#define S_SPSEMAPHOREPOOL_H_

#include "../vulkan_common.h"

class SPDevice;
class SPSemaphorePool
{
public:
	SPSemaphorePool(SPDevice& device);
	SPSemaphorePool(const SPSemaphorePool&) = delete;
	SPSemaphorePool(SPSemaphorePool&& other) = delete;
	~SPSemaphorePool();
	SPSemaphorePool& operator=(const SPSemaphorePool&) = delete;
	SPSemaphorePool& operator=(SPSemaphorePool&&) = delete;
public:
	void reset();
protected:
	SPDevice& device_;
	std::vector<VkSemaphore> semaphores_;
	std::vector<VkSemaphore> released_semaphores_;
	uint32_t active_semaphore_count_;
};

#endif