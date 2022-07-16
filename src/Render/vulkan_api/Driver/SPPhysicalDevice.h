#ifndef S_SPPHYSICALDEVICE_H_
#define S_SPPHYSICALDEVICE_H_

#include "../vulkan_common.h"
#include "SPVulkanInstance.h"

class SPPhysicalDevice
{
public:
	SPPhysicalDevice(SPVulkanInstance& instance, VkPhysicalDevice physical_device);
	~SPPhysicalDevice() = default;
	void init();

	SPVulkanInstance& get_instance() const;

public:
	const VkPhysicalDeviceProperties get_properties() const;
	void set_high_priority_graphics_queue_enable(bool enable);
	const VkPhysicalDeviceFeatures& get_features() const;
	VkPhysicalDeviceFeatures& get_mutable_requested_features();
	const std::vector<VkQueueFamilyProperties>& get_queue_family_properties() const;
	bool has_high_priority_graphics_queue() const;
	VkPhysicalDevice get_handle() const;
	void* get_extension_feature_chain() const;
	const VkPhysicalDeviceFeatures get_requested_features() const;
	VkBool32 is_present_supported(VkSurfaceKHR surface, uint32_t queue_family_index) const;
	const VkPhysicalDeviceMemoryProperties& get_memory_properties() const;
public:

	template <typename T>
	T& request_extension_features(VkStructureType type)
	{
		// We cannot request extension features if the physical device properties 2 instance extension isnt enabled
		if (!instance_.is_enabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
		{
			throw std::runtime_error("Couldn't request feature from device as " + std::string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) + " isn't enabled!");
		}

		// If the type already exists in the map, return a casted pointer to get the extension feature struct
		auto extension_features_it = extension_features_.find(type);
		if (extension_features_it != extension_features_.end())
		{
			return *static_cast<T*>(extension_features_it->second.get());
		}

		// Get the extension feature
		VkPhysicalDeviceFeatures2KHR physical_device_features{ VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2_KHR };
		T                            extension{ type };
		physical_device_features.pNext = &extension;
		vkGetPhysicalDeviceFeatures2KHR(handle_, &physical_device_features);

		// Insert the extension feature into the extension feature map so its ownership is held
		extension_features_.insert({ type, std::make_shared<T>(extension) });

		// Pull out the dereferenced void pointer, we can assume its type based on the template
		auto* extension_ptr = static_cast<T*>(extension_features_.find(type)->second.get());

		// If an extension feature has already been requested, we shift the linked list down by one
		// Making this current extension the new base pointer
		if (last_requested_extension_feature_)
		{
			extension_ptr->pNext = last_requested_extension_feature_;
		}
		last_requested_extension_feature_ = extension_ptr;

		return *extension_ptr;
	}

protected:

protected:
	SPVulkanInstance& instance_;

	VkPhysicalDevice handle_;
	VkPhysicalDeviceFeatures features_;
	VkPhysicalDeviceProperties properties_;
	VkPhysicalDeviceMemoryProperties memory_properties_;
	std::vector<VkQueueFamilyProperties> queue_family_properties_;
	bool high_priority_graphics_queue_;
	// The features that will be requested to be enabled in the logical device
	VkPhysicalDeviceFeatures requested_features_;
	// Holds the extension feature structures, we use a map to retain an order of requested structures
	std::map<VkStructureType, std::shared_ptr<void>> extension_features_;
	void* last_requested_extension_feature_{ nullptr };
};

#endif