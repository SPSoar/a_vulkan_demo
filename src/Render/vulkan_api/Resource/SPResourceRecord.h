#ifndef S_SPRESOURCERECORD_H_
#define S_SPRESOURCERECORD_H_

#include "../vulkan_common.h"
#include "SPShaderSource.h"


enum class SPResourceType
{
	ShaderModule,
	PipelineLayout,
	RenderPass,
	GraphicsPipeline
};

class SPDevice;
class SPResourceRecord
{
public:
	uint64_t register_shader_module(VkShaderStageFlagBits stage,
		const SPShaderSource& glsl_source,
		const std::string& entry_point,
		const SPShaderVariant& shader_variant);

	void set_shader_module(uint64_t index, const SPShaderModule& shader_module);
private:
	std::vector<uint64_t> shader_module_indices_;
	std::unordered_map<const SPShaderModule*, uint64_t> shader_module_to_index_;
	std::ostringstream stream_;
};

#endif