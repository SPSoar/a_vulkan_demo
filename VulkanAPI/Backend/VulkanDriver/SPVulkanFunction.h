#ifndef S_SPVULKANFUNCTION_H_
#define S_SPVULKANFUNCTION_H_
#include "vulkan_common.h"
namespace vkfunc 
{

	bool initialize();

	void bindInstance(VkInstance instance);
#if defined(VK_VERSION_1_0)
	extern PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
	extern PFN_vkCreateInstance vkCreateInstance;
	extern PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
	extern PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
#endif
#if defined(VK_VERSION_1_1)
	extern PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion; 
#endif 
#if defined(VK_EXT_debug_utils)
	extern PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
#endif

#if defined(VK_EXT_debug_report)
	extern PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
#endif

#if defined(VK_KHR_get_physical_device_properties2)
	extern PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
#endif 
}


#endif