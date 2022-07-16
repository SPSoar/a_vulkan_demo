#ifndef S_SPDESCRIPTORSETLAYOUT_H_
#define S_SPDESCRIPTORSETLAYOUT_H_

#include "../vulkan_common.h"

class SPDevice;
class SPShaderModule;
struct SPShaderResource;
class SPDescriptorSetLayout
{
public:
	SPDescriptorSetLayout(SPDevice& device,
		const uint32_t                     set_index,
		const std::vector<SPShaderModule*>& shader_modules,
		const std::vector<SPShaderResource>& resource_set);
	SPDescriptorSetLayout(const SPDescriptorSetLayout&) = delete;
	SPDescriptorSetLayout(SPDescriptorSetLayout&& other);
	~SPDescriptorSetLayout();
	SPDescriptorSetLayout& operator=(const SPDescriptorSetLayout&) = delete;
	SPDescriptorSetLayout& operator=(SPDescriptorSetLayout&&) = delete;
	VkDescriptorSetLayout get_handle() const;
public:
	const std::vector<VkDescriptorSetLayoutBinding>& get_bindings() const;
	const std::vector<VkDescriptorBindingFlagsEXT>& get_binding_flags() const;
	std::unique_ptr<VkDescriptorSetLayoutBinding> get_layout_binding(const uint32_t binding_index) const;

private:
	SPDevice& device_;
	VkDescriptorSetLayout handle_;
	const uint32_t set_index_;
	std::vector<SPShaderModule*> shader_modules_;
	std::vector<VkDescriptorBindingFlagsEXT> binding_flags_;
	std::vector<VkDescriptorSetLayoutBinding> bindings_;
	std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings_lookup_;
	std::unordered_map<uint32_t, VkDescriptorBindingFlagsEXT> binding_flags_lookup_;
	std::unordered_map<std::string, uint32_t> resources_lookup_;

};

#endif