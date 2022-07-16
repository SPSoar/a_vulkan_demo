#include "SPRenderTarget.h"
#include "../Driver/SPDevice.h"


namespace
{
	struct CompareExtent2D
	{
		bool operator()(const VkExtent2D& lhs, const VkExtent2D& rhs) const
		{
			return !(lhs.width == rhs.width && lhs.height == rhs.height) && (lhs.width < rhs.width&& lhs.height < rhs.height);
		}
	};
}

const SPRenderTarget::CreateFunc SPRenderTarget::DEFAULT_CREATE_FUNC_ = [](SPImage&& swapchain_image) -> std::unique_ptr<SPRenderTarget> {
	VkFormat depth_format = vkTool::get_suitable_depth_format(swapchain_image.get_device().get_gpu().get_handle());

	SPImage depth_image{ swapchain_image.get_device(), swapchain_image.get_extent(),
							depth_format,
							VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT,
							VMA_MEMORY_USAGE_GPU_ONLY };

	std::vector<SPImage> images;
	images.push_back(std::move(swapchain_image));
	images.push_back(std::move(depth_image));

	return std::make_unique<SPRenderTarget>(std::move(images));
};

SPAttachment::SPAttachment()
	: format_{ VK_FORMAT_UNDEFINED }
	, samples_{ VK_SAMPLE_COUNT_1_BIT }
	, usage_{ VK_IMAGE_USAGE_SAMPLED_BIT }
	, initial_layout_{ VK_IMAGE_LAYOUT_UNDEFINED }

{
}

SPAttachment::SPAttachment(VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage)
	: format_{ format }
	, samples_{ samples }
	, usage_{ usage }
	, initial_layout_{ VK_IMAGE_LAYOUT_UNDEFINED }

{
}

SPRenderTarget::SPRenderTarget(std::vector<SPImage>&& images)
	: device_{ images.back().get_device() }
	, images_{ std::move(images) }
	, extent_{}
	, views_ {}
	, attachments_{}
{
	assert(!this->images_.empty() && "Should specify at least 1 image");

	std::set<VkExtent2D, CompareExtent2D> unique_extent;

	// Returns the image extent as a VkExtent2D structure from a VkExtent3D
	auto get_image_extent = [](const SPImage& image) { return VkExtent2D{ image.get_extent().width, image.get_extent().height }; };

	// Constructs a set of unique image extens given a vector of images
	std::transform(this->images_.begin(), this->images_.end(), std::inserter(unique_extent, unique_extent.end()), get_image_extent);

	// Allow only one extent size for a render target
	if (unique_extent.size() != 1)
	{
		throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED, "Extent size is not unique" };
	}

	extent_ = *unique_extent.begin();

	for (auto& image : this->images_)
	{
		if (image.get_type() != VK_IMAGE_TYPE_2D)
		{
			throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED, "Image type is not 2D" };
		}

		views_.emplace_back(image, VK_IMAGE_VIEW_TYPE_2D);

		attachments_.emplace_back(SPAttachment{ image.get_format(), image.get_sample_count(), image.get_usage() });
	}
}

SPRenderTarget::SPRenderTarget(std::vector<SPImageView>&& image_views)
	: device_{ const_cast<SPImage&>(image_views.back().get_image()).get_device() }
	, images_{}
	, views_ { std::move(image_views) }
	, extent_{}
	, attachments_{}
{
	assert(!views_.empty() && "Should specify at least 1 image view");

	std::set<VkExtent2D, CompareExtent2D> unique_extent;

	// Returns the extent of the base mip level pointed at by a view
	auto get_view_extent = [](const SPImageView& view) {
		const VkExtent3D mip0_extent = view.get_image().get_extent();
		const uint32_t   mip_level = view.get_subresource_range().baseMipLevel;
		return VkExtent2D{ mip0_extent.width >> mip_level, mip0_extent.height >> mip_level };
	};

	// Constructs a set of unique image extents given a vector of image views;
	// allow only one extent size for a render target
	std::transform(views_.begin(), views_.end(), std::inserter(unique_extent, unique_extent.end()), get_view_extent);
	if (unique_extent.size() != 1)
	{
		throw VulkanException{ VK_ERROR_INITIALIZATION_FAILED, "Extent size is not unique" };
	}
	extent_ = *unique_extent.begin();

	for (auto& view : views_)
	{
		const auto& image = view.get_image();
		attachments_.emplace_back(SPAttachment{ image.get_format(), image.get_sample_count(), image.get_usage() });
	}
}

const std::vector<SPImageView>& SPRenderTarget::get_views() const
{
	return views_;
}

const VkExtent2D& SPRenderTarget::get_extent() const
{
	return extent_;
}
