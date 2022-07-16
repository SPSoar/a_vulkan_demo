#include "SPVulkanWindowsDriver.h"
#include "SPVulkanFunction.h"

//soar
#include "SPDrawingPadVulkan.h"
#include "SPDraftVulkan.h"
#include "SPMaterialVulkan.h"
#include "SPVulkanDrawingSkill.h"
#include "SPBufferVulkan.h"
#include "SPSamplerVulkan.h"
#include "SPShaderVulkan.h"
#include "SPWorkFlowVulkan.h"
SINGLETON_CREATE(SPVulkanWindowsDriver);

const spchar* requiredInstanceExtensions[] = { VK_KHR_WIN32_SURFACE_EXTENSION_NAME };

constexpr struct VkAllocationCallbacks* VK_ALLOC = nullptr;
constexpr static const spint VK_VERSION_MAJOR = 1;
constexpr static const spint VK_VERSION_MINOR = 0;

#define ENABLE_VALIDATION

#define ENABLE_DUMP

#define ENABLE_RENDERDOC


VKAPI_ATTR VkBool32 VKAPI_CALL debugReportCallback(VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
    int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData) {
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT) {
        std::stringstream loginfo;
        loginfo << "VULKAN ERROR: (" << pLayerPrefix << ") "
            << pMessage << loginfo.end;
        sError(loginfo.str());
    }
    else {
        std::stringstream loginfo;
        loginfo << "VULKAN WARNING: (" << pLayerPrefix << ") "
            << pMessage << loginfo.end;
        sLog(loginfo.str());
    }
    return VK_FALSE;
}



VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilsCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT types, const VkDebugUtilsMessengerCallbackDataEXT* cbdata,
    void* pUserData) {
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
     
        std::stringstream loginfo;
        loginfo << "VULKAN ERROR: (" << cbdata->pMessageIdName << ") "
            << cbdata->pMessage << loginfo.end;
        sError(loginfo.str());
    
    }
    else {
        if (strstr(cbdata->pMessage, "ALL_GRAPHICS_BIT") || strstr(cbdata->pMessage, "ALL_COMMANDS_BIT")) {
            return VK_FALSE;
        }
        std::stringstream loginfo;
        loginfo << "VULKAN WARNING: (" << cbdata->pMessageIdName << ") "
            << cbdata->pMessage << loginfo.end;
        sWarning(loginfo.str());
    }
    return VK_FALSE;
}

void SPVulkanWindowsDriver::initDriver()
{

    vkfunc::initialize();

    vkDebugUtilsSupported = true;

    //扩展相关
    spuint availableExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
    std::vector<VkExtensionProperties>  availableExtensionsProperties(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensionsProperties.data());
    for (const auto& extProps : availableExtensionsProperties) {
        if (!strcmp(extProps.extensionName, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
            vkDebugUtilsSupported = true;
            break;
        }
    }

    VkInstanceCreateInfo instanceCreateInfo = {};

    bool validationFeaturesSupported = false;

#ifdef ENABLE_VALIDATION
    const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
#ifdef ENABLE_DUMP
    "VK_LAYER_LUNARG_api_dump",
#endif
#ifdef ENABLE_RENDERDOC
    "VK_LAYER_RENDERDOC_Capture",
#endif
    }; 
    //验证层相关
    spuint layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    std::vector<const char*> enabledLayers;

      for (const auto& valLayer : validationLayers) {
        for (const VkLayerProperties& layer : availableLayers) {
            spstr availableLayer(layer.layerName);
            if (availableLayer == valLayer) {
                enabledLayers.push_back(valLayer);
            }
        }
    }

    if (!enabledLayers.empty()) {
        instanceCreateInfo.enabledLayerCount = (spuint)enabledLayers.size();
        instanceCreateInfo.ppEnabledLayerNames = enabledLayers.data();

        // Check if VK_EXT_validation_features is supported.
        spuint availableExtsCount = 0;
        vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &availableExtsCount, nullptr);
        std::vector<VkExtensionProperties> availableExts(availableExtsCount);
        vkEnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &availableExtsCount, availableExts.data());
        for (const auto& extProps : availableExts) {
            if (!strcmp(extProps.extensionName, VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME)) {
                validationFeaturesSupported = true;
                break;
            }
        }

    }
    else {
        sError("Validation layer not available; did you install the Vulkan SDK? \
            Please ensure that VK_LAYER_PATH is set correctly.");
    }
