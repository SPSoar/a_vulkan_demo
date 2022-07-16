#include "SPPhysicalDevice.h"
#include "SPVulkanInstance.h"
SPPhysicalDevice::SPPhysicalDevice(SPVulkanInstance& instance, VkPhysicalDevice physical_device)
	: instance_(instance)
	, handle_(physical_device)
{
	init();
}

void SPPhysicalDevice::init()
{
	vkGetPhysicalDeviceFeatures(handle_, &features_);
	vkGetPhysicalDeviceProperties(handle_, &properties_);
	vkGetPhysicalDeviceMemoryProperties(handle_, &memory_properties_);

	SPLog("Found GPU: "<< properties_.deviceName);

	uint32_t queue_family_properties_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(handle_, &queue_family_properties_count, nullptr);
	queue_family_properties_ = std::vector<VkQueueFamilyProperties>(queue_family_properties_count);
	vkGetPhysicalDeviceQueueFamilyProperties(handle_, &queue_family_properties_count, queue_family_properties_.data());
}

SPVulkanInstance& SPPhysicalDevice::get_instance() const
{
	return instance_;
}

const VkPhysicalDeviceProperties SPPhysicalDevice::get_properties() const
{
	return properties_;
}

void SPPhysicalDevice::set_high_priority_graphics_queue_enable(bool enable)
{
	high_priority_graphics_queue_ = enable;
}

const VkPhysicalDeviceFeatures& SPPhysicalDevice::get_features() const
{
	return features_;
}

VkPhysicalDeviceFeatures& SPPhysicalDevice::get_mutable_requested_features()
{
	return requested_features_;
}

const std::vector<VkQueueFamilyProperties>& SPPhysicalDevice::get_queue_family_properties() const
{
	return queue_family_properties_;
}

bool SPPhysicalDevice::has_high_priority_graphics_queue() const
{
	return high_priority_graphics_queue_;
}

VkPhysicalDevice SPPhysicalDevice::get_handle() const
{
	return handle_;
}

void* SPPhysicalDevice::get_extension_feature_chain() const
{

	return last_requested_extension_feature_;
}

const VkPhysicalDeviceFeatures SPPhysicalDevice::get_requested_features() const
{
	return requested_features_;
}

VkBool32 SPPhysicalDevice::is_present_supported(VkSurfaceKHR surface, uint32_t queue_family_index) const
{
	VkBool32 present_supported{ VK_FALSE };

	if (surface != VK_NULL_HANDLE)
	{
		VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(handle_, queue_family_index, surface, &present_supported));
	}

	return present_supported;
}

const VkPhysicalDeviceMemoryProperties& SPPhysicalDevice::get_memory_properties() const
{
	return memory_properties_;
}
