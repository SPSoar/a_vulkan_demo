#ifndef S_SPDESCRIPTORSET_H_
#define S_SPDESCRIPTORSET_H_

#include "../vulkan_common.h"

class SPDevice;
class SPDescriptorPool;
class SPDescriptorSetLayout;
class SPDescriptorSet
{
public:
	SPDescriptorSet(SPDevice& device,
		SPDescriptorSetLayout& descriptor_set_layout,
		SPDescriptorPool& descriptor_pool,
		const SPBindingMap<VkDescriptorBufferInfo>& buffer_infos = {},
		const SPBindingMap<VkDescriptorImageInfo>& image_infos = {});
	SPDescriptorSet(const SPDescriptorSet&) = delete;
	SPDescriptorSet(SPDescriptorSet&& other) noexcept;
	~SPDescriptorSet() = default;
	SPDescriptorSet& operator=(const SPDescriptorSet&) = delete;
	SPDescriptorSet& operator=(SPDescriptorSet&&) = delete;
public:
	void prepare();
private:
	SPDevice& device_;
	SPDescriptorSetLayout& descriptor_set_layout_;
	SPDescriptorPool& descriptor_pool_;
	SPBindingMap<VkDescriptorBufferInfo> buffer_infos_;
	SPBindingMap<VkDescriptorImageInfo> image_infos_;
	VkDescriptorSet handle_;
	std::vector<VkWriteDescriptorSet> write_descriptor_sets_;
	std::unordered_map<uint32_t, size_t> updated_bindings_;
};

#endif