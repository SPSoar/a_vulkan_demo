#include "SPVulkanInstance.h"
#include "SPInstanceInfoManager.h"
#include "SPPhysicalDevice.h"

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)

VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
	void* user_data)
{
	// Log debug messge
	if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		SPWarning(callback_data->messageIdNumber << "-" << callback_data->pMessageIdName << ":" << callback_data->pMessage);
	}
	else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		SPError(callback_data->messageIdNumber << "-" << callback_data->pMessageIdName << ":" << callback_data->pMessage);
	}
	return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT /*type*/,
	uint64_t /*object*/, size_t /*location*/, int32_t /*message_code*/,
	const char* layer_prefix, const char* message, void* /*user_data*/)
{
	if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
	{
		SPError(layer_prefix << ":" << message);
	}
	else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
	{
		SPWarning(layer_prefix << ":" << message);
	}
	else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
	{
		SPWarning(layer_prefix << ":" << message);
	}
	else
	{
		SPWarning(layer_prefix << ":" << message);
	}
	return VK_FALSE;
}
#endif


SPVulkanInstance::~SPVulkanInstance()
{
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	if (debug_utils_messenger_ != VK_NULL_HANDLE)
	{
		vkDestroyDebugUtilsMessengerEXT(handle_, debug_utils_messenger_, nullptr);
	}
	if (debug_report_callback_ != VK_NULL_HANDLE)
	{
		vkDestroyDebugReportCallbackEXT(handle_, debug_report_callback_, nullptr);
	}
#endif

	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyInstance(handle_, nullptr);
	}
}

SPVulkanInstance::SPVulkanInstance(const sp_string& name, uint32_t api_version)
	: debug_utils_messenger_(VK_NULL_HANDLE)
	, debug_report_callback_(VK_NULL_HANDLE)
	, handle_(VK_NULL_HANDLE)
	, surface_(VK_NULL_HANDLE)
	, application_name_(name)
	, api_version_(api_version)
{

}

SPVulkanInstance& SPVulkanInstance::add_application_name(const sp_string& name)
{
	this->application_name_ = name;
	return *this;
}

SPVulkanInstance& SPVulkanInstance::add_api_version(uint32_t api_version)
{
	this->api_version_ = api_version;
	return *this;
}

void SPVulkanInstance::init()
{

	SPLog("Initializing vulkan instance.");

	SPAssert(volkInitialize(), "Failed to initialize volk.");

	uint32_t instance_extension_count;
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, nullptr));

	std::vector<VkExtensionProperties> available_instance_extensions(instance_extension_count);
	VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &instance_extension_count, available_instance_extensions.data()));


	std::vector<const char*> active_instance_extensions({ VK_KHR_SURFACE_EXTENSION_NAME });

	//debug tool 开启 soar
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	// Check if VK_EXT_debug_utils is supported, which supersedes VK_EXT_Debug_Report
	bool debug_utils = false;
	for (auto& available_extension : available_instance_extensions)
	{
		if (strcmp(available_extension.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
		{
			debug_utils = true;
			SPLog(sp_string(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) + " is available, enabling it");
			enabled_extensions_.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}
	}
	if (!debug_utils)
	{
		enabled_extensions_.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}
#endif


	//验证层开启， 需要研究具体意义 soar
#if (defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)) && defined(VKB_VALIDATION_LAYERS_GPU_ASSISTED)
	bool validation_features = false;
	{
		uint32_t layer_instance_extension_count;
		VK_CHECK(vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &layer_instance_extension_count, nullptr));

		std::vector<VkExtensionProperties> available_layer_instance_extensions(layer_instance_extension_count);
		VK_CHECK(vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &layer_instance_extension_count, available_layer_instance_extensions.data()));

		for (auto& available_extension : available_layer_instance_extensions)
		{
			if (strcmp(available_extension.extensionName, VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME) == 0)
			{
				validation_features = true;
				SPLog(sp_string(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME) + " is available, enabling it");
				enabled_extensions_.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
			}
		}
	}
