#ifndef S_SPSWAPCHAIN_H_
#define S_SPSWAPCHAIN_H_

#include "../vulkan_common.h"


enum class ImageFormat : uint8_t
{
	sRGB = 0,
	UNORM
};

struct SwapchainProperties
{
	VkSwapchainKHR                old_swapchain_;
	uint32_t                      image_count_{ 3 };
	VkExtent2D                    extent_{};
	VkSurfaceFormatKHR            surface_format_{};
	uint32_t                      array_layers_;
	VkImageUsageFlags             image_usage_;
	VkSurfaceTransformFlagBitsKHR pre_transform_;
	VkCompositeAlphaFlagBitsKHR   composite_alpha_;
	VkPresentModeKHR              present_mode_;
};


class SPDevice;
class SPSwapchain
{
public:

	SPSwapchain(SPSwapchain& old_swapchain, const VkExtent2D& extent);
	SPSwapchain(SPSwapchain& old_swapchain, const uint32_t image_count);
	SPSwapchain(SPSwapchain& old_swapchain, const std::set<VkImageUsageFlagBits>& image_usage_flags);
	SPSwapchain(SPSwapchain& swapchain, const VkExtent2D& extent, const VkSurfaceTransformFlagBitsKHR transform);
	SPSwapchain(SPDevice& device,
		VkSurfaceKHR                          surface,
		const VkExtent2D& extent = {},
		const uint32_t                        image_count = 3,
		const VkSurfaceTransformFlagBitsKHR   transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		const VkPresentModeKHR                present_mode = VK_PRESENT_MODE_FIFO_KHR,
		const std::set<VkImageUsageFlagBits>& image_usage_flags = { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT });
	SPSwapchain(SPSwapchain& old_swapchain,
		SPDevice& device,
		VkSurfaceKHR                          surface,
		const VkExtent2D& extent = {},
		const uint32_t                        image_count = 3,
		const VkSurfaceTransformFlagBitsKHR   transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
		const VkPresentModeKHR                present_mode = VK_PRESENT_MODE_FIFO_KHR,
		const std::set<VkImageUsageFlagBits>& image_usage_flags = { VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_IMAGE_USAGE_TRANSFER_SRC_BIT });

	SPSwapchain(const SPSwapchain&) = delete;
	SPSwapchain(SPSwapchain&& other);
	~SPSwapchain();
	SPSwapchain& operator=(const SPSwapchain&) = delete;
	SPSwapchain& operator=(SPSwapchain&&) = delete;
public:
	SwapchainProperties& get_properties();
public:
	VkSwapchainKHR get_handle() const;
	void create();
	void set_present_mode_priority(const std::vector<VkPresentModeKHR>& present_mode_priority_list);
	void set_surface_format_priority(const std::vector<VkSurfaceFormatKHR>& surface_format_priority_list);
	const VkExtent2D& get_extent() const;
	const std::vector<VkImage>& get_images() const;
	VkFormat get_format() const;
	VkImageUsageFlags get_usage() const;
	VkSurfaceKHR get_surface() const;
	VkResult acquire_next_image(uint32_t& image_index, VkSemaphore image_acquired_semaphore, VkFence fence = VK_NULL_HANDLE) const;

private:
	SPDevice& device_;
	VkSwapchainKHR handle_;
	VkSurfaceKHR surface_;
	std::vector<VkPresentModeKHR> present_mode_priority_list_;
	std::vector<VkSurfaceFormatKHR> surface_format_priority_list_;
	std::vector<VkSurfaceFormatKHR> surface_formats_;
	std::vector<VkPresentModeKHR> present_modes_;
	SwapchainProperties properties_;
	std::set<VkImageUsageFlagBits> image_usage_flags_;
	std::vector<VkImage> images_;
};

#endif