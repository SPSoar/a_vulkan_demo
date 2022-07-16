#ifndef S_SPRENDERTARGET_H_
#define S_SPRENDERTARGET_H_

#include "../vulkan_common.h"
#include "SPImageView.h"
#include "SPImage.h"

struct SPAttachment
{
	VkFormat format_{ VK_FORMAT_UNDEFINED };

	VkSampleCountFlagBits samples_{ VK_SAMPLE_COUNT_1_BIT };

	VkImageUsageFlags usage_{ VK_IMAGE_USAGE_SAMPLED_BIT };

	VkImageLayout initial_layout_{  };

	SPAttachment();

	SPAttachment(VkFormat format, VkSampleCountFlagBits samples, VkImageUsageFlags usage);
};


class SPDevice;
class SPRenderTarget
{
public:
	using CreateFunc = std::function<std::unique_ptr<SPRenderTarget>(SPImage&&)>;

	static const CreateFunc DEFAULT_CREATE_FUNC_;

	SPRenderTarget(std::vector<SPImage>&& images);
	SPRenderTarget(std::vector<SPImageView>&& image_views);

	SPRenderTarget(const SPRenderTarget&) = delete;
	SPRenderTarget(SPRenderTarget&&) = delete;
	SPRenderTarget& operator=(const SPRenderTarget& other) noexcept = delete;
	SPRenderTarget& operator=(SPRenderTarget&& other) noexcept = delete;
public:
	const std::vector<SPImageView>& get_views() const;
	const VkExtent2D& get_extent() const;
protected:
	SPDevice& device_;
	std::vector<SPImage> images_;
	VkExtent2D extent_;
	std::vector<SPImageView> views_;
	std::vector<SPAttachment> attachments_;
};

#endif