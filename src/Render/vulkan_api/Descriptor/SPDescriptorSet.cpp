#include "SPDescriptorSet.h"
#include "../Driver/SPDevice.h"
#include "SPDescriptorPool.h"
#include "SPDescriptorSetLayout.h"

SPDescriptorSet::SPDescriptorSet(SPDevice& device, SPDescriptorSetLayout& descriptor_set_layout, SPDescriptorPool& descriptor_pool, const SPBindingMap<VkDescriptorBufferInfo>& buffer_infos, const SPBindingMap<VkDescriptorImageInfo>& image_infos)
	: device_{ device }
    , descriptor_set_layout_{ descriptor_set_layout }
    , descriptor_pool_{ descriptor_pool }
    , buffer_infos_{ buffer_infos }
    , image_infos_{ image_infos }
    , handle_{ descriptor_pool.allocate() }
    , write_descriptor_sets_{}
    , updated_bindings_{}
{
    prepare();
}

SPDescriptorSet::SPDescriptorSet(SPDescriptorSet&& other) noexcept
    : device_{ other.device_ }
    , descriptor_set_layout_{ other.descriptor_set_layout_ }
    , descriptor_pool_{ other.descriptor_pool_ }
    , buffer_infos_{ std::move(other.buffer_infos_) }
    , image_infos_{ std::move(other.image_infos_) }
    , handle_{ other.handle_ }
    , write_descriptor_sets_{ std::move(other.write_descriptor_sets_) }
    , updated_bindings_{ std::move(other.updated_bindings_) }
{
    other.handle_ = VK_NULL_HANDLE;
}

void SPDescriptorSet::prepare()
{
	// We don't want to prepare twice during the life cycle of a Descriptor Set
	if (!write_descriptor_sets_.empty())
	{
		//LOGW("Trying to prepare a descriptor set that has already been prepared, skipping.");
		return;
	}

	// Iterate over all buffer bindings
	for (auto& binding_it : buffer_infos_)
	{
		auto  binding_index = binding_it.first;
		auto& buffer_bindings = binding_it.second;

		if (auto binding_info = descriptor_set_layout_.get_layout_binding(binding_index))
		{
			// Iterate over all binding buffers in array
			for (auto& element_it : buffer_bindings)
			{
				auto& buffer_info = element_it.second;

				size_t uniform_buffer_range_limit = device_.get_gpu().get_properties().limits.maxUniformBufferRange;
				size_t storage_buffer_range_limit = device_.get_gpu().get_properties().limits.maxStorageBufferRange;

				size_t buffer_range_limit = static_cast<size_t>(buffer_info.range);

				if ((binding_info->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || binding_info->descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC) && buffer_range_limit > uniform_buffer_range_limit)
				{
					//LOGE("Set {} binding {} cannot be updated: buffer size {} exceeds the uniform buffer range limit {}", descriptor_set_layout.get_index(), binding_index, buffer_info.range, uniform_buffer_range_limit);
					buffer_range_limit = uniform_buffer_range_limit;
				}
				else if ((binding_info->descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER || binding_info->descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC) && buffer_range_limit > storage_buffer_range_limit)
				{
					//LOGE("Set {} binding {} cannot be updated: buffer size {} exceeds the storage buffer range limit {}", descriptor_set_layout.get_index(), binding_index, buffer_info.range, storage_buffer_range_limit);
					buffer_range_limit = storage_buffer_range_limit;
				}

				// Clip the buffers range to the limit if one exists as otherwise we will receive a Vulkan validation error
				buffer_info.range = buffer_range_limit;

				VkWriteDescriptorSet write_descriptor_set{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };

				write_descriptor_set.dstBinding = binding_index;
				write_descriptor_set.descriptorType = binding_info->descriptorType;
				write_descriptor_set.pBufferInfo = &buffer_info;
				write_descriptor_set.dstSet = handle_;
				write_descriptor_set.dstArrayElement = element_it.first;
				write_descriptor_set.descriptorCount = 1;

				write_descriptor_sets_.push_back(write_descriptor_set);
			}
		}
		else
		{
			//LOGE("Shader layout set does not use buffer binding at #{}", binding_index);
		}
	}

	// Iterate over all image bindings
	for (auto& binding_it : image_infos_)
	{
		auto  binding_index = binding_it.first;
		auto& binding_resources = binding_it.second;

		if (auto binding_info = descriptor_set_layout_.get_layout_binding(binding_index))
		{
			// Iterate over all binding images in array
			for (auto& element_it : binding_resources)
			{
				auto& image_info = element_it.second;

				VkWriteDescriptorSet write_descriptor_set{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };

				write_descriptor_set.dstBinding = binding_index;
				write_descriptor_set.descriptorType = binding_info->descriptorType;
				write_descriptor_set.pImageInfo = &image_info;
				write_descriptor_set.dstSet = handle_;
				write_descriptor_set.dstArrayElement = element_it.first;
				write_descriptor_set.descriptorCount = 1;

				write_descriptor_sets_.push_back(write_descriptor_set);
			}
		}
		else
		{
			//LOGE("Shader layout set does not use image binding at #{}", binding_index);
		}
	}
}
