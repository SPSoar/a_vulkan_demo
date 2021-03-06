#include "SPSwapchain.h"
#include "../Driver/SPDevice.h"


inline uint32_t choose_image_count(uint32_t request_image_count, uint32_t min_image_count, uint32_t max_image_count)
{
	if (max_image_count != 0)
	{
		request_image_count = std::min(request_image_count, max_image_count);
	}

	request_image_count = std::max(request_image_count, min_image_count);

	return request_image_count;
}

inline uint32_t choose_image_array_layers( uint32_t request_image_array_layers, uint32_t max_image_array_layers)
{
	request_image_array_layers = std::min(request_image_array_layers, max_image_array_layers);
	request_image_array_layers = std::max(request_image_array_layers, 1u);

	return request_image_array_layers;
}

inline VkExtent2D choose_extent( VkExtent2D request_extent, const VkExtent2D& min_image_extent,
	const VkExtent2D& max_image_extent, const VkExtent2D& current_extent)
{
	if (request_extent.width < 1 || request_extent.height < 1)
	{
		SPWarning("(Swapchain) Image extent ("<< request_extent.width << ", " << request_extent.height <<"\
			) not supported. Selecting ("<< current_extent.width <<","<< current_extent.height <<").");
		return current_extent;
	}

	request_extent.width = std::max(request_extent.width, min_image_extent.width);
	request_extent.width = std::min(request_extent.width, max_image_extent.width);

	request_extent.height = std::max(request_extent.height, min_image_extent.height);
	request_extent.height = std::min(request_extent.height, max_image_extent.height);

	return request_extent;
}

inline VkPresentModeKHR choose_present_mode( VkPresentModeKHR request_present_mode,
	const std::vector<VkPresentModeKHR>& available_present_modes,
	const std::vector<VkPresentModeKHR>& present_mode_priority_list)
{
	auto present_mode_it = std::find(available_present_modes.begin(), available_present_modes.end(), request_present_mode);

	if (present_mode_it == available_present_modes.end())
	{
		// If nothing found, always default to FIFO
		VkPresentModeKHR chosen_present_mode = VK_PRESENT_MODE_FIFO_KHR;

		for (auto& present_mode : present_mode_priority_list)
		{
			if (std::find(available_present_modes.begin(), available_present_modes.end(), present_mode) != available_present_modes.end())
			{
				chosen_present_mode = present_mode;
			}
		}

		//SPWarning("(Swapchain) Present mode '{}' not supported. Selecting '{}'.", to_string(request_present_mode), to_string(chosen_present_mode));
		return chosen_present_mode;
	}
	else
	{
		//SPLog("(Swapchain) Present mode selected: {}", to_string(request_present_mode));
		return *present_mode_it;
	}
}

inline VkSurfaceFormatKHR choose_surface_format(
	const VkSurfaceFormatKHR               requested_surface_format,
	const std::vector<VkSurfaceFormatKHR>& available_surface_formats,
	const std::vector<VkSurfaceFormatKHR>& surface_format_priority_list)
{
	// Try to find the requested surface format in the supported surface formats
	auto surface_format_it = std::find_if(
		available_surface_formats.begin(),
		available_surface_formats.end(),
		[&requested_surface_format](const VkSurfaceFormatKHR& surface) {
			if (surface.format == requested_surface_format.format &&
				surface.colorSpace == requested_surface_format.colorSpace)
			{
				return true;
			}

			return false;
		});

	// If the requested surface format isn't found, then try to request a format from the priority list
	if (surface_format_it == available_surface_formats.end())
	{
		for (auto& surface_format : surface_format_priority_list)
		{
			surface_format_it = std::find_if(
				available_surface_formats.begin(),
				available_surface_formats.end(),
				[&surface_format](const VkSurfaceFormatKHR& surface) {
					if (surface.format == surface_format.format &&
						surface.colorSpace == surface_format.colorSpace)
					{
						return true;
					}

					return false;
				});
			if (surface_format_it != available_surface_formats.end())
			{
				//LOGW("(Swapchain) Surface format ({}) not supported. Selecting ({}).", to_string(requested_surface_format), to_string(*surface_format_it));
				return *surface_format_it;
			}
		}

		// If nothing found, default the first supporte surface format
		surface_format_it = available_surface_formats.begin();
		//LOGW("(Swapchain) Surface format ({}) not supported. Selecting ({}).", to_string(requested_surface_format), to_string(*surface_format_it));
	}
	else
	{
		//LOGI("(Swapchain) Surface format selected: {}", to_string(requested_surface_format));
	}

	return *surface_format_it;
}

