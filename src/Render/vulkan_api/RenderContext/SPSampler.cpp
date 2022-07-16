#include "SPSampler.h"
#include "../Driver/SPDevice.h"

SPSampler::SPSampler(SPDevice const& device, const VkSamplerCreateInfo& info)
	: device_{ device }
{
	VK_CHECK(vkCreateSampler(device_.get_handle(), &info, nullptr, &handle_));
}

SPSampler::SPSampler(SPSampler&& SPSampler)
	: device_{ SPSampler.device_}
	, handle_{ SPSampler.handle_ }
{
	SPSampler.handle_ = VK_NULL_HANDLE;
}

SPSampler::~SPSampler()
{
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroySampler(device_.get_handle(), handle_, nullptr);
	}
}
