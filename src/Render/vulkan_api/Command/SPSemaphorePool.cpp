#include "SPSemaphorePool.h"
#include "../Driver/SPDevice.h"


SPSemaphorePool::SPSemaphorePool(SPDevice& device)
	: device_{ device }
	, active_semaphore_count_{0}
{

}

SPSemaphorePool::~SPSemaphorePool()
{
	reset();

	// Destroy all semaphores
	for (VkSemaphore semaphore : semaphores_)
	{
		vkDestroySemaphore(device_.get_handle(), semaphore, nullptr);
	}

	semaphores_.clear();
}

void SPSemaphorePool::reset()
{
	active_semaphore_count_ = 0;

	// Now we can safely recycle the released semaphores.
	for (auto& sem : released_semaphores_)
	{
		semaphores_.push_back(sem);
	}

	released_semaphores_.clear();
}
