#include "SPImageView.h"
#include "SPImage.h"
#include "../Driver/SPDevice.h"

//这种ImageView和Image的关系耦合比较严重，掌握vulkan架构后，尝试进行调整
SPImageView::SPImageView(SPImage& image, VkImageViewType view_type, VkFormat format, uint32_t mip_level, uint32_t array_layer, uint32_t n_mip_levels, uint32_t n_array_layers)
	: device_{ image.get_device() }
	, image_{ &image }
	, format_ { format }
	, handle_{ VK_NULL_HANDLE }
{
	if (format == VK_FORMAT_UNDEFINED)
	{
		this->format_ = format = image_->get_format();
	}

	subresource_range_.baseMipLevel = mip_level;
	subresource_range_.baseArrayLayer = array_layer;
	subresource_range_.levelCount = n_mip_levels == 0 ? image_->get_subresource().mipLevel : n_mip_levels;
	subresource_range_.layerCount = n_array_layers == 0 ? image_->get_subresource().arrayLayer : n_array_layers;

	if (vkTool::is_depth_stencil_format(format))
	{
		subresource_range_.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
	}
	else
	{
		subresource_range_.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	VkImageViewCreateInfo view_info{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
	view_info.image = image_->get_handle();
	view_info.viewType = view_type;
	view_info.format = format;
	view_info.subresourceRange = subresource_range_;

	auto result = vkCreateImageView(device_.get_handle(), &view_info, nullptr, &handle_);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create ImageView" };
	}

	image_->get_views().emplace(this);
}

SPImageView::SPImageView(SPImageView&& other)
	: device_{ other.device_ }
	, image_{ other.image_ }
	, handle_{ other.handle_ }
	, format_{ other.format_ }
	, subresource_range_{ other.subresource_range_ }
{	
	auto& views = image_->get_views();
	views.erase(&other);
	views.emplace(this);

	other.handle_ = VK_NULL_HANDLE;
}

SPImageView::~SPImageView()
{
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyImageView(device_.get_handle(), handle_, nullptr);
	}
}

const SPImage& SPImageView::get_image() const
{
	assert(image_ && "Image view is referring an invalid image");
	return *image_;
}

void SPImageView::set_image(SPImage& image)
{
	image_ = &image;
}

VkImageView SPImageView::get_handle() const
{
	return handle_;
}

VkImageSubresourceRange SPImageView::get_subresource_range() const
{
	return subresource_range_;
}

VkImageSubresourceLayers SPImageView::get_subresource_layers() const
{
	VkImageSubresourceLayers subresource{};
	subresource.aspectMask = subresource_range_.aspectMask;
	subresource.baseArrayLayer = subresource_range_.baseArrayLayer;
	subresource.layerCount = subresource_range_.layerCount;
	subresource.mipLevel = subresource_range_.baseMipLevel;
	return subresource;
}

VkFormat SPImageView::get_format() const
{

	return format_;
}
