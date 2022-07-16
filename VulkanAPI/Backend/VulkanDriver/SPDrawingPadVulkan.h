#ifndef S_SPDRAWINGPADVULKAN_H_
#define S_SPDRAWINGPADVULKAN_H_
#include "SPVulkanWindowsDriver.h"
#include "SPVulkanArtist.h"
#include "../../Interface/SPDrawingPad.h"
//soar
struct UniformBufferObject {
	alignas(16) spmat4 model;
	alignas(16) spmat4 view;
	alignas(16) spmat4 proj;
};
class SPNeMu;
class SPStudioManager;
class SPVulkanDrawingSkill;
class SPWorkFlowVulkan;
class SPWorkFlow;
class  SPDrawingPadVulkan : public SPDrawingPad
{
	friend SPWorkFlowVulkan;
	friend SPVulkanDrawingSkill;
public:
	SINGLETON_PUBLIC(SPDrawingPadVulkan);

	void powerUp(SPStudioManager* manager) override;
	void powerOff() override;

	void submitGenkoDesign(SPNeMu* nemu) override;
	void creatGenko(SPWorkFlow* workFlow) override;

    void cleanUpDrawingPad(const Canvas& canvas) override;
    void design() override;


	

	void rigestArtist(SPVulkanArtist* artist);
	void init();
	SPVulkanArtist* getArtist();
	void draw();
	void clearDrawingBoard();
	void updateUniformBuffer(spuint currentImage);
protected:
	SINGLETON_PROTECTED_P(SPDrawingPadVulkan, SPDrawingPad);
	void createRenderPass();
	void createGraphicsPipeline();
	void prepareResource();
	void createCommandBuffers();
private:
	void createDescriptorSetLayout();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
	void createColorResources();
	void createDepthResources();
	void createFramebuffers();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();

	void createSwapChain(const Canvas& canvas);
	void clearSwapChain();
protected:
	SPNeMu* nemu;

	SPVulkanWindowsDriver* s_manager;
	SPVulkanArtist* s_artist;

	VkDescriptorSetLayout descriptorSetLayout;

	std::vector<VkSurfaceFormatKHR> surfaceFormatInfo;
	std::vector<VkPresentModeKHR> presentModeInfo;


	VkExtent2D s_actualExtent;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	std::vector<VkImageView> swapChainImageViews;

	VkRenderPass renderPass;

	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	//5----------------
	VkImage colorImage;
	VkDeviceMemory colorImageMemory;
	VkImageView colorImageView;
	//6----------------
	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;
	//7------------------
	std::vector<VkFramebuffer> swapChainFramebuffers;
	//8------------------
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	//9---------------------
	VkDescriptorPool descriptorPool;
	//10------------------
	std::vector<VkDescriptorSet> descriptorSets;

	//11---------------------------
	std::vector<VkCommandBuffer> commandBuffers;

};

#endif
