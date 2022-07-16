#ifndef S_SPVULKANINSTANCE_H_
#define S_SPVULKANINSTANCE_H_

#include "../vulkan_common.h"

class SPPhysicalDevice;
class SPVulkanInstance 
{
	friend class SPRenderControlCenter;
public:
	SPVulkanInstance(const sp_string& name, uint32_t api_version);
	virtual ~SPVulkanInstance();
	void init();
	VkSurfaceKHR create_surface(HWND win_id);
	VkInstance get_handle();
	VkSurfaceKHR& get_surface();
	const std::vector<const char*>& get_extensions();
public:

	SPVulkanInstance& add_application_name(const sp_string& name);
	SPVulkanInstance& add_api_version(uint32_t api_version);
	SPPhysicalDevice& get_suitable_gpu();
	bool is_headless() const;
	bool is_enabled(const char* extension) const;

protected:
	//下面两个函数同时存在与SPVulkanInstance、SPDevice
	bool validate_extensions(const std::vector<const char*>& required, const std::vector<VkExtensionProperties>& available);
	bool validate_layers(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& available);
	void query_gpus();
protected:
	VkInstance handle_;
	std::vector<std::unique_ptr<SPPhysicalDevice>> gpus_;
#if defined(VKB_DEBUG) || defined(VKB_VALIDATION_LAYERS)
	VkDebugUtilsMessengerEXT debug_utils_messenger_;
	VkDebugReportCallbackEXT debug_report_callback_;
#endif
	std::vector<const char*> enabled_extensions_;
	bool headless_;
	std::string application_name_;
	uint32_t  api_version_;
	VkSurfaceKHR surface_;
};

#endif