#endif

    //平台类可能需要1或2个实例扩展，加上我们在公共代码中最多需要5个实例扩展。所以最多7个。
    //以后加扩展可以在这里加
    static constexpr spuint MAX_INSTANCE_EXTENSION_COUNT = 7;
    const char* enabledExtensions[MAX_INSTANCE_EXTENSION_COUNT];
    spuint enabledExtensionCount = 0;

    // Request all cross-platform extensions.
    enabledExtensions[enabledExtensionCount++] = "VK_KHR_surface";
    enabledExtensions[enabledExtensionCount++] = "VK_KHR_get_physical_device_properties2";
#ifdef ENABLE_VALIDATION
#ifdef ANDROID
    ppEnabledExtensions[enabledExtensionCount++] = "VK_EXT_debug_report";
#endif
    if (validationFeaturesSupported) {
        enabledExtensions[enabledExtensionCount++] = "VK_EXT_validation_features";
    }
#endif

    if (vkDebugUtilsSupported) {
        enabledExtensions[enabledExtensionCount++] = "VK_EXT_debug_utils";
    }

    //需要其他额外扩展可以增加参数传递
    enabledExtensions[enabledExtensionCount++] = requiredInstanceExtensions[0];

    // Create the Vulkan instance.
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.apiVersion = VK_MAKE_VERSION(VK_VERSION_MAJOR, VK_VERSION_MINOR, 0);
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;
    instanceCreateInfo.enabledExtensionCount = enabledExtensionCount;
    instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions;

    VkValidationFeaturesEXT features = {};
    VkValidationFeatureEnableEXT enables[] = {
        VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,
        // 启用同步验证
        // VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT,
    };

    if (validationFeaturesSupported) {
        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.enabledValidationFeatureCount = sizeof(enables) / sizeof(enables[0]);
        features.pEnabledValidationFeatures = enables;
        instanceCreateInfo.pNext = &features;
    }

    VkResult result = vkCreateInstance(&instanceCreateInfo, VK_ALLOC, &vkContext.vkInstance);
    vkfunc::bindInstance(vkContext.vkInstance);
    assert(result == VK_SUCCESS);

 
   const PFN_vkCreateDebugReportCallbackEXT createDebugReportCallback =
       vkfunc::vkCreateDebugReportCallbackEXT;

#ifdef ENABLE_VALIDATION
    if (vkDebugUtilsSupported) {
        VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.pNext = nullptr;
        createInfo.flags = 0;
        //严重信息可以再增加
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        createInfo.pfnUserCallback = debugUtilsCallback;
        result = vkfunc::vkCreateDebugUtilsMessengerEXT(vkContext.vkInstance, &createInfo, VK_ALLOC, &vkDebugMessenger);
        assert(result == VK_SUCCESS);
    }
    else if (createDebugReportCallback) {
        const VkDebugReportCallbackCreateInfoEXT cbinfo = {
            VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT,
            nullptr,
            VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT,
            debugReportCallback,
            nullptr
        };
        result = createDebugReportCallback(vkContext.vkInstance, &cbinfo, VK_ALLOC, &vkDebugCallback);
        assert(result == VK_SUCCESS);
    }
#endif

    vkContext.pickPhysicalDevice();

    // Initialize device, graphicsQueue, and command buffer manager.
    vkContext.createLogicalDevice();



}

