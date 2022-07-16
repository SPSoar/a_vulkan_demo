#ifndef S_SPIMAGEVIEW_H_
#define S_SPIMAGEVIEW_H_

#include "../vulkan_common.h"

class SPImage;
class SPDevice;
class SPImageView
{
public:
	SPImageView(SPImage& image, VkImageViewType view_type, VkFormat format = VK_FORMAT_UNDEFINED,
		uint32_t base_mip_level = 0, uint32_t base_array_layer = 0,
		uint32_t n_mip_levels = 0, uint32_t n_array_layers = 0);

	SPImageView(SPImageView&) = delete;

	SPImageView(SPImageView&& other);

	~SPImageView();

	SPImageView& operator=(const SPImageView&) = delete;

	SPImageView& operator=(SPImageView&&) = delete;
public:
	const SPImage& get_image() const;
	void set_image(SPImage& image);
	VkImageView get_handle() const;
	VkImageSubresourceRange get_subresource_range() const;
	VkImageSubresourceLayers get_subresource_layers() const;
	VkFormat get_format() const;
protected:
	SPDevice& device_;
	//需要后期分析这个为什么不用引用
	SPImage* image_;
	VkFormat format_;
	VkImageSubresourceRange subresource_range_;
	VkImageView handle_;
};

#endif