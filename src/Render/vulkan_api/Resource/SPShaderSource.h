#ifndef S_SPSHADERSOURCE_H_
#define S_SPSHADERSOURCE_H_

#include "../vulkan_common.h"


/// Types of shader resources
enum class SPShaderResourceType
{
	Input,
	InputAttachment,
	Output,
	Image,
	ImageSampler,
	ImageStorage,
	Sampler,
	BufferUniform,
	BufferStorage,
	PushConstant,
	SpecializationConstant,
	All
};

/// This determines the type and method of how descriptor set should be created and bound
enum class SPShaderResourceMode
{
	Static,
	Dynamic,
	UpdateAfterBind
};

struct SPShaderResourceQualifiers
{
	enum : uint32_t
	{
		None = 0,
		NonReadable = 1,
		NonWritable = 2,
	};
};

struct SPShaderResource
{
	VkShaderStageFlags stages_;
	SPShaderResourceType type_;
	SPShaderResourceMode mode_;
	uint32_t set_;
	uint32_t binding_;
	uint32_t location_;
	uint32_t input_attachment_index_;
	uint32_t vec_size_;
	uint32_t columns_;
	uint32_t array_size_;
	uint32_t offset_;
	uint32_t size_;
	uint32_t constant_id_;
	uint32_t qualifiers_;
	std::string name_;
};


class SPShaderVariant
{
public:
	SPShaderVariant() = default;
	SPShaderVariant(std::string&& preamble, std::vector<std::string>&& processes);
public:
	const std::string& get_preamble() const;
	const std::vector<std::string>& get_processes() const;
	void add_runtime_array_size(const std::string& runtime_array_name, uint64_t size);
	const std::unordered_map<std::string, uint64_t>& get_runtime_array_sizes() const;
	size_t get_id() const;
	void clear();
public:
	void add_define(const std::string& def);
private:
	void update_id();
protected:
	std::string preamble_;
	std::vector<std::string> processes_;
	std::unordered_map<std::string, uint64_t> runtime_array_sizes_;
	uint64_t id_;
};

class SPShaderSource
{
public:
	SPShaderSource() = default;
	SPShaderSource(const std::string & filename);

	uint64_t get_id() const;
	const std::string& get_filename() const;
	void set_source(const std::string & source);
	const std::string& get_source() const;

private:
	uint64_t id_;
	std::string filename_;
	std::string source_;
};

class SPDevice;
class SPShaderModule
{
public:
	SPShaderModule(SPDevice& device,
		VkShaderStageFlagBits stage,
		const SPShaderSource& glsl_source,
		const std::string& entry_point,
		const SPShaderVariant& shader_variant);

	SPShaderModule(const SPShaderModule&) = delete;
	SPShaderModule(SPShaderModule&& other) noexcept;
	SPShaderModule& operator=(const SPShaderModule&) = delete;
	SPShaderModule& operator=(SPShaderModule&&) = delete;
	uint64_t get_id() const;
private:
	SPDevice& device_;
	VkShaderStageFlagBits stage_;
	/// Name of the main function
	std::string entry_point_;

	std::vector<uint32_t> spirv_;
	std::string info_log_;
	std::vector<SPShaderResource> resources_;
	uint64_t id_;
};

#endif