#endif

	// 需要确认该功能的意义 soar
	if (headless_)
	{
		bool headless_extension = false;
		for (auto& available_extension : available_instance_extensions)
		{
			if (strcmp(available_extension.extensionName, VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME) == 0)
			{
				headless_extension = true;
				SPLog(sp_string(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME) + " is available, enabling it");
				enabled_extensions_.push_back(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME);
			}
		}
		if (!headless_extension)
		{
			SPWarning(sp_string(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME) + " is not available, disabling swapchain creation");
		}
	}
	else
	{
		enabled_extensions_.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	}

	//jiamada 应该为开启扩展单独设计一个模块
	for (auto& available_extension : available_instance_extensions)
	{
		// VK_KHR_get_physical_device_properties2 is a prerequisite of VK_KHR_performance_query
		// which will be used for stats gathering where available.
		if (strcmp(available_extension.extensionName, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) == 0)
		{
			SPLog(sp_string(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME) + " is available, enabling it");
			enabled_extensions_.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}
	}
	//枚举用户弃用扩展
	auto extension_error = false;
	for (auto extension : SPInstanceInfoManager::Instance_Extensions_)
	{
		auto extension_name = extension.first;
		auto extension_is_optional = extension.second;
		if (std::find_if(available_instance_extensions.begin(), available_instance_extensions.end(),
			[&extension_name](VkExtensionProperties available_extension) { return strcmp(available_extension.extensionName, extension_name) == 0; }) == available_instance_extensions.end())
		{
			if (extension_is_optional)
			{
				SPWarning(
					"Optional instance extension" <<
					extension_name
					<< " is not available, disabling swapchain creation"
				);

			}
			else
			{
				SPError("Required instance extension " << extension_name << " not available, cannot run");
			}
			extension_error = !extension_is_optional;
		}
		else
		{
			enabled_extensions_.push_back(extension_name);
		}
	}

	if (extension_error)
	{
		throw std::runtime_error("Required instance extensions are missing.");
	}

	uint32_t instance_layer_count;
	VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, nullptr));

	std::vector<VkLayerProperties> supported_validation_layers(instance_layer_count);
	VK_CHECK(vkEnumerateInstanceLayerProperties(&instance_layer_count, supported_validation_layers.data()));

	std::vector<const char*> requested_validation_layers(SPInstanceInfoManager::Validation_Layers_);

#ifdef VKB_VALIDATION_LAYERS
	// Determine the optimal validation layers to enable that are necessary for useful debugging
	std::vector<const char*> optimal_validation_layers = SPInstanceInfoManager::Get_Optimal_Validation_Layers(supported_validation_layers);
	requested_validation_layers.insert(requested_validation_layers.end(), optimal_validation_layers.begin(), optimal_validation_layers.end());
#endif


	if (SPInstanceInfoManager::Validate_Layers(requested_validation_layers, supported_validation_layers))
	{
		SPMessage("Enabled Validation Layers:")
			for (const auto& layer : requested_validation_layers)
			{
				SPMessage("	\t " << layer);
			}
	}
	else
	{
		throw std::runtime_error("Required validation layers are missing.");
	}


	VkApplicationInfo app_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };

	app_info.pApplicationName = application_name_.c_str();
	app_info.applicationVersion = 0;
	app_info.pEngineName = "Vulkan Samples Soar";
	app_info.engineVersion = 0;
	app_info.apiVersion = api_version_;

	VkInstanceCreateInfo instance_info = { VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };

	instance_info.pApplicationInfo = &app_info;

	instance_info.enabledExtensionCount = sp::to_u32(enabled_extensions_.size());
	instance_info.ppEnabledExtensionNames = enabled_extensions_.data();

	instance_info.enabledLayerCount = sp::to_u32(requested_validation_layers.size());
	instance_info.ppEnabledLayerNames = requested_validation_layers.data();


#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
	VkDebugReportCallbackCreateInfoEXT debug_report_create_info = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
	if (debug_utils)
	{
		debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debug_utils_create_info.pfnUserCallback = debug_utils_messenger_callback;

		instance_info.pNext = &debug_utils_create_info;
	}
	else
	{
		debug_report_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		debug_report_create_info.pfnCallback = debug_callback;

		instance_info.pNext = &debug_report_create_info;
	}
#endif

	// 开启GPU 辅助
