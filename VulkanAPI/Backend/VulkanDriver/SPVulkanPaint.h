#ifndef S_SPVULKANPAINT_H_
#define S_SPVULKANPAINT_H_
#include "vulkan_common.h"
#include "SPVulkanWindowsDriver.h"
class  SPVulkanPaint
{
public:
	SPVulkanPaint(SPVulkanWindowsDriver& driver);
	virtual ~SPVulkanPaint();
	void init(SPTexture& texture);
protected:

private:

	SPVulkanWindowsDriver& s_manager;
public:

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;

	VkImageView textureImageView;
	VkSampler textureSampler;


};	

#endif