void SPVulkanWindowsDriver::createSurface(void* nativeWindow)
{

    VkWin32SurfaceCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = (HWND) nativeWindow;
    createInfo.hinstance = GetModuleHandle(nullptr);

    VkResult result = vkCreateWin32SurfaceKHR(vkContext.vkInstance, &createInfo, nullptr, &vkContext.surface);
    assert(result == VK_SUCCESS);

}

SPBuffer* SPVulkanWindowsDriver::createIndexBuffer(std::vector<spint>& indices)
{
    SPBufferVulkan* buffer = new SPBufferVulkan();
   
    buffer->elementCount = (spint)indices.size();

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();
    buffer->bufferSize = bufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    void* data;
    vkMapMemory(vkContext.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, indices.data(), (size_t)bufferSize);
    vkUnmapMemory(vkContext.device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer->buffer, buffer->bufferMemory);

    copyBuffer(stagingBuffer, buffer->buffer, bufferSize);

    vkDestroyBuffer(vkContext.device, stagingBuffer, nullptr);
    vkFreeMemory(vkContext.device, stagingBufferMemory, nullptr);

    return buffer;
}

SPBuffer* SPVulkanWindowsDriver::createVertexBuffer(std::vector<SPVertex>& vertices)
{
    SPBufferVulkan* buffer = new SPBufferVulkan();
    buffer->elementCount = vertices.size();

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    buffer->bufferSize = bufferSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vkContext.device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(vkContext.device, stagingBufferMemory);

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, buffer->buffer, buffer->bufferMemory);

    copyBuffer(stagingBuffer, buffer->buffer, bufferSize);


    vkDestroyBuffer(vkContext.device, stagingBuffer, nullptr);
    vkFreeMemory(vkContext.device, stagingBufferMemory, nullptr);

    return buffer;
}