inline VkSurfaceTransformFlagBitsKHR choose_transform(
	VkSurfaceTransformFlagBitsKHR request_transform,
	VkSurfaceTransformFlagsKHR    supported_transform,
	VkSurfaceTransformFlagBitsKHR current_transform)
{
	if (request_transform & supported_transform)
	{
		return request_transform;
	}

	//LOGW("(Swapchain) Surface transform '{}' not supported. Selecting '{}'.", to_string(request_transform), to_string(current_transform));

	return current_transform;
}

inline VkCompositeAlphaFlagBitsKHR choose_composite_alpha(VkCompositeAlphaFlagBitsKHR request_composite_alpha, VkCompositeAlphaFlagsKHR supported_composite_alpha)
{
	if (request_composite_alpha & supported_composite_alpha)
	{
		return request_composite_alpha;
	}

	static const std::vector<VkCompositeAlphaFlagBitsKHR> composite_alpha_flags = {
		VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
		VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
		VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR };

	for (VkCompositeAlphaFlagBitsKHR composite_alpha : composite_alpha_flags)
	{
		if (composite_alpha & supported_composite_alpha)
		{
			//LOGW("(Swapchain) Composite alpha '{}' not supported. Selecting '{}.", to_string(request_composite_alpha), to_string(composite_alpha));
			return composite_alpha;
		}
	}

	throw std::runtime_error("No compatible composite alpha found.");
}

inline bool validate_format_feature(VkImageUsageFlagBits image_usage, VkFormatFeatureFlags supported_features)
{
	switch (image_usage)
	{
	case VK_IMAGE_USAGE_STORAGE_BIT:
		return VK_FORMAT_FEATURE_STORAGE_IMAGE_BIT & supported_features;
	default:
		return true;
	}
}

inline std::set<VkImageUsageFlagBits> choose_image_usage(const std::set<VkImageUsageFlagBits>& requested_image_usage_flags, VkImageUsageFlags supported_image_usage, VkFormatFeatureFlags supported_features)
{
	std::set<VkImageUsageFlagBits> validated_image_usage_flags;
	for (auto flag : requested_image_usage_flags)
	{
		if ((flag & supported_image_usage) && validate_format_feature(flag, supported_features))
		{
			validated_image_usage_flags.insert(flag);
		}
		else
		{
			//LOGW("(Swapchain) Image usage ({}) requested but not supported.", to_string(flag));
		}
	}

	if (validated_image_usage_flags.empty())
	{
		// Pick the first format from list of defaults, if supported
		static const std::vector<VkImageUsageFlagBits> image_usage_flags = {
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			VK_IMAGE_USAGE_STORAGE_BIT,
			VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_USAGE_TRANSFER_DST_BIT };

		for (VkImageUsageFlagBits image_usage : image_usage_flags)
		{
			if ((image_usage & supported_image_usage) && validate_format_feature(image_usage, supported_features))
			{
				validated_image_usage_flags.insert(image_usage);
				break;
			}
		}
	}

	if (!validated_image_usage_flags.empty())
	{
		// Log image usage flags used
		/*std::string usage_list;
		for (VkImageUsageFlagBits image_usage : validated_image_usage_flags)
		{
			usage_list += to_string(image_usage) + " ";
		}
		LOGI("(Swapchain) Image usage flags: {}", usage_list);*/
	}
	else
	{
		throw std::runtime_error("No compatible image usage found.");
	}

	return validated_image_usage_flags;
}

inline VkImageUsageFlags composite_image_flags(std::set<VkImageUsageFlagBits>& image_usage_flags)
{
	VkImageUsageFlags image_usage{};
	for (auto flag : image_usage_flags)
	{
		image_usage |= flag;
	}
	return image_usage;
}



SPSwapchain::SPSwapchain(SPSwapchain& old_swapchain, const VkExtent2D& extent)
	: SPSwapchain { 
		old_swapchain, old_swapchain.device_, 
		old_swapchain.surface_,
		extent, 
		old_swapchain.properties_.image_count_,
		old_swapchain.properties_.pre_transform_,
		old_swapchain.properties_.present_mode_,
		old_swapchain.image_usage_flags_
	}

{
	present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
	surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;
	create();
}