#if (defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)) && defined(VKB_VALIDATION_LAYERS_GPU_ASSISTED)
	VkValidationFeaturesEXT validation_features_info = { VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT };
	if (validation_features)
	{
		static const VkValidationFeatureEnableEXT enable_features[2] = {
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_RESERVE_BINDING_SLOT_EXT,
			VK_VALIDATION_FEATURE_ENABLE_GPU_ASSISTED_EXT,
		};
		validation_features_info.enabledValidationFeatureCount = 2;
		validation_features_info.pEnabledValidationFeatures = enable_features;
		validation_features_info.pNext = instance_info.pNext;
		instance_info.pNext = &validation_features_info;
	}
#endif



	VK_CHECK(vkCreateInstance(&instance_info, nullptr, &handle_));

	volkLoadInstance(handle_);

#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	if (debug_utils)
	{
		VK_CHECK(vkCreateDebugUtilsMessengerEXT(handle_, &debug_utils_create_info, nullptr, &debug_utils_messenger_));
	}
	else
	{
		VK_CHECK(vkCreateDebugReportCallbackEXT(handle_, &debug_report_create_info, nullptr, &debug_report_callback_));
	}
#endif
	query_gpus();
}

void SPVulkanInstance::query_gpus()
{
	// Querying valid physical devices on the machine
	uint32_t physical_device_count{ 0 };
	VK_CHECK(vkEnumeratePhysicalDevices(handle_, &physical_device_count, nullptr));

	if (physical_device_count < 1)
	{
		throw std::runtime_error("Couldn't find a physical device that supports Vulkan.");
	}

	std::vector<VkPhysicalDevice> physical_devices;
	physical_devices.resize(physical_device_count);
	VK_CHECK(vkEnumeratePhysicalDevices(handle_, &physical_device_count, physical_devices.data()));

	// Create gpus wrapper objects from the VkPhysicalDevice's
	for (auto& physical_device : physical_devices)
	{
		gpus_.push_back(std::make_unique<SPPhysicalDevice>(*this, physical_device));
	}
}

VkSurfaceKHR SPVulkanInstance::create_surface(HWND win_id)
{
	if (handle_ == VK_NULL_HANDLE)
		surface_ = VK_NULL_HANDLE;

	VkWin32SurfaceCreateInfoKHR surface_info;
	surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surface_info.pNext = NULL;
	surface_info.flags = 0;
	surface_info.hinstance = GetModuleHandle(nullptr);
	surface_info.hwnd = win_id;
	VK_CHECK(vkCreateWin32SurfaceKHR(handle_, &surface_info, NULL, &surface_));
	return surface_;
}

VkInstance SPVulkanInstance::get_handle()
{
	return handle_;
}

VkSurfaceKHR& SPVulkanInstance::get_surface()
{
	return surface_;
}

const std::vector<const char*>& SPVulkanInstance::get_extensions()
{
	return enabled_extensions_;
}

SPPhysicalDevice& SPVulkanInstance::get_suitable_gpu()
{
	assert(!gpus_.empty() && "No physical devices were found on the system.");

	// Find a discrete GPU
	for (auto& gpu : gpus_)
	{
		if (gpu->get_properties().deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			return *gpu;
		}
	}

	// Otherwise just pick the first one
	SPWarning("Couldn't find a discrete physical device, picking default GPU");
	return *gpus_.at(0);
}

bool SPVulkanInstance::is_headless() const
{
	return headless_;
}

bool SPVulkanInstance::is_enabled(const char* extension) const
{
	return std::find_if(enabled_extensions_.begin(), enabled_extensions_.end(), [extension](const char* enabled_extension) { return strcmp(extension, enabled_extension) == 0; }) != enabled_extensions_.end();
}

bool SPVulkanInstance::validate_extensions(const std::vector<const char*>& required, const std::vector<VkExtensionProperties>& available)
{

	for (auto extension : required)
	{
		bool found = false;
		for (auto& available_extension : available)
		{
			if (strcmp(available_extension.extensionName, extension) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			return false;
		}
	}

	return true;
}

bool SPVulkanInstance::validate_layers(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& available)
{

	for (auto extension : required)
	{
		bool found = false;
		for (auto& available_extension : available)
		{
			if (strcmp(available_extension.layerName, extension) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			return false;
		}
	}

	return true;
}
