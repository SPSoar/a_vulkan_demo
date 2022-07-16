#include "SPResourceRecord.h"

inline void write_processes(std::ostringstream& os, const std::vector<std::string>& value)
{
	sp::write(os, value.size());
	for (const std::string& item : value)
	{
		sp::write(os, item);
	}
}

uint64_t SPResourceRecord::register_shader_module(VkShaderStageFlagBits stage, const SPShaderSource& glsl_source, const std::string& entry_point, const SPShaderVariant& shader_variant)
{
	shader_module_indices_.push_back(shader_module_indices_.size());

	sp::write(stream_, SPResourceType::ShaderModule, stage, glsl_source.get_source(), entry_point, shader_variant.get_preamble());

	write_processes(stream_, shader_variant.get_processes());

	return shader_module_indices_.back();
}

void SPResourceRecord::set_shader_module(uint64_t index, const SPShaderModule& shader_module)
{
    shader_module_to_index_[&shader_module] = index;
}
