#ifndef S_SPENGINE_H_
#define S_SPENGINE_H_

#include "../vulkan_common.h"

class SPVulkanInstance;
class SPDevice;
class SPPhysicalDevice;
class SPCommandPool;
class SPEngine
{
public:
	SPEngine();
	~SPEngine();
	SPEngine(SPEngine const& v) = delete;
	SPEngine(SPEngine&& v) = delete;
	SPEngine& operator=(SPEngine& v) = delete;
	SPEngine& operator=(SPEngine&& v) = delete;
public:
	void init();
	VkSurfaceKHR create_surface(HWND win_id);
	VkSurfaceKHR& get_surface();
	SPDevice& get_device() const;
	SPVulkanInstance& get_instance() const;
	const SPCommandPool& get_command_pool();
	const VkPipelineCache& get_pipeline_cache();
	void create_queue();
	void create_fence_pool();
	bool debug_check_extension();
private:
	void add_device_extension(const char* extension, bool optional = false);
	void request_gpu_features(SPPhysicalDevice& gpu);
public:
	void create_command_pool();
	void create_pipeline_cache();
private:
	SPVulkanInstance* instance_;
	SPDevice* device_;
	std::unordered_map<const char*, bool> device_extensions_;
	//pool-----------------------
	VkPipelineCache pipeline_cache_;
	//pool-----------------------
};

#endif