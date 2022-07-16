#include "SPDevice.h"
#include "../Command/SPFencePool.h"
#include "../Command/SPCommandPool.h"
#include "../Command/SPQueue.h"
#include "../Buffer/SPBuffer.h"
#define VMA_IMPLEMENTATION
#include "../vk_vma/vk_mem_alloc.h"

SPDevice::SPDevice(SPPhysicalDevice& gpu_, std::unordered_map<const char*, bool> requested_extensions)
	: gpu_(gpu_)
	, requested_extensions_(requested_extensions)
	, resource_cache_{ *this }
{

}

SPDevice::~SPDevice()
{
	//resource_cache.clear();

	command_pool_.reset();
	fence_pool_.reset();

	if (memory_allocator_ != VK_NULL_HANDLE)
	{
		VmaStats stats;
		vmaCalculateStats(memory_allocator_, &stats);

		SPLog("Total device memory leaked: "<< stats.total.usedBytes << "bytes.");

		vmaDestroyAllocator(memory_allocator_);
	}

	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyDevice(handle_, nullptr);
	}
}

void SPDevice::init()
{
	SPMessage("Selected GPU: " << gpu_.get_properties().deviceName);

	// Prepare the device queues
	uint32_t                             queue_family_properties_count = sp::to_u32(gpu_.get_queue_family_properties().size());
	std::vector<VkDeviceQueueCreateInfo> queue_create_infos(queue_family_properties_count, { VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO });
	std::vector<std::vector<float>>      queue_priorities(queue_family_properties_count);

	for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties_count; ++queue_family_index)
	{
		const VkQueueFamilyProperties& queue_family_property = gpu_.get_queue_family_properties()[queue_family_index];

		if (gpu_.has_high_priority_graphics_queue())
		{
			uint32_t graphics_queue_family = get_queue_family_index(VK_QUEUE_GRAPHICS_BIT);
			if (graphics_queue_family == queue_family_index)
			{
				queue_priorities[queue_family_index].reserve(queue_family_property.queueCount);
				queue_priorities[queue_family_index].push_back(1.0f);
				for (uint32_t i = 1; i < queue_family_property.queueCount; i++)
				{
					queue_priorities[queue_family_index].push_back(0.5f);
				}
			}
			else
			{
				queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
			}
		}
		else
		{
			queue_priorities[queue_family_index].resize(queue_family_property.queueCount, 0.5f);
		}

		VkDeviceQueueCreateInfo& queue_create_info = queue_create_infos[queue_family_index];

		queue_create_info.queueFamilyIndex = queue_family_index;
		queue_create_info.queueCount = queue_family_property.queueCount;
		queue_create_info.pQueuePriorities = queue_priorities[queue_family_index].data();
	}

	// Check extensions to enable Vma Dedicated Allocation
	uint32_t device_extension_count;
	VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu_.get_handle(), nullptr, &device_extension_count, nullptr));
	device_extensions_ = std::vector<VkExtensionProperties>(device_extension_count);
	VK_CHECK(vkEnumerateDeviceExtensionProperties(gpu_.get_handle(), nullptr, &device_extension_count, device_extensions_.data()));

	// Display supported extensions
	if (device_extensions_.size() > 0)
	{
		SPLog("Device supports the following extensions:");
		for (auto& extension : device_extensions_)
		{
			SPLog("  \t"<< extension.extensionName);
		}
	}
	//这里开启了额外扩展
	bool can_get_memory_requirements = is_extension_supported("VK_KHR_get_memory_requirements2");
	bool has_dedicated_allocation = is_extension_supported("VK_KHR_dedicated_allocation");

	if (can_get_memory_requirements && has_dedicated_allocation)
	{
		enabled_extensions_.push_back("VK_KHR_get_memory_requirements2");
		enabled_extensions_.push_back("VK_KHR_dedicated_allocation");

		SPLog("Dedicated Allocation enabled");
	}

	// For performance queries, we also use host query reset since queryPool resets cannot
	// live in the same command buffer as beginQuery
	if (is_extension_supported("VK_KHR_performance_query") &&
		is_extension_supported("VK_EXT_host_query_reset"))
	{
		auto perf_counter_features = gpu_.request_extension_features<VkPhysicalDevicePerformanceQueryFeaturesKHR>
				(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PERFORMANCE_QUERY_FEATURES_KHR);
		auto host_query_reset_features = gpu_.request_extension_features<VkPhysicalDeviceHostQueryResetFeatures>
				(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES);

		if (perf_counter_features.performanceCounterQueryPools && host_query_reset_features.hostQueryReset)
		{
			enabled_extensions_.push_back("VK_KHR_performance_query");
			enabled_extensions_.push_back("VK_EXT_host_query_reset");
			SPLog("Performance query enabled");
		}
	}

	// Query whether the device supports buffer device addresses
	VkPhysicalDeviceVulkan12Features features12;
	features12.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
	features12.pNext = VK_NULL_HANDLE;
	VkPhysicalDeviceFeatures2 features2;
	features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
	features2.pNext = &features12;
	vkGetPhysicalDeviceFeatures2(gpu_.get_handle(), &features2);

	bool m_supports_buffer_device = features12.bufferDeviceAddress;

	if (m_supports_buffer_device)
	{
		enabled_extensions_.push_back("VK_KHR_buffer_device_address");
		auto& features = gpu_.request_extension_features<VkPhysicalDeviceBufferDeviceAddressFeaturesKHR>(
			VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR);
		features.bufferDeviceAddress = VK_TRUE;
	}

	//enabled_extensions_.push_back(VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME);
	//enabled_extensions_.push_back(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME);
	//enabled_extensions_.push_back(VK_KHR_MULTIVIEW_EXTENSION_NAME);
	//enabled_extensions_.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
	//enabled_extensions_.push_back(VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME);

	enabled_extensions_.push_back(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME);
	enabled_extensions_.push_back(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME);
	enabled_extensions_.push_back(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME);
	enabled_extensions_.push_back(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME);
	enabled_extensions_.push_back(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);
	enabled_extensions_.push_back(VK_KHR_SPIRV_1_4_EXTENSION_NAME);
	enabled_extensions_.push_back(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME);

	//// buffer_device_address 开启特性
	//// Need to enable the bufferDeviceAddress feature.
	//auto& features = gpu_.request_extension_features<VkPhysicalDeviceBufferDeviceAddressFeaturesKHR>(
	//	VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR);
	//features.bufferDeviceAddress = VK_TRUE;

	// Check that extensions are supported before trying to create the device
	std::vector<const char*> unsupported_extensions{};
	for (auto& extension : requested_extensions_)
	{
		if (is_extension_supported(extension.first))
		{
			enabled_extensions_.emplace_back(extension.first);
		}
		else
		{
			unsupported_extensions.emplace_back(extension.first);
		}
	}

	if (enabled_extensions_.size() > 0)
	{
		SPLog("Device supports the following requested extensions:");
		for (auto& extension : enabled_extensions_)
		{
			SPLog("  \t" << extension);
		}
	}

	if (unsupported_extensions.size() > 0)
	{
		auto error = false;
		for (auto& extension : unsupported_extensions)
		{
			auto extension_is_optional = requested_extensions_[extension];
			if (extension_is_optional)
			{
				SPWarning("Optional device extension {} not available, some features may be disabled" << extension);
			}
			else
			{
				SPWarning("Required device extension {} not available, cannot run" << extension);
			}
			error = !extension_is_optional;
		}

		if (error)
		{
			throw VulkanException(VK_ERROR_EXTENSION_NOT_PRESENT, "Extensions not present");
		}
	}

	VkDeviceCreateInfo create_info{ VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO };

	// Latest requested feature will have the pNext's all set up for device creation.
	create_info.pNext = gpu_.get_extension_feature_chain();

	create_info.pQueueCreateInfos = queue_create_infos.data();
	create_info.queueCreateInfoCount = sp::to_u32(queue_create_infos.size());
	create_info.enabledExtensionCount = sp::to_u32(enabled_extensions_.size());
	create_info.ppEnabledExtensionNames = enabled_extensions_.data();

	const auto requested_gpu_features = gpu_.get_requested_features();
	create_info.pEnabledFeatures = &requested_gpu_features;

	VkResult result = vkCreateDevice(gpu_.get_handle(), &create_info, nullptr, &handle_);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create device" };
	}

	


	VmaVulkanFunctions vma_vulkan_func{};
	vma_vulkan_func.vkAllocateMemory = vkAllocateMemory;
	vma_vulkan_func.vkBindBufferMemory = vkBindBufferMemory;
	vma_vulkan_func.vkBindImageMemory = vkBindImageMemory;
	vma_vulkan_func.vkCreateBuffer = vkCreateBuffer;
	vma_vulkan_func.vkCreateImage = vkCreateImage;
	vma_vulkan_func.vkDestroyBuffer = vkDestroyBuffer;
	vma_vulkan_func.vkDestroyImage = vkDestroyImage;
	vma_vulkan_func.vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges;
	vma_vulkan_func.vkFreeMemory = vkFreeMemory;
	vma_vulkan_func.vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements;
	vma_vulkan_func.vkGetImageMemoryRequirements = vkGetImageMemoryRequirements;
	vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
	vma_vulkan_func.vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties;
	vma_vulkan_func.vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges;
	vma_vulkan_func.vkMapMemory = vkMapMemory;
	vma_vulkan_func.vkUnmapMemory = vkUnmapMemory;
	vma_vulkan_func.vkCmdCopyBuffer = vkCmdCopyBuffer;

	VmaAllocatorCreateInfo allocator_info{};
	allocator_info.physicalDevice = gpu_.get_handle();
	allocator_info.device = handle_;
	allocator_info.instance = gpu_.get_instance().get_handle();

	if (can_get_memory_requirements && has_dedicated_allocation)
	{
		allocator_info.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
		vma_vulkan_func.vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR;
		vma_vulkan_func.vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR;
	}

	if (is_extension_supported(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME) && is_enabled(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
	{
		allocator_info.flags |= VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
	}

	allocator_info.pVulkanFunctions = &vma_vulkan_func;

	result = vmaCreateAllocator(&allocator_info, &memory_allocator_);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create allocator" };
	}
}

