#ifndef S_SPSSAMPLER_H_
#define S_SPSSAMPLER_H_

#include "../RenderContext/SPSampler.h"

#include "SPComponent.h"

class SPSSampler : public SPComponent
{ 
public:
	SPSSampler(const std::string& name, SPSampler&& vk_sampler);
	SPSSampler(SPSSampler&& other) = default;
	virtual ~SPSSampler() = default;
	virtual std::type_index get_type() override;
public:
	SPSampler vk_sampler_;
};

#endif