SPSwapchain::SPSwapchain(SPSwapchain& old_swapchain, const uint32_t image_count)
	: SPSwapchain { 
		old_swapchain, 
		old_swapchain.device_,
		old_swapchain.surface_,
		old_swapchain.properties_.extent_,
		image_count, old_swapchain.properties_.pre_transform_,
		old_swapchain.properties_.present_mode_,
		old_swapchain.image_usage_flags_
	}
{
	present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
	surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;
	create();
}

SPSwapchain::SPSwapchain(SPSwapchain& old_swapchain, const std::set<VkImageUsageFlagBits>& image_usage_flags)
	: SPSwapchain { 
		old_swapchain, 
		old_swapchain.device_,
		old_swapchain.surface_,
		old_swapchain.properties_.extent_,
		old_swapchain.properties_.image_count_,
		old_swapchain.properties_.pre_transform_,
		old_swapchain.properties_.present_mode_,
		image_usage_flags 
	}

{
	present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
	surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;
	create();
}

SPSwapchain::SPSwapchain(SPSwapchain& old_swapchain, const VkExtent2D& extent, const VkSurfaceTransformFlagBitsKHR transform)
	: SPSwapchain { 
		old_swapchain, 
		old_swapchain.device_,
		old_swapchain.surface_,
		extent, 
		old_swapchain.properties_.image_count_,
		transform, old_swapchain.properties_.present_mode_,
		old_swapchain.image_usage_flags_
	}
{
	present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
	surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;
	create();
}

SPSwapchain::SPSwapchain(SPDevice& device, VkSurfaceKHR surface, const VkExtent2D& extent, const uint32_t image_count, const VkSurfaceTransformFlagBitsKHR transform, const VkPresentModeKHR present_mode, const std::set<VkImageUsageFlagBits>& image_usage_flags)
	: SPSwapchain{ *this, device, surface, extent, image_count, transform, present_mode, image_usage_flags }
{

}

SPSwapchain::SPSwapchain(SPSwapchain& old_swapchain, SPDevice& device, VkSurfaceKHR surface, const VkExtent2D& extent, const uint32_t image_count, const VkSurfaceTransformFlagBitsKHR transform, const VkPresentModeKHR present_mode, const std::set<VkImageUsageFlagBits>& image_usage_flags)
	: device_{ device }
	, surface_{ surface }
	, present_mode_priority_list_{
		 VK_PRESENT_MODE_FIFO_KHR,
		 VK_PRESENT_MODE_MAILBOX_KHR
	}
	, surface_format_priority_list_{
		{VK_FORMAT_R8G8B8A8_SRGB,  VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		{VK_FORMAT_B8G8R8A8_SRGB,  VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		{VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
		{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
	}
{
	present_mode_priority_list_ = old_swapchain.present_mode_priority_list_;
	surface_format_priority_list_ = old_swapchain.surface_format_priority_list_;

	VkSurfaceCapabilitiesKHR surface_capabilities{};
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(this->device_.get_gpu().get_handle(), surface, &surface_capabilities);

	uint32_t surface_format_count{ 0U };
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(this->device_.get_gpu().get_handle(), surface, &surface_format_count, nullptr));
	surface_formats_.resize(surface_format_count);
	VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(this->device_.get_gpu().get_handle(), surface, &surface_format_count, surface_formats_.data()));

	//LOGI("Surface supports the following surface formats:");
	//for (auto& surface_format : surface_formats)
	//{
	//	LOGI("  \t{}", to_string(surface_format));
	//}

	uint32_t present_mode_count{ 0U };
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(this->device_.get_gpu().get_handle(), surface, &present_mode_count, nullptr));
	present_modes_.resize(present_mode_count);
	VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(this->device_.get_gpu().get_handle(), surface, &present_mode_count, present_modes_.data()));

	//LOGI("Surface supports the following present modes:");
	//for (auto& present_mode : present_modes)
	//{
	//	LOGI("  \t{}", to_string(present_mode));
	//}

	// Chose best properties based on surface capabilities
	properties_.image_count_ = choose_image_count(image_count, surface_capabilities.minImageCount, surface_capabilities.maxImageCount);
	properties_.extent_ = choose_extent(extent, surface_capabilities.minImageExtent, surface_capabilities.maxImageExtent, surface_capabilities.currentExtent);
	properties_.array_layers_ = choose_image_array_layers(1U, surface_capabilities.maxImageArrayLayers);
	properties_.surface_format_ = choose_surface_format(properties_.surface_format_, surface_formats_, surface_format_priority_list_);
	VkFormatProperties format_properties;
	vkGetPhysicalDeviceFormatProperties(this->device_.get_gpu().get_handle(), properties_.surface_format_.format, &format_properties);
	this->image_usage_flags_ = choose_image_usage(image_usage_flags, surface_capabilities.supportedUsageFlags, format_properties.optimalTilingFeatures);
	properties_.image_usage_ = composite_image_flags(this->image_usage_flags_);
	properties_.pre_transform_ = choose_transform(transform, surface_capabilities.supportedTransforms, surface_capabilities.currentTransform);
	properties_.composite_alpha_ = choose_composite_alpha(VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR, surface_capabilities.supportedCompositeAlpha);

	// Pass through defaults to the create function
	properties_.old_swapchain_ = old_swapchain.get_handle();
	properties_.present_mode_ = present_mode;
}

