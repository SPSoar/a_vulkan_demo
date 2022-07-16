#ifndef S_SPBUFFER_H_
#define S_SPBUFFER_H_

#include "../vulkan_common.h"

class SPDevice;

class SPBuffer
{
public:
	SPBuffer(SPDevice const&		     device,
		VkDeviceSize                 size,
		VkBufferUsageFlags           buffer_usage,
		VmaMemoryUsage               memory_usage,
		VmaAllocationCreateFlags     flags = VMA_ALLOCATION_CREATE_MAPPED_BIT,
		const std::vector<uint32_t>& queue_family_indices = {});

	SPBuffer(const SPBuffer&) = delete;
	SPBuffer(SPBuffer&& other) noexcept;
	~SPBuffer();
	SPBuffer& operator=(const SPBuffer&) = delete;
	SPBuffer& operator=(SPBuffer&&) = delete;
public:
	const VkBuffer& get_handle() const;
	VkDeviceSize get_size() const;
	void flush() const;
	uint8_t* map();
	void unmap();
	void update(const std::vector<uint8_t>& data, size_t offset = 0);
	void update(const uint8_t* data, size_t size, size_t offset = 0);
	void update(void* data, size_t size, size_t offset = 0);
	template <class T>
	void convert_and_update(const T& object, size_t offset = 0)
	{
		update(reinterpret_cast<const uint8_t*>(&object), sizeof(T), offset);
	}
	const VkBuffer* get() const;
protected:
	SPDevice const& device_;
	VkDeviceSize size_;
	bool persistent_;
	VkBuffer handle_;
	VmaAllocation allocation_;
	VkDeviceMemory memory_;
	uint8_t* mapped_data_;
	bool mapped_;
};

#endif