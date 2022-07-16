#include "SPResourceCache.h"
#include "../vulkan_resource_caching.h"


template <class T, class... A>
T& request_resource(SPDevice& device, SPResourceRecord& recorder, std::mutex& resource_mutex, std::unordered_map<uint64_t, T>& resources, A &... args)
{
	std::lock_guard<std::mutex> guard(resource_mutex);

	auto& res = request_resource(device, &recorder, resources, args...);

	return res;
}

SPResourceCache::SPResourceCache(SPDevice& device)
	: device_{ device }
	, recorder_{}
	, shader_module_mutex_{}
	, state_{}
{

}

SPShaderModule& SPResourceCache::request_shader_module(VkShaderStageFlagBits stage, const SPShaderSource& glsl_source, const SPShaderVariant& shader_variant)
{
	std::string entry_point{ "main" };
	return request_resource(device_, recorder_, shader_module_mutex_, state_.shader_modules_, stage, glsl_source, entry_point, shader_variant);

}

SPPipelineLayout& SPResourceCache::request_pipeline_layout(const std::vector<SPShaderModule*>& shader_modules)
{
	SPPipelineLayout aa;
	return  aa;
		//request_resource(device_, recorder_, pipeline_layout_mutex_, state_.pipeline_layouts_, shader_modules);
}

void SPResourceCache::clear_framebuffers()
{
	state_.framebuffers_.clear();
}