VkDevice SPDevice::get_handle() const
{

	return handle_;
}

const SPPhysicalDevice& SPDevice::get_gpu() const
{
	return gpu_;
}

void SPDevice::create_queue(VkSurfaceKHR& surface)
{
	surface_ = surface;
	uint32_t queue_family_properties_count = sp::to_u32(gpu_.get_queue_family_properties().size());
	queues_.resize(queue_family_properties_count);

	for (uint32_t queue_family_index = 0U; queue_family_index < queue_family_properties_count; ++queue_family_index)
	{
		const VkQueueFamilyProperties& queue_family_property = gpu_.get_queue_family_properties()[queue_family_index];

		VkBool32 present_supported = gpu_.is_present_supported(surface, queue_family_index);

		// Only check if surface is valid to allow for headless applications
		if (surface != VK_NULL_HANDLE)
		{
			VK_CHECK(vkGetPhysicalDeviceSurfaceSupportKHR(gpu_.get_handle(), queue_family_index, surface, &present_supported));
		}

		for (uint32_t queue_index = 0U; queue_index < queue_family_property.queueCount; ++queue_index)
		{
			queues_[queue_family_index].emplace_back(*this, queue_family_index, queue_family_property, present_supported, queue_index);
		}
	}
}

void SPDevice::create_command_pool()
{
	command_pool_ = std::make_unique<SPCommandPool>(*this, get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT, 0).get_family_index());
}

