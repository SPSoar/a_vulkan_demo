#ifndef S_SPSHADERVULKAN_H_
#define S_SPSHADERVULKAN_H_
#include "vulkan_common.h"
#include "../../Interface/SPBuffer.h"
class  SPShaderVulkan : public SPBuffer
{
public:
	VkShaderModule shaderModule;
};

#endif
