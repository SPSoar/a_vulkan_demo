#include "SPShaderSource.h"
#include "../Driver/SPDevice.h"
#include "../vk_tool/glsl_compiler.h"
#include "../vk_tool/spirv_reflection.h"

inline std::vector<std::string> precompile_shader(const std::string& source)
{
	std::vector<std::string> final_file;

	auto lines = vkTool::split(source, '\n');

	for (auto& line : lines)
	{
		if (line.find("#include \"") == 0)
		{
			// Include paths are relative to the base shader directory
			std::string include_path = line.substr(10);
			size_t      last_quote = include_path.find("\"");
			if (!include_path.empty() && last_quote != std::string::npos)
			{
				include_path = include_path.substr(0, last_quote);
			}
			//ÐèÒªÐÞÕý ???
			auto include_file = precompile_shader(vkTool::read_text_file(include_path));
			for (auto& include_file_line : include_file)
			{
				final_file.push_back(include_file_line);
			}
		}
		else
		{
			final_file.push_back(line);
		}
	}

	return final_file;
}


SPShaderSource::SPShaderSource(const std::string& filename)
	: filename_{ filename }
	, source_{ vkTool::read_text_file(filename) }
{
	std::hash<std::string> hasher{};
	id_ = hasher(std::string{ this->source_.cbegin(), this->source_.cend() });
}

uint64_t SPShaderSource::get_id() const
{
	return id_;
}

const std::string& SPShaderSource::get_filename() const
{
	return filename_;
}

void SPShaderSource::set_source(const std::string& source)
{
	source_ = source;
	std::hash<std::string> hasher{};
	id_ = hasher(std::string{ this->source_.cbegin(), this->source_.cend() });
}

const std::string& SPShaderSource::get_source() const
{
	return source_;
}




inline std::vector<uint8_t> convert_to_bytes(std::vector<std::string>& lines)
{
	std::vector<uint8_t> bytes;

	for (auto& line : lines)
	{
		line += "\n";
		std::vector<uint8_t> line_bytes(line.begin(), line.end());
		bytes.insert(bytes.end(), line_bytes.begin(), line_bytes.end());
	}

	return bytes;
}

SPShaderModule::SPShaderModule(SPDevice& device, VkShaderStageFlagBits stage, const SPShaderSource& glsl_source, const std::string& entry_point, const SPShaderVariant& shader_variant)
	: device_{ device }
	, stage_{ stage }
	, entry_point_{ entry_point }
	, id_{}
	, spirv_{}
	, resources_{}
	, info_log_{}
{
	// Compiling from GLSL source requires the entry point
	if (entry_point.empty())
	{
		throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
	}

	auto& source = glsl_source.get_source();

	// Check if application is passing in GLSL source code to compile to SPIR-V
	if (source.empty())
	{
		throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
	}

	// Precompile source into the final spirv bytecode
	auto glsl_final_source = precompile_shader(source);

	// Compile the GLSL source
	vkb::GLSLCompiler glsl_compiler;

	if (!glsl_compiler.compile_to_spirv(stage, convert_to_bytes(glsl_final_source), entry_point, shader_variant, spirv_, info_log_))
	{
		//LOGE("Shader compilation failed for shader \"{}\"", glsl_source.get_filename());
		//LOGE("{}", info_log);
		throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
	}

	vkb::SPIRVReflection spirv_reflection;

	// Reflect all shader resouces
	if (!spirv_reflection.reflect_shader_resources(stage, spirv_, resources_, shader_variant))
	{
		throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED };
	}

	// Generate a unique id, determined by source and variant
	std::hash<std::string> hasher{};
	id_ = hasher(std::string{ reinterpret_cast<const char*>(spirv_.data()),
							reinterpret_cast<const char*>(spirv_.data() + spirv_.size()) });
}

SPShaderModule::SPShaderModule(SPShaderModule&& other) noexcept
	: device_{ other.device_ }
	, id_{ other.id_ }
	, stage_{ other.stage_ }
	, entry_point_{ other.entry_point_ }
	, spirv_{ other.spirv_ }
	, resources_{ other.resources_ }
	, info_log_{ other.info_log_ }
{
	other.stage_ = {};
}

uint64_t SPShaderModule::get_id() const
{
	return id_;
}

SPShaderVariant::SPShaderVariant(std::string&& preamble, std::vector<std::string>&& processes)
	: preamble_{ std::move(preamble) }
	, processes_{ std::move(processes) }
	
{
	update_id();
}

const std::string& SPShaderVariant::get_preamble() const
{
	return preamble_;
}

const std::vector<std::string>& SPShaderVariant::get_processes() const
{
	return processes_;
}

void SPShaderVariant::add_runtime_array_size(const std::string& runtime_array_name, uint64_t size)
{
	if (runtime_array_sizes_.find(runtime_array_name) == runtime_array_sizes_.end())
	{
		runtime_array_sizes_.insert({ runtime_array_name, size });
	}
	else
	{
		runtime_array_sizes_[runtime_array_name] = size;
	}
}

const std::unordered_map<std::string, uint64_t>& SPShaderVariant::get_runtime_array_sizes() const
{
	return runtime_array_sizes_;
}

size_t SPShaderVariant::get_id() const
{
	return id_;
}

void SPShaderVariant::clear()
{
	preamble_.clear();
	processes_.clear();
	runtime_array_sizes_.clear();
	update_id();
}

void SPShaderVariant::add_define(const std::string& def)
{
	processes_.push_back("D" + def);

	std::string tmp_def = def;

	// The "=" needs to turn into a space
	size_t pos_equal = tmp_def.find_first_of("=");
	if (pos_equal != std::string::npos)
	{
		tmp_def[pos_equal] = ' ';
	}

	preamble_.append("#define " + tmp_def + "\n");

	update_id();
}

void SPShaderVariant::update_id()
{
	std::hash<std::string> hasher{};
	id_ = hasher(preamble_);
}