void SPVulkanWindowsDriver::createTextureSampler(SPTextureSampler& textureSampler)
{
  
   
    spint w = textureSampler.mWidth;
    spint h = textureSampler.mHeight;
    spint c = textureSampler.mChannel;

    VkDeviceSize imageSize = w * h * c;
    
    SPSamplerVulkan* sampler = new SPSamplerVulkan();


    spuint mipLevels = static_cast<spuint>(std::floor(std::log2(std::max(w, h)))) + 1;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(vkContext.device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, textureSampler.mData, static_cast<size_t>(imageSize));
    vkUnmapMemory(vkContext.device, stagingBufferMemory);

    createImage(w, h, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, sampler->textureImage, sampler->textureImageMemory);

    transitionImageLayout(sampler->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    copyBufferToImage(stagingBuffer, sampler->textureImage, static_cast<uint32_t>(w), static_cast<uint32_t>(h));

    vkDestroyBuffer(vkContext.device, stagingBuffer, nullptr);
    vkFreeMemory(vkContext.device, stagingBufferMemory, nullptr);

    generateMipmaps(sampler->textureImage, VK_FORMAT_R8G8B8A8_SRGB, w, h, mipLevels);

    //createImageView
    sampler->textureImageView =
        createImageView(sampler->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);


    //createTextureSampler 
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(vkContext.physicalDevice, &properties);

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0.0f;

    VkResult result = vkCreateSampler(vkContext.device, &samplerInfo, nullptr, &sampler->textureSampler);
    assert(result == VK_SUCCESS);

    textureSampler.textureSamplerBuffer = sampler;
}


SPDrawingPad* SPVulkanWindowsDriver::createDrawingBoard()
{
    return  SPDrawingPadVulkan::get();
}

SPDraft* SPVulkanWindowsDriver::createDraft()
{
    return new SPDraftVulkan();
}

SPMaterial* SPVulkanWindowsDriver::createMaterial()
{
    return  new SPMaterialVulkan(*this);
}

SPWorkFlow* SPVulkanWindowsDriver::createWorkFlow(WorkFlowType wfType)
{
    return new SPWorkFlowVulkan(this, wfType);
}

SPBuffer* SPVulkanWindowsDriver::createShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(vkContext.device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    SPShaderVulkan* buffer = new SPShaderVulkan();
    buffer->shaderModule = shaderModule;
    return buffer;
}

void SPVulkanWindowsDriver::destroySurface()
{
    vkDestroySurfaceKHR(vkContext.vkInstance, vkContext.surface, nullptr);
}

void SPVulkanWindowsDriver::destroyDriver()
{
    //以后再说
}

void SPVulkanWindowsDriver::destroyVertexBuffer(SPBuffer* vBuffer)
{
    SPBufferVulkan* vVertexBuffer = static_cast<SPBufferVulkan*>(vBuffer);
    if (vVertexBuffer) 
    {
        vkDestroyBuffer(vkContext.device, vVertexBuffer->buffer, nullptr);
        vkFreeMemory(vkContext.device, vVertexBuffer->bufferMemory, nullptr);
    }
}

void SPVulkanWindowsDriver::destroyIndexBufferr(SPBuffer* iBuffer)
{
    SPBufferVulkan* vIndexBuffer = static_cast<SPBufferVulkan*>(iBuffer);
    if (vIndexBuffer)
    {
        vkDestroyBuffer(vkContext.device, vIndexBuffer->buffer, nullptr);
        vkFreeMemory(vkContext.device, vIndexBuffer->bufferMemory, nullptr);
    }
}

void SPVulkanWindowsDriver::destroyShaderModule(SPBuffer* sBuffer)
{
    SPShaderVulkan* vShaderBuffer = static_cast<SPShaderVulkan*>(sBuffer);
    if (vShaderBuffer)
    {
        vkDestroyShaderModule(vkContext.device, vShaderBuffer->shaderModule, nullptr);
    }
}

void SPVulkanWindowsDriver::destroyTextureSampler(SPTextureSampler& textureSampler)
{
    SPSamplerVulkan* vTextureBuffer = static_cast<SPSamplerVulkan*>(textureSampler.textureSamplerBuffer);
    if (vTextureBuffer) 
    {
        vkDestroySampler(vkContext.device, vTextureBuffer->textureSampler, nullptr);
        vkDestroyImageView(vkContext.device, vTextureBuffer->textureImageView, nullptr);

        vkDestroyImage(vkContext.device, vTextureBuffer->textureImage, nullptr);
        vkFreeMemory(vkContext.device, vTextureBuffer->textureImageMemory, nullptr);
    }
   
}


VkSurfaceCapabilitiesKHR SPVulkanWindowsDriver::getSurfaceCapabilitiesInfo()
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkContext.physicalDevice, vkContext.surface, &capabilities);
    return capabilities;
}

std::vector<VkSurfaceFormatKHR> SPVulkanWindowsDriver::getSurfaceFormatInfo()
{
    std::vector<VkSurfaceFormatKHR> surfaceFormatInfo;
    spuint formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext.physicalDevice, vkContext.surface, &formatCount, nullptr);

    if (formatCount != 0) {
        surfaceFormatInfo.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(vkContext.physicalDevice, vkContext.surface, &formatCount, surfaceFormatInfo.data());
    }

    return surfaceFormatInfo;
}

std::vector<VkPresentModeKHR> SPVulkanWindowsDriver::getPresentModeInfo()
{
    std::vector<VkPresentModeKHR> presentModeInfo;
    spuint presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkContext.physicalDevice, vkContext.surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        presentModeInfo.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(vkContext.physicalDevice, vkContext.surface, &presentModeCount, presentModeInfo.data());
    }
    return presentModeInfo;
}

const VulkanContext& SPVulkanWindowsDriver::getVulkanContext() 
{
    return vkContext;
}

VkFormat SPVulkanWindowsDriver::findDepthFormat()
{
    std::vector<VkFormat> candidates = { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT };
    VkImageTiling tiling = VK_IMAGE_TILING_OPTIMAL;
    VkFormatFeatureFlags features = VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT;
   
    for (VkFormat format : candidates) {
	    VkFormatProperties props;
	    vkGetPhysicalDeviceFormatProperties(vkContext.physicalDevice, format, &props);

	    if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
		    return format;
	    }
	    else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
		    return format;
	    }
	}

    return VkFormat();
}


