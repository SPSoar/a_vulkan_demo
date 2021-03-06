#include "SPFencePool.h"
#include "../Driver/SPDevice.h"

SPFencePool::SPFencePool(SPDevice& device)
	: device_(device)
	, active_fence_count_(0)
{
}

SPFencePool::~SPFencePool()
{
	wait();
	reset();

	// Destroy all fences
	for (VkFence fence : fences_)
	{
		vkDestroyFence(device_.get_handle(), fence, nullptr);
	}

	fences_.clear();
}

void SPFencePool::init()
{
}

VkResult SPFencePool::wait(uint32_t timeout) const
{
	if (active_fence_count_ < 1 || fences_.empty())
	{
		return VK_SUCCESS;
	}

	return vkWaitForFences(device_.get_handle(), active_fence_count_, fences_.data(), true, timeout);
}

VkResult SPFencePool::reset()
{
	if (active_fence_count_ < 1 || fences_.empty())
	{
		return VK_SUCCESS;
	}

	VkResult result = vkResetFences(device_.get_handle(), active_fence_count_, fences_.data());

	if (result != VK_SUCCESS)
	{
		return result;
	}

	active_fence_count_ = 0;

	return VK_SUCCESS;
}

VkFence SPFencePool::request_fence()
{
	// Check if there is an available fence
	if (active_fence_count_ < fences_.size())
	{
		return fences_.at(active_fence_count_++);
	}

	VkFence fence{ VK_NULL_HANDLE };

	VkFenceCreateInfo create_info{ VK_STRUCTURE_TYPE_FENCE_CREATE_INFO };

	VkResult result = vkCreateFence(device_.get_handle(), &create_info, nullptr, &fence);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create fence.");
	}

	fences_.push_back(fence);

	active_fence_count_++;

	return fences_.back();
}