void SPDevice::create_fence_pool()
{
	fence_pool_ = std::make_unique<SPFencePool>(*this);
}

VkSurfaceKHR& SPDevice::get_surface()
{
	return surface_;
}

VkCommandBuffer SPDevice::create_command_buffer(VkCommandBufferLevel level, bool begin) const
{

	assert(command_pool_ && "No command pool exists in the device");

	VkCommandBufferAllocateInfo cmd_buf_allocate_info{};
	cmd_buf_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmd_buf_allocate_info.commandPool = command_pool_->get_handle();
	cmd_buf_allocate_info.level = level;
	cmd_buf_allocate_info.commandBufferCount = 1;

	VkCommandBuffer command_buffer;
	VK_CHECK(vkAllocateCommandBuffers(handle_, &cmd_buf_allocate_info, &command_buffer));

	// If requested, also start recording for the new command buffer
	if (begin)
	{
		VkCommandBufferBeginInfo command_buffer_info{};
		command_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_CHECK(vkBeginCommandBuffer(command_buffer, &command_buffer_info));
	}

	return command_buffer;
}

void SPDevice::flush_command_buffer(VkCommandBuffer command_buffer, VkQueue queue, bool free, VkSemaphore signalSemaphore) const
{
	if (command_buffer == VK_NULL_HANDLE)
	{
		return;
	}

	VK_CHECK(vkEndCommandBuffer(command_buffer));

	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submit_info.commandBufferCount = 1;
	submit_info.pCommandBuffers = &command_buffer;
	if (signalSemaphore)
	{
		submit_info.pSignalSemaphores = &signalSemaphore;
		submit_info.signalSemaphoreCount = 1;
	}

	// Create fence to ensure that the command buffer has finished executing
	VkFenceCreateInfo fence_info{};
	fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_info.flags = VK_FLAGS_NONE;

	VkFence fence;
	VK_CHECK(vkCreateFence(handle_, &fence_info, nullptr, &fence));

	// Submit to the queue
	VkResult result = vkQueueSubmit(queue, 1, &submit_info, fence);
	// Wait for the fence to signal that command buffer has finished executing
	VK_CHECK(vkWaitForFences(handle_, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT));

	vkDestroyFence(handle_, fence, nullptr);

	if (command_pool_ && free)
	{
		vkFreeCommandBuffers(handle_, command_pool_->get_handle(), 1, &command_buffer);
	}
}