void SPVulkanWindowsDriver::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateBuffer(vkContext.device, &bufferInfo, nullptr, &buffer);
    assert(result == VK_SUCCESS);

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(vkContext.device, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(vkContext.device, &allocInfo, nullptr, &bufferMemory);
    assert(result == VK_SUCCESS);

    vkBindBufferMemory(vkContext.device, buffer, bufferMemory, 0);
}

void SPVulkanWindowsDriver::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void SPVulkanWindowsDriver::createImage(spuint width, spuint height, spuint mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult result = vkCreateImage(vkContext.device, &imageInfo, nullptr, &image);
    assert(result == VK_SUCCESS);
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkContext.device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(vkContext.device, &allocInfo, nullptr, &imageMemory);
    assert(result == VK_SUCCESS);
    vkBindImageMemory(vkContext.device, image, imageMemory, 0);
}

void SPVulkanWindowsDriver::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, spuint mipLevels)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;

    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else {
        assert(false);
    }

    vkCmdPipelineBarrier(
        commandBuffer,
        sourceStage, destinationStage,
        0,
        0, nullptr,
        0, nullptr,
        1, &barrier
    );

    endSingleTimeCommands(commandBuffer);
}

void SPVulkanWindowsDriver::copyBufferToImage(VkBuffer buffer, VkImage image, spuint width, spuint height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };

    vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    endSingleTimeCommands(commandBuffer);
}

void SPVulkanWindowsDriver::generateMipmaps(VkImage image, VkFormat imageFormat, spuint texWidth, spuint texHeight, spuint mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(vkContext.physicalDevice, imageFormat, &formatProperties);

    if (!(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT)) {
        assert(false);
    }
  
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;

    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;

    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1, &blit,
            VK_FILTER_LINEAR);

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);

        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }

    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(commandBuffer,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
        0, nullptr,
        0, nullptr,
        1, &barrier);

    endSingleTimeCommands(commandBuffer);
}

spuint SPVulkanWindowsDriver::findMemoryType(spuint typeFilter, VkMemoryPropertyFlags properties)
{

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vkContext.physicalDevice, &memProperties);

    for (spuint i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    assert(false);
    return -1;
}

