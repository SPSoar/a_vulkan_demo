#ifndef S_SPBUFFERVULKAN_H_
#define S_SPBUFFERVULKAN_H_
#include "vulkan_common.h"
#include "../../Interface/SPBuffer.h"
class  SPBufferVulkan : public SPBuffer
{
public:
	VkBuffer buffer;
	VkDeviceMemory bufferMemory;
	spint elementCount;
	spint bufferSize;
};

#endif
