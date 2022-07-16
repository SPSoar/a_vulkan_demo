#include "SPRenderContext.h"
#include "../ak_engine/SPEngine.h"
#include "../Driver/SPDevice.h"
#include "../Command/SPQueue.h"
#include "../RenderContext/SPSwapchain.h"
#include "../RenderContext/SPImage.h"
#include "../ak_pipeline/SPRenderWorker.h"

VkFormat SPRenderContext::DEFAULT_VK_FORMAT = VK_FORMAT_R8G8B8A8_SRGB;

SPRenderContext::SPRenderContext(SPEngine& engine, uint32_t window_width, uint32_t window_height)
    : engine_{ engine }
    , queue_ { engine.get_device().get_suitable_graphics_queue()}
    , surface_extent_ { window_width, window_height }
	, thread_count_{1}
	, prepared_ {false}
	, create_render_target_func_ { SPRenderTarget::DEFAULT_CREATE_FUNC_ }
    , present_mode_priority_list_ { VK_PRESENT_MODE_FIFO_KHR, VK_PRESENT_MODE_MAILBOX_KHR }
    , surface_format_priority_list_{
         { VK_FORMAT_R8G8B8A8_SRGB , VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
         { VK_FORMAT_B8G8R8A8_SRGB,  VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
         { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
         { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
    }
                    
{
  
}

void SPRenderContext::init()
{

    if (engine_.get_surface() != VK_NULL_HANDLE)
    {
		VkSurfaceCapabilitiesKHR surface_properties;
		VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(engine_.get_device().get_gpu().get_handle(),
			engine_.get_surface(),
			&surface_properties));

		if (surface_properties.currentExtent.width == 0xFFFFFFFF)
		{
			swapchain_ = std::make_unique<SPSwapchain>(engine_.get_device(), engine_.get_surface(), surface_extent_);
		}
		else
		{
			swapchain_ = std::make_unique<SPSwapchain>(engine_.get_device(), engine_.get_surface());
		}
    }

	set_present_mode_priority(
		{
		  VK_PRESENT_MODE_MAILBOX_KHR,
		  VK_PRESENT_MODE_FIFO_KHR,
		  VK_PRESENT_MODE_IMMEDIATE_KHR
		});
	request_image_format(VK_FORMAT_R8G8B8A8_SRGB);
	set_surface_format_priority({
			{VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
			{VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
			{VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR},
			{VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR}
		});
	switch (Vsync::Default)
	{
	case Vsync::ON:
		request_present_mode(VK_PRESENT_MODE_FIFO_KHR);
		break;
	case Vsync::OFF:
		request_present_mode(VK_PRESENT_MODE_MAILBOX_KHR);
	default:
		break;
	}
}

void SPRenderContext::set_present_mode_priority(const std::vector<VkPresentModeKHR>& new_present_mode_priority_list)
{
    this->present_mode_priority_list_ = new_present_mode_priority_list;
}

void SPRenderContext::set_surface_format_priority(const std::vector<VkSurfaceFormatKHR>& new_surface_format_priority_list)
{
    this->surface_format_priority_list_ = new_surface_format_priority_list;
}

void SPRenderContext::request_image_format(const VkFormat format)
{
	if (swapchain_)
	{
		auto& properties = swapchain_->get_properties();
		properties.surface_format_.format = format;
	}
}

void SPRenderContext::prepare(size_t thread_count, SPRenderTarget::CreateFunc create_render_target_func)
{
	engine_.get_device().wait_idle();

	if (swapchain_)
	{
		swapchain_->set_present_mode_priority(present_mode_priority_list_);
		swapchain_->set_surface_format_priority(surface_format_priority_list_);
		swapchain_->create();

		surface_extent_ = swapchain_->get_extent();

		VkExtent3D extent{ surface_extent_.width, surface_extent_.height, 1 };

		for (auto& image_handle : swapchain_->get_images())
		{
			auto swapchain_image = SPImage {
				engine_.get_device(), 
				image_handle,
				extent,
				swapchain_->get_format(),
				swapchain_->get_usage()
			};
			auto render_target = create_render_target_func(std::move(swapchain_image));
			frames_.emplace_back(std::make_unique<SPRenderFrame>(engine_.get_device(), std::move(render_target), thread_count));
		}
	}
	else
	{
		// Otherwise, create a single RenderFrame
		swapchain_ = nullptr;

		auto color_image = SPImage{ engine_.get_device(),
									   VkExtent3D{surface_extent_.width, surface_extent_.height, 1},
									   DEFAULT_VK_FORMAT,        // We can use any format here that we like
									   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
									   VMA_MEMORY_USAGE_GPU_ONLY };

		auto render_target = create_render_target_func(std::move(color_image));
		frames_.emplace_back(std::make_unique<SPRenderFrame>(engine_.get_device(), std::move(render_target), thread_count));
	}

	this->create_render_target_func_ = create_render_target_func;
	this->thread_count_ = thread_count;
	this->prepared_ = true;
}

void SPRenderContext::prepare_queue()
{
	depth_format_ = vkTool::get_suitable_depth_format(engine_.get_device().get_gpu().get_handle());
	VkSemaphoreCreateInfo semaphore_create_info = vkTool::semaphore_create_info();
	VK_CHECK(vkCreateSemaphore(engine_.get_device().get_handle(), &semaphore_create_info, nullptr, &semaphores_.acquired_image_ready_));
	VK_CHECK(vkCreateSemaphore(engine_.get_device().get_handle(), &semaphore_create_info, nullptr, &semaphores_.render_complete_));
	submit_info_ = vkTool::submit_info();
	submit_info_.pWaitDstStageMask = &submit_pipeline_stages_;

	submit_info_.waitSemaphoreCount = 1;
	submit_info_.pWaitSemaphores = &semaphores_.acquired_image_ready_;
	submit_info_.signalSemaphoreCount = 1;
	submit_info_.pSignalSemaphores = &semaphores_.render_complete_;

	vk_queue_ = engine_.get_device().get_suitable_graphics_queue().get_handle();

}

void SPRenderContext::create_swapchain_buffers()
{
	if (has_swapchain())
	{
		auto& images = get_swapchain().get_images();

		// Get the swap chain buffers containing the image and imageview
		for (auto& swapchain_buffer : swapchain_buffers_)
		{
			vkDestroyImageView(engine_.get_device().get_handle(), swapchain_buffer.view_, nullptr);
		}
		swapchain_buffers_.clear();
		swapchain_buffers_.resize(images.size());
		for (uint32_t i = 0; i < images.size(); i++)
		{
			VkImageViewCreateInfo color_attachment_view = {};
			color_attachment_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			color_attachment_view.pNext = NULL;
			color_attachment_view.format = get_swapchain().get_format();
			color_attachment_view.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A };
			color_attachment_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			color_attachment_view.subresourceRange.baseMipLevel = 0;
			color_attachment_view.subresourceRange.levelCount = 1;
			color_attachment_view.subresourceRange.baseArrayLayer = 0;
			color_attachment_view.subresourceRange.layerCount = 1;
			color_attachment_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
			color_attachment_view.flags = 0;

			swapchain_buffers_[i].image_ = images[i];

			color_attachment_view.image = swapchain_buffers_[i].image_;

			VK_CHECK(vkCreateImageView(engine_.get_device().get_handle(), &color_attachment_view, nullptr, &swapchain_buffers_[i].view_));
		}
	}
	else
	{
		auto& frames = get_render_frames();

		// Get the swap chain buffers containing the image and imageview
		swapchain_buffers_.clear();
		swapchain_buffers_.resize(frames.size());
		for (uint32_t i = 0; i < frames.size(); i++)
		{
			auto& image_view = *frames[i]->get_render_target().get_views().begin();

			swapchain_buffers_[i].image_ = image_view.get_image().get_handle();
			swapchain_buffers_[i].view_ = image_view.get_handle();
		}
	}
}

bool SPRenderContext::has_swapchain()
{
	return swapchain_ != nullptr;
}

SPSwapchain& SPRenderContext::get_swapchain()
{
	assert(swapchain_ && "Swapchain is not valid");
	return *swapchain_;
}

std::vector<std::unique_ptr<SPRenderFrame>>& SPRenderContext::get_render_frames()
{
	return frames_;
}

VkExtent2D const& SPRenderContext::get_surface_extent() const
{
	return surface_extent_;
}

VkFormat SPRenderContext::get_format() const
{
	VkFormat format = DEFAULT_VK_FORMAT;

	if (swapchain_)
	{
		format = swapchain_->get_format();
	}

	return format;
}

SPDevice& SPRenderContext::get_device()
{
	return engine_.get_device();
}

bool SPRenderContext::handle_surface_changes(SPRenderWorker& render_worker, bool force_update)
{
	if (!swapchain_)
	{
		//LOGW("Can't handle surface changes in headless mode, skipping.");
		return false;
	}

	VkSurfaceCapabilitiesKHR surface_properties;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(get_device().get_gpu().get_handle(),
		swapchain_->get_surface(),
		&surface_properties));

	if (surface_properties.currentExtent.width == 0xFFFFFFFF)
	{
		return false;
	}

	// Only recreate the swapchain if the dimensions have changed;
	// handle_surface_changes() is called on VK_SUBOPTIMAL_KHR,
	// which might not be due to a surface resize
	if (surface_properties.currentExtent.width != surface_extent_.width ||
		surface_properties.currentExtent.height != surface_extent_.height ||
		force_update)
	{
		// Recreate swapchain
		get_device().wait_idle();

		update_swapchain(surface_properties.currentExtent, pre_transform_);

		surface_extent_ = surface_properties.currentExtent;

		return true;
	}

	return false;
}

void SPRenderContext::update_swapchain(const VkExtent2D& extent, const VkSurfaceTransformFlagBitsKHR transform)
{
	if (!swapchain_)
	{
		//LOGW("Can't update the swapchains extent and surface transform in headless mode, skipping.");
		return;
	}

	engine_.get_device().get_resource_cache().clear_framebuffers();

	auto width = extent.width;
	auto height = extent.height;
	if (transform == VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR || transform == VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR)
	{
		// Pre-rotation: always use native orientation i.e. if rotated, use width and height of identity transform
		std::swap(width, height);
	}

	swapchain_ = std::make_unique<SPSwapchain>(*swapchain_, VkExtent2D{ width, height }, transform);

	// Save the preTransform attribute for future rotations
	pre_transform_ = transform;

	recreate();
}

void SPRenderContext::update_swapchain(const std::set<VkImageUsageFlagBits>& image_usage_flags)
{
	if (!swapchain_)
	{
		//LOGW("Can't update the swapchains image usage in headless mode, skipping.");
		return;
	}

	engine_.get_device().get_resource_cache().clear_framebuffers();

	swapchain_ = std::make_unique<SPSwapchain>(*swapchain_, image_usage_flags);

	recreate();
}

void SPRenderContext::recreate()
{
	//LOGI("Recreated swapchain");

	VkExtent2D swapchain_extent = swapchain_->get_extent();
	VkExtent3D extent{ swapchain_extent.width, swapchain_extent.height, 1 };

	auto frame_it = frames_.begin();

	for (auto& image_handle : swapchain_->get_images())
	{
		SPImage swapchain_image{ engine_.get_device(), image_handle,
									extent,
									swapchain_->get_format(),
									swapchain_->get_usage() };

		auto render_target = create_render_target_func_(std::move(swapchain_image));

		if (frame_it != frames_.end())
		{
			(*frame_it)->update_render_target(std::move(render_target));
		}
		else
		{
			// Create a new frame if the new swapchain has more images than current frames
			frames_.emplace_back(std::make_unique<SPRenderFrame>(engine_.get_device(), std::move(render_target), thread_count_));
		}

		++frame_it;
	}

	engine_.get_device().get_resource_cache().clear_framebuffers();
}

void SPRenderContext::request_present_mode(const VkPresentModeKHR present_mode)
{
	if (swapchain_)
	{
		auto& properties = swapchain_->get_properties();
		properties.present_mode_ = present_mode;
	}
}
void SPRenderContext::draw(SPRenderWorker& render_worker, const sp_uint_32& width, const sp_uint_32& height)
{
	//queue_begin_label(queue_.get_handle(), std::string("Graphics queue command buffer " + std::to_string(current_buffer_) + " submission").c_str(), { 1.0f, 1.0f, 1.0f, 1.0f });

	prepare_frame(render_worker, width, height);

	submit_info_.commandBufferCount = 1;
	submit_info_.pCommandBuffers = &render_worker.draw_cmd_buffers_[current_buffer_];
	VK_CHECK(vkQueueSubmit(queue_.get_handle(), 1, &submit_info_, VK_NULL_HANDLE));

	submit_frame(render_worker, width, height);

	//queue_end_label(queue_.get_handle());

}


void SPRenderContext::resize(SPRenderWorker& render_worker, const sp_uint_32& width, const sp_uint_32& height)
{
	if (!prepared_)
	{
		return;
	}

	handle_surface_changes(render_worker, false);

	prepared_ = false;

	// Ensure all operations on the device have been finished before destroying resources
	get_device().wait_idle();

	create_swapchain_buffers();

	render_worker.resize(width, height);

	prepared_ = true;
}

void SPRenderContext::handle_surface_changes(SPRenderWorker& render_worker)
{
	VkSurfaceCapabilitiesKHR surface_properties;
	VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(engine_.get_device().get_gpu().get_handle(),
		get_swapchain().get_surface(),
		&surface_properties));

	if (surface_properties.currentExtent.width != get_surface_extent().width ||
		surface_properties.currentExtent.height != get_surface_extent().height)
	{
		resize(render_worker, surface_properties.currentExtent.width, surface_properties.currentExtent.height);
	}
}

std::vector<SPSwapchainBuffer>& SPRenderContext::get_swapchain_buffers()
{
	return swapchain_buffers_;
}

VkFormat& SPRenderContext::get_depth_format()
{
	return depth_format_;
}

void SPRenderContext::prepare_frame(SPRenderWorker& render_worker, const sp_uint_32& width, const sp_uint_32& height)
{
	if (has_swapchain())
	{
		handle_surface_changes(render_worker);
		// Acquire the next image from the swap chain
		VkResult result = get_swapchain().acquire_next_image(current_buffer_, semaphores_.acquired_image_ready_, VK_NULL_HANDLE);
		// Recreate the swapchain if it's no longer compatible with the surface (OUT_OF_DATE) or no longer optimal for presentation (SUBOPTIMAL)
		if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR))
		{
			resize(render_worker, width, height);
		}
		else
		{
			VK_CHECK(result);
		}
	}
}

