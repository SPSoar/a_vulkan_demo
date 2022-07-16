#include "SPBufferPool.h"

#include "../Driver/SPDevice.h"

SPBufferBlock::SPBufferBlock(SPDevice& device, VkDeviceSize size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage)
	: buffer_{ device, size, usage, memory_usage }
	, alignment_{ 0 }
{
	if (usage == VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
	{
		alignment_ = device.get_gpu().get_properties().limits.minUniformBufferOffsetAlignment;
	}
	else if (usage == VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
	{
		alignment_ = device.get_gpu().get_properties().limits.minStorageBufferOffsetAlignment;
	}
	else if (usage == VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT)
	{
		alignment_ = device.get_gpu().get_properties().limits.minTexelBufferOffsetAlignment;
	}
	else if (usage == VK_BUFFER_USAGE_INDEX_BUFFER_BIT || usage == VK_BUFFER_USAGE_VERTEX_BUFFER_BIT || usage == VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT)
	{
		// Used to calculate the offset, required when allocating memory (its value should be power of 2)
		alignment_ = 16;
	}
	else
	{
		throw std::runtime_error("Usage not recognised");
	}
}

VkDeviceSize SPBufferBlock::get_size() const
{
	return buffer_.get_size();
}

SPBufferPool::SPBufferPool(SPDevice& device, VkDeviceSize block_size, VkBufferUsageFlags usage, VmaMemoryUsage memory_usage)
	: device_{ device }
	, block_size_{ block_size }
	, usage_{ usage }
	, memory_usage_{ memory_usage }
	, active_buffer_block_count_{0}
{

}

SPBufferBlock& SPBufferPool::request_buffer_block(VkDeviceSize minimum_size)
{
	// Find the first block in the range of the inactive blocks
	// which can fit the minimum size
	auto it = std::upper_bound(buffer_blocks_.begin() + active_buffer_block_count_, buffer_blocks_.end(), minimum_size,
		[](const VkDeviceSize& a, const std::unique_ptr<SPBufferBlock>& b) -> bool { return a <= b->get_size(); });

	if (it != buffer_blocks_.end())
	{
		// Recycle inactive block
		active_buffer_block_count_++;
		return *it->get();
	}

	//LOGD("Building #{} buffer block ({})", buffer_blocks.size(), usage);

	// Create a new block, store and return it
	buffer_blocks_.emplace_back(std::make_unique<SPBufferBlock>(device_, std::max(block_size_, minimum_size), usage_, memory_usage_));

	auto& block = buffer_blocks_[active_buffer_block_count_++];

	return *block.get();
}