VkCommandBuffer SPVulkanWindowsDriver::beginSingleTimeCommands()
{

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkContext.commandPool;
    allocInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(vkContext.device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void SPVulkanWindowsDriver::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    vkQueueSubmit(vkContext.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(vkContext.graphicsQueue);

    vkFreeCommandBuffers(vkContext.device, vkContext.commandPool, 1, &commandBuffer);
}

VkImageView SPVulkanWindowsDriver::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, spuint mipLevels)
{

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    VkResult result = vkCreateImageView(vkContext.device, &viewInfo, nullptr, &imageView);
    assert(result == VK_SUCCESS);
    return imageView;
}

VkFormat SPVulkanWindowsDriver::getVulkanImgFormat(const ImgFormat& format)
{
    VkFormat vkFormat = VkFormat::VK_FORMAT_UNDEFINED;
    switch (format)
    {
    case ImgFormat::R8G8B8A8_SRGB:
       // vkFormat = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
        vkFormat = VkFormat::VK_FORMAT_B8G8R8A8_SRGB;
        break;
    case ImgFormat::R8G8B8A8_UINT:
        vkFormat = VkFormat::VK_FORMAT_R8G8B8A8_UINT;
        break;
    case ImgFormat::R8G8B8A8_SINT:
        vkFormat = VkFormat::VK_FORMAT_R8G8B8A8_SINT;
        break;
    case ImgFormat::R8G8B8A8_UNORM:
        vkFormat = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
        break;
    case ImgFormat::R8G8B8A8_SNORM:
        vkFormat = VkFormat::VK_FORMAT_R8G8B8A8_SNORM;
        break;

    case ImgFormat::R8_SRGB:
        vkFormat = VkFormat::VK_FORMAT_R8_SRGB;
        break;
    case ImgFormat::R8_UINT:
        vkFormat = VkFormat::VK_FORMAT_R8_UINT;
        break;
    case ImgFormat::R8_SINT:
        vkFormat = VkFormat::VK_FORMAT_R8_SINT;
        break;
    case ImgFormat::R8_UNORM:
        vkFormat = VkFormat::VK_FORMAT_R8_UNORM;
        break;
    case ImgFormat::R8_SNORM:
        vkFormat = VkFormat::VK_FORMAT_R8_SNORM;
        break;
    default:
        vkFormat = VkFormat::VK_FORMAT_R8G8B8A8_UINT;
        break;
    }

    return vkFormat;
}

VkColorSpaceKHR SPVulkanWindowsDriver::getVulkanColorSpace(const ColorSpace& colorSpace)
{
    VkColorSpaceKHR  vkColorSpace = VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR;
    switch (colorSpace)
    {
    case ColorSpace::SRGB_NONLINEAR:
        vkColorSpace = VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        break;
    default:
        vkColorSpace = VkColorSpaceKHR::VK_COLORSPACE_SRGB_NONLINEAR_KHR;
        break;
    }
    return vkColorSpace;
}

VkPresentModeKHR SPVulkanWindowsDriver::getVulkanPresentMode(const PresentMode& presentMode)
{
    VkPresentModeKHR  vkPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
    switch (presentMode)
    {
    case PresentMode::IMMEDIATE:
        vkPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
        break;
    case PresentMode::DOUBLE_BUFFER:
        vkPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_RELAXED_KHR;
        break;
    case PresentMode::TRIPLE_BUFFER:
        vkPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
        break;
    default:
        vkPresentMode = VkPresentModeKHR::VK_PRESENT_MODE_IMMEDIATE_KHR;
        break;
    }
    return vkPresentMode;
}

void VulkanContext::pickPhysicalDevice()
{
    spuint physicalDeviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
    assert(result == VK_SUCCESS && physicalDeviceCount > 0);

    std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
    result = vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, physicalDevices.data());
    assert(result == VK_SUCCESS);
   
    for (spuint i = 0; i < physicalDeviceCount; ++i) {
        physicalDevice = physicalDevices[i];
        vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);

        const spint major = VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion);
        const spint minor = VK_VERSION_MINOR(physicalDeviceProperties.apiVersion);

        // 设备是否支持所需的Vulkan版本
        if (major < VK_VERSION_MAJOR) {
            continue;
        }
        if (major == VK_VERSION_MAJOR && minor < VK_VERSION_MINOR) {
            continue;
        }

        // Does the device have any command queues that support graphics?
        // In theory we should also ensure that the device supports presentation of our
        // particular VkSurface, but we don't have a VkSurface yet so we'll skip this requirement.
        spuint queueFamiliesCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount, nullptr);
        if (queueFamiliesCount == 0) {
            continue;
        }
        std::vector<VkQueueFamilyProperties> queueFamiliesProperties(queueFamiliesCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamiliesCount,
            queueFamiliesProperties.data());
        graphicsQueueFamilyIndex = 0xffff;
        for (spuint j = 0; j < queueFamiliesCount; ++j) {
            VkQueueFamilyProperties props = queueFamiliesProperties[j];
            if (props.queueCount == 0) {
                continue;
            }
            if (props.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                graphicsQueueFamilyIndex = j;
            }
        }
        if (graphicsQueueFamilyIndex == 0xffff) continue;

        //设备是否支持VK_KHR_swapchain扩展
        spuint extensionCount;
        result = vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);
        assert(result == VK_SUCCESS);
        std::vector<VkExtensionProperties> extensions(extensionCount);
        result = vkEnumerateDeviceExtensionProperties(physicalDevice,  nullptr,  &extensionCount, extensions.data());
        assert(result == VK_SUCCESS);
      
        spbool supportsSwapchain = False;
        for (uint32_t k = 0; k < extensionCount; ++k) {
            if (!strcmp(extensions[k].extensionName, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
                supportsSwapchain = True;
            }
            if (!strcmp(extensions[k].extensionName, VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
                debugMarkersSupported = True;
            }
            if (!strcmp(extensions[k].extensionName, VK_KHR_MAINTENANCE1_EXTENSION_NAME)) {
                maintenanceSupported[0] = True;
            }
            if (!strcmp(extensions[k].extensionName, VK_KHR_MAINTENANCE2_EXTENSION_NAME)) {
                maintenanceSupported[1] = True;
            }
            if (!strcmp(extensions[k].extensionName, VK_KHR_MAINTENANCE3_EXTENSION_NAME)) {
                maintenanceSupported[2] = True;
            }
        }
        if (!supportsSwapchain) continue;

        //通过循环找到了一个支持所有特性的物理设备
        vkGetPhysicalDeviceFeatures(physicalDevice, &physicalDeviceFeatures);
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);

        // 如果设备可用，打印一些驱动程序或MoltenVK信息
        if (vkfunc::vkGetPhysicalDeviceProperties2KHR) {
            VkPhysicalDeviceDriverProperties driverProperties;
            driverProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;
          
            VkPhysicalDeviceProperties2 physicalDeviceProperties2;
            physicalDeviceProperties2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
            physicalDeviceProperties2.pNext = &driverProperties;

            vkfunc::vkGetPhysicalDeviceProperties2KHR(physicalDevice, &physicalDeviceProperties2);
          
            std::stringstream loginfo;
            loginfo << "Vulkan device driver: ";
            loginfo << driverProperties.driverName << " ";
            loginfo << "Vulkan device driver: ";
            loginfo << driverProperties.driverInfo;
            sLog(loginfo.str());
        }

        VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts 
            & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
        if (counts & VK_SAMPLE_COUNT_64_BIT) { maxMSAASamples = VK_SAMPLE_COUNT_64_BIT; }
        else if (counts & VK_SAMPLE_COUNT_32_BIT) { maxMSAASamples = VK_SAMPLE_COUNT_32_BIT; }
        else if (counts & VK_SAMPLE_COUNT_16_BIT) { maxMSAASamples = VK_SAMPLE_COUNT_16_BIT; }
        else if (counts & VK_SAMPLE_COUNT_8_BIT)  { maxMSAASamples = VK_SAMPLE_COUNT_8_BIT; }
        else if (counts & VK_SAMPLE_COUNT_4_BIT)  { maxMSAASamples = VK_SAMPLE_COUNT_4_BIT; }
        else if (counts & VK_SAMPLE_COUNT_2_BIT)  { maxMSAASamples = VK_SAMPLE_COUNT_2_BIT; }
        else                                      { maxMSAASamples = VK_SAMPLE_COUNT_1_BIT; }

        // Print out some properties of the GPU for diagnostic purposes.
        //
        // Ideally, the vendors register their vendor ID's with Khronos so that apps can make an
        // id => string mapping. However in practice this hasn't happened. At the time of this
        // writing the gpuinfo database has the following ID's:
        //
        //     0x1002 - AMD
        //     0x1010 - ImgTec
        //     0x10DE - NVIDIA
        //     0x13B5 - ARM
        //     0x5143 - Qualcomm
        //     0x8086 - INTEL
        //
        // Since we don't have any vendor-specific workarounds yet, there's no need to make this
        // mapping in code. The "deviceName" string informally reveals the marketing name for the
        // GPU. (e.g., Quadro)

        const spuint driverVersion = physicalDeviceProperties.driverVersion;
        const spuint vendorID = physicalDeviceProperties.vendorID;
        const spuint deviceID = physicalDeviceProperties.deviceID;
        std::stringstream loginfo;
        loginfo << "Selected physical device '"
            << physicalDeviceProperties.deviceName
            << "' from " << physicalDeviceCount << " physical devices. "
            << "(vendor " << loginfo.hex << vendorID << ", "
            << "device " << deviceID << ", "
            << "driver " << driverVersion << ", "
            << loginfo.dec << "api " << major << "." << minor << ")"
            << loginfo.end;
        sLog(loginfo.str());
        return;
    }
    sError("failed to find suitable device.");
}