void SPRenderContext::submit_frame(SPRenderWorker& render_worker, const sp_uint_32& width, const sp_uint_32& height)
{
	if (has_swapchain())
	{
		const auto& queue = engine_.get_device().get_queue_by_present(0);

		VkSwapchainKHR sc = get_swapchain().get_handle();

		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		present_info.pNext = NULL;
		present_info.swapchainCount = 1;
		present_info.pSwapchains = &sc;
		present_info.pImageIndices = &current_buffer_;
		// Check if a wait semaphore has been specified to wait for before presenting the image
		if (semaphores_.render_complete_ != VK_NULL_HANDLE)
		{
			present_info.pWaitSemaphores = &semaphores_.render_complete_;
			present_info.waitSemaphoreCount = 1;
		}

		VkResult present_result = queue.present(present_info);

		if (!((present_result == VK_SUCCESS) || (present_result == VK_SUBOPTIMAL_KHR)))
		{
			if (present_result == VK_ERROR_OUT_OF_DATE_KHR)
			{
				// Swap chain is no longer compatible with the surface and needs to be recreated
				resize(render_worker, width, height);
				return;
			}
			else
			{
				VK_CHECK(present_result);
			}
		}
	}

	// DO NOT USE
	// vkDeviceWaitIdle and vkQueueWaitIdle are extremely expensive functions, and are used here purely for demonstrating the vulkan API
	// without having to concern ourselves with proper syncronization. These functions should NEVER be used inside the render loop like this (every frame).
	VK_CHECK(engine_.get_device().get_queue_by_present(0).wait_idle());
}

void SPRenderContext::queue_begin_label(VkQueue queue, const char* label_name, std::vector<float> color)
{
	if (!debug_utils_supported_)
	{
		return;
	}
	VkDebugUtilsLabelEXT label = { VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
	label.pLabelName = label_name;
	label.color[0] = color[0];
	label.color[1] = color[1];
	label.color[2] = color[2];
	label.color[3] = color[3];
	vkQueueBeginDebugUtilsLabelEXT(queue, &label);
}

void SPRenderContext::queue_insert_label(VkQueue queue, const char* label_name, std::vector<float> color)
{
	if (!debug_utils_supported_)
	{
		return;
	}
	VkDebugUtilsLabelEXT label = { VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT };
	label.pLabelName = label_name;
	label.color[0] = color[0];
	label.color[1] = color[1];
	label.color[2] = color[2];
	label.color[3] = color[3];
	vkQueueInsertDebugUtilsLabelEXT(queue, &label);
}

void SPRenderContext::queue_end_label(VkQueue queue)
{
	if (!debug_utils_supported_)
	{
		return;
	}
	vkQueueEndDebugUtilsLabelEXT(queue);
}
