#ifndef S_SPDESCRIPTORPOOL_H_
#define S_SPDESCRIPTORPOOL_H_

#include "../vulkan_common.h"

class SPDevice;
class SPDescriptorSetLayout;
class SPDescriptorPool
{
public:
	static const uint32_t MAX_SETS_PER_POOL_ = 16;

	SPDescriptorPool(SPDevice& device,
		const SPDescriptorSetLayout& descriptor_set_layout,
		uint32_t                   pool_size = MAX_SETS_PER_POOL_);
	SPDescriptorPool(const SPDescriptorPool&) = delete;
	SPDescriptorPool(SPDescriptorPool&&) = default;
	~SPDescriptorPool();
	SPDescriptorPool& operator=(const SPDescriptorPool&) = delete;
	SPDescriptorPool& operator=(SPDescriptorPool&&) = delete;

public:
	VkDescriptorSet allocate();
	const SPDescriptorSetLayout& get_descriptor_set_layout() const;
private:
	uint32_t find_available_pool(uint32_t pool_index);
private:
	SPDevice& device_;
	const SPDescriptorSetLayout* descriptor_set_layout_;
	std::vector<VkDescriptorPoolSize> pool_sizes_;
	uint32_t pool_max_sets_;
	std::vector<VkDescriptorPool> pools_;
	uint32_t pool_index_;
	std::vector<uint32_t> pool_sets_count_;
	std::unordered_map<VkDescriptorSet, uint32_t> set_pool_mapping_;
};

#endif