void VulkanContext::createLogicalDevice()
{
  
    const spfloat queuePriority = 1.0f;
    std::vector<const spchar*> deviceExtensionNames;
    deviceExtensionNames.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    if (debugMarkersSupported && !debugUtilsSupported) {
        deviceExtensionNames.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
    }
    if (maintenanceSupported[0]) {
        deviceExtensionNames.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME);
    }
    if (maintenanceSupported[1]) {
        deviceExtensionNames.push_back(VK_KHR_MAINTENANCE2_EXTENSION_NAME);
    }
    if (maintenanceSupported[2]) {
        deviceExtensionNames.push_back(VK_KHR_MAINTENANCE3_EXTENSION_NAME);
    }

    VkDeviceQueueCreateInfo deviceQueueCreateInfo[1] = {};
    deviceQueueCreateInfo->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo->queueFamilyIndex = graphicsQueueFamilyIndex;
    deviceQueueCreateInfo->queueCount = 1;
    deviceQueueCreateInfo->pQueuePriorities = &queuePriority;
  
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfo;

    //启用所有受支持的功能会影响性能，所以只启用需要的功能。
    const auto& supportedFeatures = physicalDeviceFeatures;
    VkPhysicalDeviceFeatures enabledFeatures;
    enabledFeatures.samplerAnisotropy = supportedFeatures.samplerAnisotropy;
    enabledFeatures.textureCompressionETC2 = supportedFeatures.textureCompressionETC2;
    enabledFeatures.textureCompressionBC = supportedFeatures.textureCompressionBC;

    deviceCreateInfo.pEnabledFeatures = &enabledFeatures;
    deviceCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensionNames.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensionNames.data();

