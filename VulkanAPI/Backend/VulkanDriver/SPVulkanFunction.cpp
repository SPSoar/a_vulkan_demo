#include "SPVulkanFunction.h"
namespace vkfunc 
{

	static void loadLoaderFunctions(void* context, PFN_vkVoidFunction(*loadcb)(void*, const char*));
	static void loadInstanceFunctions(void* context, PFN_vkVoidFunction(*loadcb)(void*, const char*));
	static void loadDeviceFunctions(void* context, PFN_vkVoidFunction(*loadcb)(void*, const char*));

	static PFN_vkVoidFunction vkGetInstanceProcAddrWrapper(void* context, const char* name);
	static PFN_vkVoidFunction vkGetDeviceProcAddrWrapper(void* context, const char* name);

	// OS Dependent. this is only windows func
	extern bool loadLibrary();
	extern void* getInstanceProcAddr();

	static const char* VKLIBRARY_PATH = "vulkan-1.dll";

	static HMODULE library = nullptr;

	bool loadLibrary() {
		library = LoadLibraryA(VKLIBRARY_PATH);
		return library != nullptr;
	}

	void* getInstanceProcAddr() {
		return GetProcAddress(library, "vkGetInstanceProcAddr");
	}

	bool initialize()
	{
		if (!loadLibrary()) {
			return false;
		}

		vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)getInstanceProcAddr();
		loadLoaderFunctions(nullptr, vkGetInstanceProcAddrWrapper);
		return true;
	}

	void bindInstance(VkInstance instance) {
		loadInstanceFunctions(instance, vkGetInstanceProcAddrWrapper);
		loadDeviceFunctions(instance, vkGetInstanceProcAddrWrapper);
	}

	void loadLoaderFunctions(void* context, PFN_vkVoidFunction(*loadcb)(void*, const char*))
	{
#if defined(VK_VERSION_1_0)
		vkCreateInstance = (PFN_vkCreateInstance)loadcb(context, "vkCreateInstance");
		vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)loadcb(context, "vkEnumerateInstanceExtensionProperties");
		vkEnumerateInstanceLayerProperties = (PFN_vkEnumerateInstanceLayerProperties)loadcb(context, "vkEnumerateInstanceLayerProperties");
#endif  
#if defined(VK_VERSION_1_1)
		vkEnumerateInstanceVersion = (PFN_vkEnumerateInstanceVersion)loadcb(context, "vkEnumerateInstanceVersion");
#endif 
	}

	void loadInstanceFunctions(void* context, PFN_vkVoidFunction(*loadcb)(void*, const char*))
	{
#ifdef VK_EXT_debug_utils
		vkCreateDebugUtilsMessengerEXT = (PFN_vkCreateDebugUtilsMessengerEXT)loadcb(context, "vkCreateDebugUtilsMessengerEXT");
#endif
#ifdef VK_EXT_debug_report
		vkCreateDebugReportCallbackEXT = (PFN_vkCreateDebugReportCallbackEXT)loadcb(context, "vkCreateDebugReportCallbackEXT");
#endif
#ifdef  VK_KHR_get_physical_device_properties2 
		vkGetPhysicalDeviceProperties2KHR = (PFN_vkGetPhysicalDeviceProperties2KHR)loadcb(context, "vkGetPhysicalDeviceProperties2KHR");
#endif
	}

	void loadDeviceFunctions(void* context, PFN_vkVoidFunction(*loadcb)(void*, const char*))
	{
#if defined(VK_VERSION_1_0)
		vkGetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)loadcb(context, "vkGetInstanceProcAddr");
#endif
	}

	PFN_vkVoidFunction vkGetInstanceProcAddrWrapper(void* context, const char* name)
	{
		return vkGetInstanceProcAddr((VkInstance)context, name);
	}

	PFN_vkVoidFunction vkGetDeviceProcAddrWrapper(void* context, const char* name)
	{
		return vkGetDeviceProcAddr((VkDevice)context, name);
	}
#if defined(VK_VERSION_1_0)
	PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;
	PFN_vkCreateInstance vkCreateInstance;
	PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
	PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties;
#endif
#if defined(VK_VERSION_1_1)
	PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion;
#endif 
#ifdef VK_EXT_debug_utils
	PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
#endif
#ifdef VK_EXT_debug_report
	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
#endif
#ifdef  VK_KHR_get_physical_device_properties2 
	PFN_vkGetPhysicalDeviceProperties2KHR vkGetPhysicalDeviceProperties2KHR;
#endif

}
