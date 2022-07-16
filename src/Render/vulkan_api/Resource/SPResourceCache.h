#ifndef S_SPRESOURCECACHE_H_
#define S_SPRESOURCECACHE_H_

#include "../vulkan_common.h"
#include "SPShaderSource.h"
#include "SPResourceRecord.h"
#include "../Pipeline/SPPipelineLayout.h"
#include "../RenderContext/SPFramebuffer.h"

struct SPResourceCacheState
{
	std::unordered_map<uint64_t, SPShaderModule> shader_modules_;
	std::unordered_map<uint64_t, SPPipelineLayout> pipeline_layouts_;

	std::unordered_map<uint64_t, SPFramebuffer> framebuffers_;
};

class SPDevice;
class SPResourceCache
{
public:
	SPResourceCache(SPDevice& device);
	SPResourceCache(const SPResourceCache&) = delete;
	SPResourceCache(SPResourceCache&&) = delete;
	SPResourceCache& operator=(const SPResourceCache&) = delete;
	SPResourceCache& operator=(SPResourceCache&&) = delete;
public:
	SPShaderModule& request_shader_module(VkShaderStageFlagBits stage, const SPShaderSource& glsl_source, const SPShaderVariant& shader_variant = {});
	SPPipelineLayout& request_pipeline_layout(const std::vector<SPShaderModule*>& shader_modules);
	void clear_framebuffers();
protected:
	SPDevice& device_;
	SPResourceRecord recorder_;
	std::mutex shader_module_mutex_;
	SPResourceCacheState state_;
	std::mutex pipeline_layout_mutex_;

};

#endif