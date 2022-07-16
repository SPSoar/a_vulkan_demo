#include "SPEngine.h"
#include "../Driver/SPInstanceInfoManager.h"
#include "../Driver/SPVulkanInstance.h"
#include "../Driver/SPPhysicalDevice.h"
#include "../Driver/SPDevice.h"

#include "../Command/SPQueue.h"

SPEngine::SPEngine()
	: instance_{nullptr}
	, device_{ nullptr }
	, device_extensions_{}
	, pipeline_cache_{ VK_NULL_HANDLE }

{

}

SPEngine::~SPEngine()
{
	//pool µÄÏú»Ù £¿£¿£¿

	SP_DELETE_OBJECT_PTR(instance_);
	SP_DELETE_OBJECT_PTR(device_);
}

void SPEngine::init()
{

#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	active_instance_extensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
	SPInstanceInfoManager::Add_Instance_Extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME, true);
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
	active_instance_extensions.push_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	active_instance_extensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
	active_instance_extensions.push_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#else
#	pragma error Platform not supported
#endif

	SPInstanceInfoManager::Add_Instance_Extension(VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME, true);
	SPInstanceInfoManager::Add_Instance_Extension(VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME, true);
	SPInstanceInfoManager::Add_Instance_Extension(VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME, true);
	SPInstanceInfoManager::Add_Instance_Extension(VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME, true);
	SPInstanceInfoManager::Add_Instance_Extension(VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, true);
	SPInstanceInfoManager::Add_Instance_Extension(VK_KHR_SPIRV_1_4_EXTENSION_NAME, true);
	SPInstanceInfoManager::Add_Instance_Extension(VK_KHR_SHADER_FLOAT_CONTROLS_EXTENSION_NAME, true);
	
	SP_CREATE_OBJECT_PTR(instance_, SPVulkanInstance, "sp_vk_api", VK_API_VERSION_1_0);

	auto& gpu = instance_->get_suitable_gpu();
	bool high_priority_graphics_queue = false;
	gpu.set_high_priority_graphics_queue_enable(high_priority_graphics_queue);

	// Request to enable ASTC ÎÆÀíÑ¹Ëõ
	if (gpu.get_features().textureCompressionASTC_LDR)
	{
		gpu.get_mutable_requested_features().textureCompressionASTC_LDR = VK_TRUE;
	}

	// Request sample required GPU features
	request_gpu_features(gpu);


	// Creating vulkan device, specifying the swapchain extension always
	if (!instance_->is_headless() || instance_->is_enabled(VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME))
	{
		add_device_extension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	SP_CREATE_OBJECT_PTR(device_, SPDevice, gpu, device_extensions_);

	create_pipeline_cache();
}

VkSurfaceKHR SPEngine::create_surface(HWND win_id)
{
	return instance_->create_surface(win_id);
}

VkSurfaceKHR& SPEngine::get_surface()
{
	return instance_->get_surface();
}

SPDevice& SPEngine::get_device() const
{
	return *device_;
}

SPVulkanInstance& SPEngine::get_instance() const
{
	return *instance_;
}

const SPCommandPool& SPEngine::get_command_pool()
{
	return device_->get_command_pool();
}

const VkPipelineCache& SPEngine::get_pipeline_cache()
{
	return pipeline_cache_;
}

void SPEngine::create_queue()
{
	device_->create_queue(get_surface());
}

void SPEngine::create_fence_pool()
{
	device_->create_fence_pool();
}

bool SPEngine::debug_check_extension()
{
	std::vector<const char*> enabled_instance_extensions = instance_->get_extensions();
	for (auto& enabled_extension : enabled_instance_extensions)
	{
		if (strcmp(enabled_extension, VK_EXT_DEBUG_UTILS_EXTENSION_NAME) == 0)
		{
			return true;
		}
	}
}

void SPEngine::add_device_extension(const char* extension, bool optional)
{
	device_extensions_[extension] = optional;
}

void SPEngine::request_gpu_features(SPPhysicalDevice& gpu)
{
	//
	gpu.get_mutable_requested_features().fillModeNonSolid = gpu.get_features().fillModeNonSolid;
	gpu.get_mutable_requested_features().wideLines = gpu.get_features().wideLines;
	//
	if (gpu.get_features().samplerAnisotropy)
	{
		gpu.get_mutable_requested_features().samplerAnisotropy = VK_TRUE;
	}
	//
	if (true)
	{
		auto& requested_dynamic_rendering = gpu.request_extension_features<VkPhysicalDeviceDynamicRenderingFeaturesKHR>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES_KHR);
		requested_dynamic_rendering.dynamicRendering = VK_TRUE;
	}

	//
	gpu.get_mutable_requested_features().shaderSampledImageArrayDynamicIndexing = VK_TRUE;

	auto& features = gpu.request_extension_features<VkPhysicalDeviceDescriptorIndexingFeaturesEXT>(
		VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT);

	features.shaderSampledImageArrayNonUniformIndexing = VK_TRUE;

	// These are required to support the 4 descriptor binding flags we use in this sample.
	features.descriptorBindingSampledImageUpdateAfterBind = VK_TRUE;
	features.descriptorBindingPartiallyBound = VK_TRUE;
	features.descriptorBindingUpdateUnusedWhilePending = VK_TRUE;
	features.descriptorBindingVariableDescriptorCount = VK_TRUE;

	// Enables use of runtimeDescriptorArrays in SPIR-V shaders.
	features.runtimeDescriptorArray = VK_TRUE;

	// Enable the shading rate attachment feature required by this sample
	// These are passed to device creation via a pNext structure chain
	auto& requested_extension_features = gpu.request_extension_features<VkPhysicalDeviceFragmentShadingRateFeaturesKHR>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR);
	requested_extension_features.attachmentFragmentShadingRate = VK_TRUE;
	requested_extension_features.pipelineFragmentShadingRate = VK_FALSE;
	requested_extension_features.primitiveFragmentShadingRate = VK_FALSE;

	// Enable extension features required by this sample
	// These are passed to device creation via a pNext structure chain
	auto& requested_buffer_device_address_features = gpu.request_extension_features<VkPhysicalDeviceBufferDeviceAddressFeatures>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES);
	requested_buffer_device_address_features.bufferDeviceAddress = VK_TRUE;
	auto& requested_ray_tracing_features = gpu.request_extension_features<VkPhysicalDeviceRayTracingPipelineFeaturesKHR>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR);
	requested_ray_tracing_features.rayTracingPipeline = VK_TRUE;
	auto& requested_acceleration_structure_features = gpu.request_extension_features<VkPhysicalDeviceAccelerationStructureFeaturesKHR>(VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR);
	requested_acceleration_structure_features.accelerationStructure = VK_TRUE;

}


void SPEngine::create_command_pool()
{
	device_->create_command_pool();
}

void SPEngine::create_pipeline_cache()
{
	VkPipelineCacheCreateInfo pipeline_cache_create_info = {};
	pipeline_cache_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	VK_CHECK(vkCreatePipelineCache(get_device().get_handle(), &pipeline_cache_create_info, nullptr, &pipeline_cache_));
}
