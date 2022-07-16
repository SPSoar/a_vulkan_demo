#include "SPSSampler.h"


SPSSampler::SPSSampler(const std::string& name, SPSampler&& vk_sampler)
	: SPComponent{ name }
	, vk_sampler_{ std::move(vk_sampler) }
{

}

std::type_index SPSSampler::get_type()
{
	return typeid(SPSSampler);
}