VkBuffer SPDevice::create_buffer(VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkDeviceSize size, VkDeviceMemory* memory, void* data)
{
	VkBuffer buffer = VK_NULL_HANDLE;

	// Create the buffer handle
	VkBufferCreateInfo buffer_create_info{};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.usage = usage;
	buffer_create_info.size = size;
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	VK_CHECK(vkCreateBuffer(handle_, &buffer_create_info, nullptr, &buffer));

	// Create the memory backing up the buffer handle
	VkMemoryRequirements memory_requirements;
	VkMemoryAllocateInfo memory_allocation{};
	memory_allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	vkGetBufferMemoryRequirements(handle_, buffer, &memory_requirements);
	memory_allocation.allocationSize = memory_requirements.size;
	// Find a memory type index that fits the properties of the buffer
	memory_allocation.memoryTypeIndex = get_memory_type(memory_requirements.memoryTypeBits, properties);
	VK_CHECK(vkAllocateMemory(handle_, &memory_allocation, nullptr, memory));

	// If a pointer to the buffer data has been passed, map the buffer and copy over the
	if (data != nullptr)
	{
		void* mapped;
		VK_CHECK(vkMapMemory(handle_, *memory, 0, size, 0, &mapped));
		memcpy(mapped, data, static_cast<size_t>(size));
		// If host coherency hasn't been requested, do a manual flush to make writes visible
		if ((properties & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
		{
			VkMappedMemoryRange mapped_range{};
			mapped_range.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mapped_range.memory = *memory;
			mapped_range.offset = 0;
			mapped_range.size = size;
			vkFlushMappedMemoryRanges(handle_, 1, &mapped_range);
		}
		vkUnmapMemory(handle_, *memory);
	}

	// Attach the memory to the buffer object
	VK_CHECK(vkBindBufferMemory(handle_, buffer, *memory, 0));

	return buffer;
}

void SPDevice::copy_buffer(SPBuffer& src, SPBuffer& dst, VkQueue queue, VkBufferCopy* copy_region)
{
	assert(dst.get_size() <= src.get_size());
	assert(src.get_handle());

	VkCommandBuffer command_buffer = create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

	VkBufferCopy buffer_copy{};
	if (copy_region == nullptr)
	{
		buffer_copy.size = src.get_size();
	}
	else
	{
		buffer_copy = *copy_region;
	}

	vkCmdCopyBuffer(command_buffer, src.get_handle(), dst.get_handle(), 1, &buffer_copy);

	flush_command_buffer(command_buffer, queue);
}

uint32_t SPDevice::get_queue_family_index(VkQueueFlagBits queue_flag)
{

	const auto& queue_family_properties = gpu_.get_queue_family_properties();

	// Dedicated queue for compute
	// Try to find a queue family index that supports compute but not graphics
	if (queue_flag & VK_QUEUE_COMPUTE_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
		{
			if ((queue_family_properties[i].queueFlags & queue_flag) && ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	// Dedicated queue for transfer
	// Try to find a queue family index that supports transfer but not graphics and compute
	if (queue_flag & VK_QUEUE_TRANSFER_BIT)
	{
		for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
		{
			if ((queue_family_properties[i].queueFlags & queue_flag) && ((queue_family_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queue_family_properties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
			{
				return i;
				break;
			}
		}
	}

	// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
	for (uint32_t i = 0; i < static_cast<uint32_t>(queue_family_properties.size()); i++)
	{
		if (queue_family_properties[i].queueFlags & queue_flag)
		{
			return i;
			break;
		}
	}

	throw std::runtime_error("Could not find a matching queue family index");
}

bool SPDevice::is_extension_supported(const std::string& requested_extension)
{
	return std::find_if(device_extensions_.begin(), device_extensions_.end(),
		[requested_extension](auto& device_extension) {
			return std::strcmp(device_extension.extensionName, requested_extension.c_str()) == 0;
		}) != device_extensions_.end();
}

bool SPDevice::is_enabled(const char* extension)
{
	return std::find_if(enabled_extensions_.begin(), enabled_extensions_.end(), [extension](const char* enabled_extension) { return strcmp(extension, enabled_extension) == 0; }) != enabled_extensions_.end();
}

bool SPDevice::is_image_format_supported(VkFormat format) const
{
	VkImageFormatProperties format_properties;

	auto result = vkGetPhysicalDeviceImageFormatProperties(gpu_.get_handle(),
		format,
		VK_IMAGE_TYPE_2D,
		VK_IMAGE_TILING_OPTIMAL,
		VK_IMAGE_USAGE_SAMPLED_BIT,
		0,        // no create flags
		&format_properties);
	return result != VK_ERROR_FORMAT_NOT_SUPPORTED;
}

const SPQueue& SPDevice::get_queue_by_flags(VkQueueFlags required_queue_flags, uint32_t queue_index)
{

	for (uint32_t queue_family_index = 0U; queue_family_index < queues_.size(); ++queue_family_index)
	{
		SPQueue& first_queue = queues_[queue_family_index][0];

		VkQueueFlags queue_flags = first_queue.get_properties().queueFlags;
		uint32_t     queue_count = first_queue.get_properties().queueCount;

		if (((queue_flags & required_queue_flags) == required_queue_flags) && queue_index < queue_count)
		{
			return queues_[queue_family_index][queue_index];
		}
	}

	throw std::runtime_error("Queue not found");
}

const SPQueue& SPDevice::get_suitable_graphics_queue()
{
	for (uint32_t queue_family_index = 0U; queue_family_index < queues_.size(); ++queue_family_index)
	{
		SPQueue& first_queue = queues_[queue_family_index][0];

		uint32_t queue_count = first_queue.get_properties().queueCount;

		if (first_queue.support_present() && 0 < queue_count)
		{
			return queues_[queue_family_index][0];
		}
	}

	return get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);
}

VkResult SPDevice::wait_idle()
{
	return vkDeviceWaitIdle(handle_);
}

VmaAllocator SPDevice::get_memory_allocator() const
{
	return memory_allocator_;
}

uint32_t SPDevice::get_memory_type(uint32_t bits, VkMemoryPropertyFlags properties, VkBool32* memory_type_found) const
{
	for (uint32_t i = 0; i < gpu_.get_memory_properties().memoryTypeCount; i++)
	{
		if ((bits & 1) == 1)
		{
			if ((gpu_.get_memory_properties().memoryTypes[i].propertyFlags & properties) == properties)
			{
				if (memory_type_found)
				{
					*memory_type_found = true;
				}
				return i;
			}
		}
		bits >>= 1;
	}

	if (memory_type_found)
	{
		*memory_type_found = false;
		return 0;
	}
	else
	{
		throw std::runtime_error("Could not find a matching memory type");
	}
}

SPCommandBuffer& SPDevice::request_command_buffer() const
{
	return command_pool_->request_command_buffer();
}

SPResourceCache& SPDevice::get_resource_cache()
{
	return resource_cache_;
}

void SPDevice::add_device_extension(const char* extension, bool optional)
{
}

VkFence SPDevice::request_fence() const
{

	return fence_pool_->request_fence();
}

SPFencePool& SPDevice::get_fence_pool() const
{
	return *fence_pool_;
}

SPCommandPool& SPDevice::get_command_pool() const
{
	return *command_pool_;
}

const SPQueue& SPDevice::get_queue_by_present(uint32_t queue_index) const
{
	for (uint32_t queue_family_index = 0U; queue_family_index < queues_.size(); ++queue_family_index)
	{
		SPQueue const& first_queue = queues_[queue_family_index][0];

		uint32_t queue_count = first_queue.get_properties().queueCount;

		if (first_queue.support_present() && queue_index < queue_count)
		{
			return queues_[queue_family_index][queue_index];
		}
	}

	throw std::runtime_error("Queue not found");
}

bool SPDevice::validate_extensions(const std::vector<const char*>& required, const std::vector<VkExtensionProperties>& available)
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

	return true;;
}

bool SPDevice::validate_layers(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& available)
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
