#include "SPDescriptorSetLayout.h"

#include "../Resource/SPShaderSource.h"
#include "../Driver/SPDevice.h"


inline VkDescriptorType find_descriptor_type(SPShaderResourceType resource_type, bool dynamic)
{
	switch (resource_type)
	{
	case SPShaderResourceType::InputAttachment:
		return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		break;
	case SPShaderResourceType::Image:
		return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		break;
	case SPShaderResourceType::ImageSampler:
		return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		break;
	case SPShaderResourceType::ImageStorage:
		return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		break;
	case SPShaderResourceType::Sampler:
		return VK_DESCRIPTOR_TYPE_SAMPLER;
		break;
	case SPShaderResourceType::BufferUniform:
		if (dynamic)
		{
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		}
		else
		{
			return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}
		break;
	case SPShaderResourceType::BufferStorage:
		if (dynamic)
		{
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		}
		else
		{
			return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		}
		break;
	default:
		throw std::runtime_error("No conversion possible for the shader resource type.");
		break;
	}
}


inline bool validate_flags(const SPPhysicalDevice& gpu, const std::vector<VkDescriptorSetLayoutBinding>& bindings, const std::vector<VkDescriptorBindingFlagsEXT>& flags)
{
	// Assume bindings are valid if there are no flags
	if (flags.empty())
	{
		return true;
	}

	// Binding count has to equal flag count as its a 1:1 mapping
	if (bindings.size() != flags.size())
	{
		//LOGE("Binding count has to be equal to flag count.");
		return false;
	}

	return true;
}

SPDescriptorSetLayout::SPDescriptorSetLayout(SPDevice& device, const uint32_t set_index, const std::vector<SPShaderModule*>& shader_modules, const std::vector<SPShaderResource>& resource_set)
	: device_{ device }
	, set_index_{ set_index }
	, shader_modules_{ shader_modules }
{
	for (auto& resource : resource_set)
	{

		if (resource.type_ == SPShaderResourceType::Input ||
			resource.type_ == SPShaderResourceType::Output ||
			resource.type_ == SPShaderResourceType::PushConstant ||
			resource.type_ == SPShaderResourceType::SpecializationConstant)
		{
			continue;
		}

		// Convert from ShaderResourceType to VkDescriptorType.
		auto descriptor_type = find_descriptor_type(resource.type_, resource.mode_ == SPShaderResourceMode::Dynamic);

		if (resource.mode_ == SPShaderResourceMode::UpdateAfterBind)
		{
			binding_flags_.push_back(VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT);
		}
		else
		{
			// When creating a descriptor set layout, if we give a structure to create_info.pNext, each binding needs to have a binding flag
			// (pBindings[i] uses the flags in pBindingFlags[i])
			// Adding 0 ensures the bindings that dont use any flags are mapped correctly.
			binding_flags_.push_back(0);
		}

		// Convert ShaderResource to VkDescriptorSetLayoutBinding
		VkDescriptorSetLayoutBinding layout_binding{};

		layout_binding.binding = resource.binding_;
		layout_binding.descriptorCount = resource.array_size_;
		layout_binding.descriptorType = descriptor_type;
		layout_binding.stageFlags = static_cast<VkShaderStageFlags>(resource.stages_);

		bindings_.push_back(layout_binding);

		// Store mapping between binding and the binding point
		bindings_lookup_.emplace(resource.binding_, layout_binding);

		binding_flags_lookup_.emplace(resource.binding_, binding_flags_.back());

		resources_lookup_.emplace(resource.name_, resource.binding_);
	}

	VkDescriptorSetLayoutCreateInfo create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
	create_info.flags = 0;
	create_info.bindingCount = sp::to_u32(bindings_.size());
	create_info.pBindings = bindings_.data();

	// Handle update-after-bind extensions
	if (std::find_if(resource_set.begin(), resource_set.end(),
		[](const SPShaderResource& shader_resource) { return shader_resource.mode_ == SPShaderResourceMode::UpdateAfterBind; }) != resource_set.end())
	{
		// Spec states you can't have ANY dynamic resources if you have one of the bindings set to update-after-bind
		if (std::find_if(resource_set.begin(), resource_set.end(),
			[](const SPShaderResource& shader_resource) { return shader_resource.mode_ == SPShaderResourceMode::Dynamic; }) != resource_set.end())
		{
			throw std::runtime_error("Cannot create descriptor set layout, dynamic resources are not allowed if at least one resource is update-after-bind.");
		}

		if (!validate_flags(device.get_gpu(), bindings_, binding_flags_))
		{
			throw std::runtime_error("Invalid binding, couldn't create descriptor set layout.");
		}

		VkDescriptorSetLayoutBindingFlagsCreateInfoEXT binding_flags_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO_EXT };
		binding_flags_create_info.bindingCount = sp::to_u32(binding_flags_.size());
		binding_flags_create_info.pBindingFlags = binding_flags_.data();

		create_info.pNext = &binding_flags_create_info;
		create_info.flags |= std::find(binding_flags_.begin(), binding_flags_.end(), VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT_EXT) != binding_flags_.end() ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT : 0;
	}

	// Create the Vulkan descriptor set layout handle
	VkResult result = vkCreateDescriptorSetLayout(device.get_handle(), &create_info, nullptr, &handle_);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create DescriptorSetLayout" };
	}
}

SPDescriptorSetLayout::SPDescriptorSetLayout(SPDescriptorSetLayout&& other)
	: device_{ other.device_ }
	, shader_modules_{ other.shader_modules_ }
	, handle_{ other.handle_ }
	, set_index_{ other.set_index_ }
	, bindings_{ std::move(other.bindings_) }
	, binding_flags_{ std::move(other.binding_flags_) }
	, bindings_lookup_{ std::move(other.bindings_lookup_) }
	, binding_flags_lookup_{ std::move(other.binding_flags_lookup_) }
	, resources_lookup_{ std::move(other.resources_lookup_) }
{
	other.handle_ = VK_NULL_HANDLE;
}

SPDescriptorSetLayout::~SPDescriptorSetLayout()
{
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyDescriptorSetLayout(device_.get_handle(), handle_, nullptr);
	}
}

VkDescriptorSetLayout SPDescriptorSetLayout::get_handle() const
{
	return handle_;
}

const std::vector<VkDescriptorSetLayoutBinding>& SPDescriptorSetLayout::get_bindings() const
{
	return bindings_;
}

const std::vector<VkDescriptorBindingFlagsEXT>& SPDescriptorSetLayout::get_binding_flags() const
{
	return binding_flags_;
}

std::unique_ptr<VkDescriptorSetLayoutBinding> SPDescriptorSetLayout::get_layout_binding(const uint32_t binding_index) const
{
	auto it = bindings_lookup_.find(binding_index);

	if (it == bindings_lookup_.end())
	{
		return nullptr;
	}

	return std::make_unique<VkDescriptorSetLayoutBinding>(it->second);
}
