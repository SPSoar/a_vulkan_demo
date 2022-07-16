#ifndef S_SPSAMPLER_H_
#define S_SPSAMPLER_H_

#include "../vulkan_common.h"

class SPDevice;
class SPSampler
{
public:
	SPSampler(SPDevice const& device, const VkSamplerCreateInfo& info);
	SPSampler(const SPSampler&) = delete;
	SPSampler(SPSampler&& SPSampler);
	~SPSampler();
	SPSampler& operator=(const SPSampler&) = delete;
	SPSampler& operator=(SPSampler&&) = delete;
protected:
	SPDevice const& device_;
	VkSampler handle_;
};

#endif