SPSwapchain::SPSwapchain(SPSwapchain&& other)
	: device_ { other.device_ }
	, handle_{ other.handle_ }
	, image_usage_flags_ { std::move(other.image_usage_flags_) }
	, images_ { std::move(other.images_) }
	, properties_ { std::move(other.properties_) }
{	
	other.handle_ = VK_NULL_HANDLE;
}

SPSwapchain::~SPSwapchain()
{
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroySwapchainKHR(device_.get_handle(), handle_, nullptr);
	}
}

SwapchainProperties& SPSwapchain::get_properties()
{

	return properties_;
}

VkSwapchainKHR SPSwapchain::get_handle() const
{
	return handle_;
}

void SPSwapchain::create()
{
	// Revalidate the present mode and surface format
	properties_.present_mode_ = choose_present_mode(properties_.present_mode_, present_modes_, present_mode_priority_list_);
	properties_.surface_format_ = choose_surface_format(properties_.surface_format_, surface_formats_, surface_format_priority_list_);

	VkSwapchainCreateInfoKHR create_info{ VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR };
	create_info.minImageCount = properties_.image_count_;
	create_info.imageExtent = properties_.extent_;
	create_info.presentMode = properties_.present_mode_;
	create_info.imageFormat = properties_.surface_format_.format;
	create_info.imageColorSpace = properties_.surface_format_.colorSpace;
	create_info.imageArrayLayers = properties_.array_layers_;
	create_info.imageUsage = properties_.image_usage_;
	create_info.preTransform = properties_.pre_transform_;
	create_info.compositeAlpha = properties_.composite_alpha_;
	create_info.oldSwapchain = properties_.old_swapchain_;
	create_info.surface = device_.get_surface();

	VkResult result = vkCreateSwapchainKHR(device_.get_handle(), &create_info, nullptr, &handle_);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create Swapchain" };
	}

	uint32_t image_available{ 0u };
	VK_CHECK(vkGetSwapchainImagesKHR(device_.get_handle(), handle_, &image_available, nullptr));

	images_.resize(image_available);

	VK_CHECK(vkGetSwapchainImagesKHR(device_.get_handle(), handle_, &image_available, images_.data()));
}

void SPSwapchain::set_present_mode_priority(const std::vector<VkPresentModeKHR>& new_present_mode_priority_list)
{
	present_mode_priority_list_ = new_present_mode_priority_list;
}

void SPSwapchain::set_surface_format_priority(const std::vector<VkSurfaceFormatKHR>& new_surface_format_priority_list)
{
	surface_format_priority_list_ = new_surface_format_priority_list;
}

const VkExtent2D& SPSwapchain::get_extent() const
{
	return properties_.extent_;
}

const std::vector<VkImage>& SPSwapchain::get_images() const
{
	return images_;
}

VkFormat SPSwapchain::get_format() const
{
	return properties_.surface_format_.format;
}

VkImageUsageFlags SPSwapchain::get_usage() const
{
	return  properties_.image_usage_;
}

VkSurfaceKHR SPSwapchain::get_surface() const
{

	return device_.get_surface();
}

VkResult SPSwapchain::acquire_next_image(uint32_t& image_index, VkSemaphore image_acquired_semaphore, VkFence fence) const
{

	return vkAcquireNextImageKHR(device_.get_handle(), handle_, std::numeric_limits<uint64_t>::max(), image_acquired_semaphore, fence, &image_index);

}
