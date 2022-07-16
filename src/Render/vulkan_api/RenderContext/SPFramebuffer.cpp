#include "SPFramebuffer.h"
#include "../Driver/SPDevice.h"
#include "SPRenderTarget.h"
#include "../Pipeline/SPRenderPass.h"

SPFramebuffer::SPFramebuffer(SPDevice& device, const SPRenderTarget& render_target, const SPRenderPass& render_pass)
	: device_{ device }
	, extent_{ render_target.get_extent() }
{
	std::vector<VkImageView> attachments;

	for (auto& view : render_target.get_views())
	{
		attachments.emplace_back(view.get_handle());
	}

	VkFramebufferCreateInfo create_info{ VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO };

	create_info.renderPass = render_pass.get_handle();
	create_info.attachmentCount = sp::to_u32(attachments.size());
	create_info.pAttachments = attachments.data();
	create_info.width = extent_.width;
	create_info.height = extent_.height;
	create_info.layers = 1;

	auto result = vkCreateFramebuffer(device.get_handle(), &create_info, nullptr, &handle_);

	if (result != VK_SUCCESS)
	{
		throw VulkanException{ result, "Cannot create Framebuffer" };
	}
}

SPFramebuffer::SPFramebuffer(SPFramebuffer&& other) noexcept
	: device_{ other.device_ }
	, handle_{ other.handle_ }
	, extent_{ other.extent_ }
{
	other.handle_ = VK_NULL_HANDLE;
}

SPFramebuffer::~SPFramebuffer()
{
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyFramebuffer(device_.get_handle(), handle_, nullptr);
	}
}

VkFramebuffer SPFramebuffer::get_handle() const
{
	return handle_;
}

const VkExtent2D& SPFramebuffer::get_extent() const
{
	return extent_;
}
