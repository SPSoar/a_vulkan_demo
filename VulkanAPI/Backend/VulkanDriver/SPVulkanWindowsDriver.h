#ifndef S_SPVULKANWINDOWSDRIVER_H_
#define S_SPVULKANWINDOWSDRIVER_H_
#include "../../Interface/SPStudioManager.h"
#include "SPSingleton.h"
#include "vulkan_common.h"
class SPBufferVulkan;
class SPTextureSampler;
struct VulkanContext {
    void pickPhysicalDevice();
    void createLogicalDevice();

    VkInstance vkInstance;
    VkPhysicalDevice physicalDevice;
    VkPhysicalDeviceProperties physicalDeviceProperties;
    VkPhysicalDeviceFeatures physicalDeviceFeatures;
    VkPhysicalDeviceMemoryProperties memoryProperties;
    VkDevice device;
    VkCommandPool commandPool;
    spuint graphicsQueueFamilyIndex;
    spuint presentQueueFamilyIndex;
    spbool debugMarkersSupported = False;
    spbool debugUtilsSupported = False;
    spbool maintenanceSupported[3] = {};
    VkQueue graphicsQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkSampleCountFlagBits maxMSAASamples;

};

class SPVulkanWindowsDriver : public SPStudioManager
{
     friend SPStudioManager;
public:
	SINGLETON_PUBLIC(SPVulkanWindowsDriver);
	virtual void initDriver() override;
    virtual void createSurface(void* nativeWindow) override;
    virtual SPBuffer* createIndexBuffer(std::vector<spint>& indices) override;
    virtual SPBuffer* createVertexBuffer(std::vector<SPVertex>& vertices) override;
    virtual void createTextureSampler(SPTextureSampler& textureSampler) override;
    virtual SPBuffer* createShaderModule(const std::vector<spchar>& code) override;
    
    virtual void destroySurface()  override;
    virtual void destroyDriver() override;

    void destroyVertexBuffer(SPBuffer* vBuffer) override;
    void destroyIndexBufferr(SPBuffer* iBuffer) override;
    void destroyShaderModule(SPBuffer* sBuffer) override;
    void destroyTextureSampler(SPTextureSampler& textureSampler) override;


    //
    virtual SPDrawingPad* createDrawingBoard() override;
    virtual SPDraft* createDraft() override;
    virtual SPMaterial* createMaterial() override;
    virtual SPWorkFlow* createWorkFlow(WorkFlowType wfType) override;

    VkSurfaceCapabilitiesKHR getSurfaceCapabilitiesInfo();
    std::vector<VkSurfaceFormatKHR> getSurfaceFormatInfo();
    std::vector<VkPresentModeKHR> getPresentModeInfo();
    const VulkanContext& getVulkanContext();
    VkFormat findDepthFormat();

    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
    void createImage(spuint width, spuint height, spuint mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, spuint mipLevels);
    void copyBufferToImage(VkBuffer buffer, VkImage image, spuint width, spuint height);
    void generateMipmaps(VkImage image, VkFormat imageFormat, spuint texWidth, spuint texHeight, spuint mipLevels);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, spuint mipLevels);
public:
    VkFormat getVulkanImgFormat(const ImgFormat& format);
    VkColorSpaceKHR getVulkanColorSpace(const ColorSpace& colorSpace);
    VkPresentModeKHR getVulkanPresentMode(const PresentMode& presentMode);
protected:
	SINGLETON_PROTECTED(SPVulkanWindowsDriver);
   
private:
    spuint findMemoryType(spuint typeFilter, VkMemoryPropertyFlags properties);
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
private:
	spbool vkDebugUtilsSupported = False;
    VulkanContext vkContext;
	VkDebugReportCallbackEXT vkDebugCallback = VK_NULL_HANDLE;
	VkDebugUtilsMessengerEXT vkDebugMessenger = VK_NULL_HANDLE;
};
#endif
