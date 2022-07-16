#include "SPDescriptorPool.h"
#include "../Driver/SPDevice.h"
#include "SPDescriptorSetLayout.h"

SPDescriptorPool::SPDescriptorPool(SPDevice& device, const SPDescriptorSetLayout& descriptor_set_layout, uint32_t pool_size)
	: device_{ device }
	, descriptor_set_layout_{ &descriptor_set_layout }
	, pool_index_{0}
{
	const auto& bindings = descriptor_set_layout.get_bindings();
	std::map<VkDescriptorType, std::uint32_t> descriptor_type_counts;

	// Count each type of descriptor set
	for (auto& binding : bindings)
	{
		descriptor_type_counts[binding.descriptorType] += binding.descriptorCount;
	}

	// Allocate pool sizes array
	pool_sizes_.resize(descriptor_type_counts.size());

	auto pool_size_it = pool_sizes_.begin();

	// Fill pool size for each descriptor type count multiplied by the pool size
	for (auto& it : descriptor_type_counts)
	{
		pool_size_it->type = it.first;

		pool_size_it->descriptorCount = it.second * pool_size;

		++pool_size_it;
	}

	pool_max_sets_ = pool_size;
}

SPDescriptorPool::~SPDescriptorPool()
{
	// Destroy all descriptor pools
	for (auto pool : pools_)
	{
		vkDestroyDescriptorPool(device_.get_handle(), pool, nullptr);
	}
}

VkDescriptorSet SPDescriptorPool::allocate()
{

	pool_index_ = find_available_pool(pool_index_);

	// Increment allocated set count for the current pool
	++pool_sets_count_[pool_index_];

	VkDescriptorSetLayout set_layout = get_descriptor_set_layout().get_handle();

	VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
	alloc_info.descriptorPool = pools_[pool_index_];
	alloc_info.descriptorSetCount = 1;
	alloc_info.pSetLayouts = &set_layout;

	VkDescriptorSet handle = VK_NULL_HANDLE;

	// Allocate a new descriptor set from the current pool
	auto result = vkAllocateDescriptorSets(device_.get_handle(), &alloc_info, &handle);

	if (result != VK_SUCCESS)
	{
		// Decrement allocated set count for the current pool
		--pool_sets_count_[pool_index_];

		return VK_NULL_HANDLE;
	}

	// Store mapping between the descriptor set and the pool
	set_pool_mapping_.emplace(handle, pool_index_);

	return handle;
}

const SPDescriptorSetLayout& SPDescriptorPool::get_descriptor_set_layout() const
{
	assert(descriptor_set_layout_ && "Descriptor set layout is invalid");
	return *descriptor_set_layout_;
}

uint32_t SPDescriptorPool::find_available_pool(uint32_t search_index)
{
	// Create a new pool
	if (pools_.size() <= search_index)
	{
		VkDescriptorPoolCreateInfo create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO };

		create_info.poolSizeCount = sp::to_u32(pool_sizes_.size());
		create_info.pPoolSizes = pool_sizes_.data();
		create_info.maxSets = pool_max_sets_;

		// We do not set FREE_DESCRIPTOR_SET_BIT as we do not need to free individual descriptor sets
		create_info.flags = 0;

		// Check descriptor set layout and enable the required flags
		auto& binding_flags = descriptor_set_layout_->get_binding_flags();
		for (auto binding_flag : binding_flags)
		{
			if (binding_flag & VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT)
			{
				create_info.flags |= VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT_EXT;
			}
		}

		VkDescriptorPool handle = VK_NULL_HANDLE;

		// Create the Vulkan descriptor pool
		auto result = vkCreateDescriptorPool(device_.get_handle(), &create_info, nullptr, &handle);

		if (result != VK_SUCCESS)
		{
			return 0;
		}

		// Store internally the Vulkan handle
		pools_.push_back(handle);

		// Add set count for the descriptor pool
		pool_sets_count_.push_back(0);

		return search_index;
	}
	else if (pool_sets_count_[search_index] < pool_max_sets_)
	{
		return search_index;
	}

	// Increment pool index
	return find_available_pool(++search_index);
}
