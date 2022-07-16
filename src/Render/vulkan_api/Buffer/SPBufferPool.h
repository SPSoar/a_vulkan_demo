#ifndef S_SPBUFFERPOOL_H_
#define S_SPBUFFERPOOL_H_

#include "../vulkan_common.h"

#include "SPBuffer.h"

class SPDevice;
class SPBufferBlock
{
public:
	SPBufferBlock(SPDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage);
	VkDeviceSize get_size() const;
private:
	SPBuffer buffer_;
	VkDeviceSize alignment_;
};

class SPBufferPool
{
public:
	SPBufferPool(SPDevice& device, VkDeviceSize block_size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage = VMA_MEMORY_USAGE_CPU_TO_GPU);
	SPBufferBlock& request_buffer_block(VkDeviceSize minimum_size);
private:
	SPDevice& device_;
	VkDeviceSize block_size_;
	VkBufferUsageFlags usage_;
	VmaMemoryUsage memory_usage_;

	std::vector<std::unique_ptr<SPBufferBlock>> buffer_blocks_;
	uint32_t active_buffer_block_count_;
};

#endif