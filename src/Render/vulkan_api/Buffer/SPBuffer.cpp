#include "SPBuffer.h"
#include "../Driver/SPDevice.h"

SPBuffer::SPBuffer(SPDevice const& device, VkDeviceSize size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage memory_usage, VmaAllocationCreateFlags flags, const std::vector<uint32_t>& queue_family_indices)
	: device_{ device }
	, size_{ size }
	, persistent_{false}
	, allocation_{ VK_NULL_HANDLE }
	, memory_{ VK_NULL_HANDLE }
	, mapped_data_{ nullptr }
	, mapped_{ false }
	, handle_{ VK_NULL_HANDLE }
{

	persistent_ = (flags & VMA_ALLOCATION_CREATE_MAPPED_BIT) != 0;

	VkBufferCreateInfo buffer_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
	buffer_info.usage = buffer_usage;
	buffer_info.size = size;
	if (queue_family_indices.size() >= 2)
	{
		buffer_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
		buffer_info.queueFamilyIndexCount = static_cast<uint32_t>(queue_family_indices.size());
		buffer_info.pQueueFamilyIndices = queue_family_indices.data();
	}

	VmaAllocationCreateInfo memory_info{};
	memory_info.flags = flags;
	memory_info.usage = memory_usage;

	VmaAllocationInfo allocation_info{};
	auto result = vmaCreateBuffer(device.get_memory_allocator(),
		&buffer_info, &memory_info, &handle_, &allocation_, &allocation_info);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create Buffer" };
	}

	memory_ = allocation_info.deviceMemory;

	if (persistent_)
	{
		mapped_data_ = static_cast<uint8_t*>(allocation_info.pMappedData);
	}
}

SPBuffer::SPBuffer(SPBuffer&& other) noexcept
	: handle_{ other.handle_ }
	, device_{ other.device_ }
	, allocation_{ other.allocation_ }
	, memory_{ other.memory_ }
	, size_{ other.size_ }
	, mapped_data_{ other.mapped_data_ }
	, mapped_{ other.mapped_ }
	, persistent_{ other.persistent_}
{
	other.handle_ = VK_NULL_HANDLE;
	other.allocation_ = VK_NULL_HANDLE;
	other.memory_ = VK_NULL_HANDLE;
	other.mapped_data_ = nullptr;
	other.mapped_ = false;
}

SPBuffer::~SPBuffer()
{
	if (handle_ != VK_NULL_HANDLE && allocation_ != VK_NULL_HANDLE)
	{
		unmap();
		vmaDestroyBuffer(device_.get_memory_allocator(), handle_, allocation_);
	}
}

const VkBuffer& SPBuffer::get_handle() const
{
	return handle_;
}

VkDeviceSize SPBuffer::get_size() const
{
	return size_;
}

void SPBuffer::flush() const
{
	vmaFlushAllocation(device_.get_memory_allocator(), allocation_, 0, size_);
}

uint8_t* SPBuffer::map()
{
	if (!mapped_ && !mapped_data_)
	{
		VK_CHECK(vmaMapMemory(device_.get_memory_allocator(), allocation_, reinterpret_cast<void**>(&mapped_data_)));
		mapped_ = true;
	}
	return mapped_data_;
}

void SPBuffer::unmap()
{
	if (mapped_)
	{
		vmaUnmapMemory(device_.get_memory_allocator(), allocation_);
		mapped_data_ = nullptr;
		mapped_ = false;
	}
}

void SPBuffer::update(const std::vector<uint8_t>& data, size_t offset)
{
	update(data.data(), data.size(), offset);
}

void SPBuffer::update(const uint8_t* data, size_t size, size_t offset)
{
	if (persistent_)
	{
		std::copy(data, data + size, mapped_data_ + offset);
		flush();
	}
	else
	{
		map();
		std::copy(data, data + size, mapped_data_ + offset);
		flush();
		unmap();
	}
}

void SPBuffer::update(void* data, size_t size, size_t offset)
{
	update(reinterpret_cast<const uint8_t*>(data), size, offset);
}

const VkBuffer* SPBuffer::get() const
{
	return &handle_;
}
