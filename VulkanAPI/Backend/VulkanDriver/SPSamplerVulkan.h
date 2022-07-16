#ifndef S_SPSAMPLERVULKAN_H_
#define S_SPSAMPLERVULKAN_H_
#include "vulkan_common.h"
#include "../../Interface/SPBuffer.h"
class  SPSamplerVulkan : public SPBuffer
{
public:
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;

	VkImageView textureImageView;
	VkSampler textureSampler;
};

#endif
