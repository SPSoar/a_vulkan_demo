#ifndef S_SPIMAGE_H_
#define S_SPIMAGE_H_

#include "../vulkan_common.h"

class SPDevice;
class SPImageView;
class SPImage
{
public:
	SPImage(SPDevice&	      device,
		VkImage               handle,
		const VkExtent3D&	  extent,
		VkFormat              format,
		VkImageUsageFlags     image_usage,
		VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT);

	SPImage(SPDevice&		  device,
		const VkExtent3D&	  extent,
		VkFormat              format,
		VkImageUsageFlags     image_usage,
		VmaMemoryUsage        memory_usage,
		VkSampleCountFlagBits sample_count = VK_SAMPLE_COUNT_1_BIT,
		uint32_t              mip_levels = 1,
		uint32_t              array_layers = 1,
		VkImageTiling         tiling = VK_IMAGE_TILING_OPTIMAL,
		VkImageCreateFlags    flags = 0,
		uint32_t              num_queue_families = 0,
		const uint32_t*		  queue_families = nullptr);

	SPImage(const SPImage&) = delete;

	SPImage(SPImage&& other);

	~SPImage();

	SPImage& operator=(const SPImage&) = delete;

	SPImage& operator=(SPImage&&) = delete;
public:
	SPDevice& get_device();
	VkImage get_handle() const;
	const VkExtent3D& get_extent() const;
	void unmap();
	VkFormat get_format() const;
	VkImageSubresource get_subresource() const;
	std::unordered_set<SPImageView*>& get_views();
	VkImageType get_type() const;
	VkSampleCountFlagBits get_sample_count() const;
	VkImageUsageFlags get_usage() const;
	uint32_t get_array_layer_count() const;
private:
	SPDevice& device_;
	VkImage handle_;
	VkExtent3D extent_;
	VkImageType type_;
	VkFormat format_;
	VkSampleCountFlagBits sample_count_;
	VkImageUsageFlags usage_;

	VkImageSubresource subresource_;
	uint32_t array_layer_count_;
	VkImageTiling tiling_;
	VmaAllocation memory_;

	uint8_t* mapped_data_;
	bool mapped_;
	std::unordered_set<SPImageView*> views_;
};

#endif