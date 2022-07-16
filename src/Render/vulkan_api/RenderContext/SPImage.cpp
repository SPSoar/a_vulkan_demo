#include "SPImage.h"
#include "../Driver/SPDevice.h"
#include "SPImageView.h"

inline VkImageType find_image_type(VkExtent3D extent)
{
	VkImageType result{};

	uint32_t dim_num{ 0 };

	if (extent.width >= 1)
	{
		dim_num++;
	}

	if (extent.height >= 1)
	{
		dim_num++;
	}

	if (extent.depth > 1)
	{
		dim_num++;
	}

	switch (dim_num)
	{
	case 1:
		result = VK_IMAGE_TYPE_1D;
		break;
	case 2:
		result = VK_IMAGE_TYPE_2D;
		break;
	case 3:
		result = VK_IMAGE_TYPE_3D;
		break;
	default:
		throw std::runtime_error("No image type found.");
		break;
	}

	return result;
}

SPImage::SPImage(SPDevice& device, VkImage handle, const VkExtent3D& extent,
	VkFormat format, VkImageUsageFlags image_usage, VkSampleCountFlagBits sample_count)
	: device_{ device }
    , handle_{ handle }
    , type_{ find_image_type(extent) }
    , extent_{ extent }
    , format_{ format }
    , sample_count_{ sample_count }
    , usage_{ image_usage }
	, array_layer_count_{0}
	, tiling_{}
	, memory_{ VK_NULL_HANDLE }
	, mapped_data_{ nullptr }
	, mapped_ {false}
	, views_{}
{	
	subresource_.mipLevel = 1;
	subresource_.arrayLayer = 1;
}

SPImage::SPImage(
				SPDevice&			  device, 
				const VkExtent3D&	  extent, 
				VkFormat			  format,
				VkImageUsageFlags	  image_usage, 
				VmaMemoryUsage		  memory_usage, 
				VkSampleCountFlagBits sample_count,
				uint32_t			  mip_levels,
				uint32_t			  array_layers, 
				VkImageTiling		  tiling, 
				VkImageCreateFlags	  flags, 
				uint32_t			  num_queue_families, 
				const uint32_t*		  queue_families)
	: device_{ device }
	, type_{ find_image_type(extent) }
	, extent_{ extent }
	, format_{ format }
	, sample_count_{ sample_count }
	, usage_{ image_usage }
	, array_layer_count_{ array_layers }
	, tiling_{ tiling }
	, mapped_data_{ nullptr }
	, mapped_{ false }
	, views_{}
{
	assert(mip_levels > 0 && "Image should have at least one level");
	assert(array_layers > 0 && "Image should have at least one layer");

	subresource_.mipLevel = mip_levels;
	subresource_.arrayLayer = array_layers;

	VkImageCreateInfo image_info{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
	image_info.flags = flags;
	image_info.imageType = type_;
	image_info.format = format;
	image_info.extent = extent;
	image_info.mipLevels = mip_levels;
	image_info.arrayLayers = array_layers;
	image_info.samples = sample_count;
	image_info.tiling = tiling;
	image_info.usage = image_usage;

	if (num_queue_families != 0)
	{
		image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
		image_info.queueFamilyIndexCount = num_queue_families;
		image_info.pQueueFamilyIndices = queue_families;
	}

	VmaAllocationCreateInfo memory_info{};
	memory_info.usage = memory_usage;

	if (image_usage & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT)
	{
		memory_info.preferredFlags = VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT;
	}

	auto result = vmaCreateImage(device_.get_memory_allocator(),
		&image_info, &memory_info,
		&handle_, &memory_,
		nullptr);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create Image" };
	}
}

SPImage::SPImage(SPImage&& other)
	: device_{ other.device_ }
	, handle_{ other.handle_ }
	, memory_{ other.memory_ }
	, type_{ other.type_ }
	, extent_{ other.extent_ }
	, format_{ other.format_ }
	, sample_count_{ other.sample_count_ }
	, usage_{ other.usage_ }
	, tiling_{ other.tiling_ }
	, subresource_{ other.subresource_ }
	, mapped_data_{ other.mapped_data_ }
	, mapped_{ other.mapped_ }
{
	other.handle_ = VK_NULL_HANDLE;
	other.memory_ = VK_NULL_HANDLE;
	other.mapped_data_ = nullptr;
	other.mapped_ = false;

	// Update image views references to this image to avoid dangling pointers
	for (auto& view : views_)
	{
		view->set_image(*this);
	}
}

SPImage::~SPImage()
{
	if (handle_ != VK_NULL_HANDLE && memory_ != VK_NULL_HANDLE)
	{
		unmap();
		vmaDestroyImage(device_.get_memory_allocator(), handle_, memory_);
	}
}

SPDevice& SPImage::get_device()
{
	return device_;
}

VkImage SPImage::get_handle() const
{
	return handle_;
}

const VkExtent3D& SPImage::get_extent() const
{
	return extent_;
}

void SPImage::unmap()
{
	if (mapped_)
	{
		vmaUnmapMemory(device_.get_memory_allocator(), memory_);
		mapped_data_ = nullptr;
		mapped_ = false;
	}
}

VkFormat SPImage::get_format() const
{
	return format_;
}

VkImageSubresource SPImage::get_subresource() const
{
	return subresource_;
}

std::unordered_set<SPImageView*>& SPImage::get_views()
{
	return views_;
}

VkImageType SPImage::get_type() const
{
	return type_;
}

VkSampleCountFlagBits SPImage::get_sample_count() const
{
	return sample_count_;
}

VkImageUsageFlags SPImage::get_usage() const
{
	return usage_;
}

uint32_t SPImage::get_array_layer_count() const
{
	return array_layer_count_;
}
