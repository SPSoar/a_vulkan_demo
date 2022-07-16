#ifndef S_SPDEVICE_H_
#define S_SPDEVICE_H_

#include "../vulkan_common.h"

#include "SPPhysicalDevice.h"
#include "../Resource/SPResourceCache.h"

class SPBuffer;
class SPQueue;
class SPCommandPool;
class SPFencePool;
class SPCommandBuffer;
class SPDevice
{
public:
	SPDevice(SPPhysicalDevice& gpu, std::unordered_map<const char*, bool> requested_extensions = {});
	~SPDevice(); 
	SPDevice(const SPDevice&) = delete;
	SPDevice(SPDevice&&) = delete;
	SPDevice& operator=(const SPDevice&) = delete;
	SPDevice& operator=(SPDevice&&) = delete;
	void init();
	VkDevice get_handle() const;
	const SPPhysicalDevice& get_gpu() const;
	void create_queue(VkSurfaceKHR& surface);
	void create_command_pool();
	void create_fence_pool();
	VkSurfaceKHR& get_surface();
public:
	VkCommandBuffer create_command_buffer(VkCommandBufferLevel level, bool begin = false) const;
	void flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free = true, VkSemaphore signalSemaphore = VK_NULL_HANDLE) const;
	VkBuffer create_buffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size, VkDeviceMemory* memory, void* data = nullptr);
	void copy_buffer(SPBuffer& src, SPBuffer& dst, VkQueue queue, VkBufferCopy* copy_region = nullptr);
public:
	uint32_t get_queue_family_index(VkQueueFlagBits queue_flag);
	bool is_extension_supported(const std::string& requested_extension);
	bool is_enabled(const char* extension);
	bool is_image_format_supported(VkFormat format) const;
	const SPQueue& get_queue_by_flags(VkQueueFlags queue_flags, uint32_t queue_index);
	const SPQueue& get_suitable_graphics_queue();
	VkResult wait_idle();
	VmaAllocator get_memory_allocator() const;
	uint32_t get_memory_type(uint32_t bits, VkMemoryPropertyFlags properties, VkBool32* memory_type_found = nullptr) const;
	SPCommandBuffer &request_command_buffer() const;
	SPResourceCache& get_resource_cache();
	void add_device_extension(const char* extension, bool optional = false);
public:
	VkFence request_fence() const;
	SPFencePool& get_fence_pool() const;
	SPCommandPool& get_command_pool() const;
	const SPQueue& get_queue_by_present(uint32_t queue_index) const;
protected:
	//下面两个函数同时存在与SPVulkanInstance、SPDevice
	bool validate_extensions(const std::vector<const char*>& required, const std::vector<VkExtensionProperties>& available);
	bool validate_layers(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& available);
protected:
	SPPhysicalDevice& gpu_;
	VkSurfaceKHR surface_;
	std::vector<VkExtensionProperties> device_extensions_;
	std::vector<const char*> enabled_extensions_;

	VkDevice handle_{ VK_NULL_HANDLE };
	std::vector<std::vector<SPQueue>> queues_;

	VmaAllocator memory_allocator_{ VK_NULL_HANDLE };

	/// A command pool associated to the primary queue
	std::unique_ptr<SPCommandPool> command_pool_;
	/// A fence pool associated to the primary queue
	std::unique_ptr<SPFencePool> fence_pool_;

	std::unordered_map<const char*, bool> requested_extensions_;

	SPResourceCache resource_cache_;
};

#endif