#ifdef ENABLE_VALIDATION
    const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation",
#ifdef ENABLE_DUMP
    "VK_LAYER_LUNARG_api_dump",
#endif
#ifdef ENABLE_RENDERDOC
    "VK_LAYER_RENDERDOC_Capture",
#endif
    };
    //验证层相关
    spuint layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    std::vector<const char*> enabledLayers;

    for (const auto& valLayer : validationLayers) {
        for (const VkLayerProperties& layer : availableLayers) {
            spstr availableLayer(layer.layerName);
            if (availableLayer == valLayer) {
                enabledLayers.push_back(valLayer);
            }
        }
    }
    deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(enabledLayers.size());
    deviceCreateInfo.ppEnabledLayerNames = enabledLayers.data();
#endif
    VkResult result = vkCreateDevice(physicalDevice, &deviceCreateInfo, VK_ALLOC,
        &device);
    assert(result == VK_SUCCESS);


    vkGetDeviceQueue(device, graphicsQueueFamilyIndex, 0, &graphicsQueue);
    vkGetDeviceQueue(device, presentQueueFamilyIndex, 0, &presentQueue);

    //CommandPool创建放在这里了
    VkCommandPoolCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags =
        VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    createInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
    result = vkCreateCommandPool(device, &createInfo, VK_ALLOC, &commandPool);
    assert(result == VK_SUCCESS);

    ////QueryPool创建放在这里了
    //VkQueryPoolCreateInfo tqpCreateInfo = {};
    //tqpCreateInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    //tqpCreateInfo.queryType = VK_QUERY_TYPE_TIMESTAMP;
    //result = vkCreateQueryPool(device, &tqpCreateInfo, VK_ALLOC, &timestamps.pool);

}
