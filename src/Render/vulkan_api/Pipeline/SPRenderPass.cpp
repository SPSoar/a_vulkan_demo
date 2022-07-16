#include "SPRenderPass.h"
#include "../RenderContext/SPRenderTarget.h"
#include "../Driver/SPDevice.h"

//inline VkResult create_vk_renderpass(VkDevice device, VkRenderPassCreateInfo& create_info, VkRenderPass* handle)
//{
//	return vkCreateRenderPass(device, &create_info, nullptr, handle);
//}
//
//inline VkResult create_vk_renderpass(VkDevice device, VkRenderPassCreateInfo2KHR& create_info, VkRenderPass* handle)
//{
//	return vkCreateRenderPass2KHR(device, &create_info, nullptr, handle);
//}
//
//
//inline void set_pointer_next(VkSubpassDescription& subpass_description, VkSubpassDescriptionDepthStencilResolveKHR& depth_resolve, VkAttachmentReference& depth_resolve_attachment)
//{
//	// VkSubpassDescription cannot have pNext point to a VkSubpassDescriptionDepthStencilResolveKHR containing a VkAttachmentReference
//}
//
//
//inline void set_pointer_next(VkSubpassDescription2KHR& subpass_description, VkSubpassDescriptionDepthStencilResolveKHR& depth_resolve, VkAttachmentReference2KHR& depth_resolve_attachment)
//{
//	depth_resolve.pDepthStencilResolveAttachment = &depth_resolve_attachment;
//	subpass_description.pNext = &depth_resolve;
//}
//
//inline void set_structure_type(VkAttachmentDescription& attachment)
//{
//	// VkAttachmentDescription has no sType field
//}
//
//inline void set_structure_type(VkAttachmentDescription2KHR& attachment)
//{
//	attachment.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
//}
//
//inline void set_structure_type(VkAttachmentReference& reference)
//{
//	// VkAttachmentReference has no sType field
//}
//
//inline void set_structure_type(VkAttachmentReference2KHR& reference)
//{
//	reference.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
//}
//
//inline void set_structure_type(VkRenderPassCreateInfo& create_info)
//{
//	create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//}
//
//inline void set_structure_type(VkRenderPassCreateInfo2KHR& create_info)
//{
//	create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR;
//}
//
//inline void set_structure_type(VkSubpassDescription& description)
//{
//	// VkSubpassDescription has no sType field
//}
//
//inline void set_structure_type(VkSubpassDescription2KHR& description)
//{
//	description.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR;
//}
//
//
//inline const VkAttachmentReference2KHR* get_depth_resolve_reference(const VkSubpassDescription& subpass_description)
//{
//	// VkSubpassDescription cannot have pNext point to a VkSubpassDescriptionDepthStencilResolveKHR containing a VkAttachmentReference2KHR
//	return nullptr;
//}
//
//inline const VkAttachmentReference2KHR* get_depth_resolve_reference(const VkSubpassDescription2KHR& subpass_description)
//{
//	auto description_depth_resolve = static_cast<const VkSubpassDescriptionDepthStencilResolveKHR*>(subpass_description.pNext);
//
//	const VkAttachmentReference2KHR* depth_resolve_attachment = nullptr;
//	if (description_depth_resolve)
//	{
//		depth_resolve_attachment = description_depth_resolve->pDepthStencilResolveAttachment;
//	}
//
//	return depth_resolve_attachment;
//}
//
//
//template <typename T>
//std::vector<T> get_attachment_descriptions(const std::vector<SPAttachment>& attachments, const std::vector<SPLoadStoreInfo>& load_store_infos)
//{
//	std::vector<T> attachment_descriptions;
//
//	for (size_t i = 0U; i < attachments.size(); ++i)
//	{
//		T attachment{};
//		set_structure_type(attachment);
//
//		attachment.format = attachments[i].format_;
//		attachment.samples = attachments[i].samples_;
//		attachment.initialLayout = attachments[i].initial_layout_;
//		attachment.finalLayout = vkTool::is_depth_stencil_format(attachment.format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//		if (i < load_store_infos.size())
//		{
//			attachment.loadOp = load_store_infos[i].load_op_;
//			attachment.storeOp = load_store_infos[i].store_op_;
//			attachment.stencilLoadOp = load_store_infos[i].load_op_;
//			attachment.stencilStoreOp = load_store_infos[i].store_op_;
//		}
//
//		attachment_descriptions.push_back(std::move(attachment));
//	}
//
//	return attachment_descriptions;
//}
//
//
//template <typename T_SubpassDescription, typename T_AttachmentDescription, typename T_AttachmentReference>
//void set_attachment_layouts(std::vector<T_SubpassDescription>& subpass_descriptions, std::vector<T_AttachmentDescription>& attachment_descriptions)
//{
//	// Make the initial layout same as in the first subpass using that attachment
//	for (auto& subpass : subpass_descriptions)
//	{
//		for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
//		{
//			auto& reference = subpass.pColorAttachments[k];
//			// Set it only if not defined yet
//			if (attachment_descriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
//			{
//				attachment_descriptions[reference.attachment].initialLayout = reference.layout;
//			}
//		}
//
//		for (size_t k = 0U; k < subpass.inputAttachmentCount; ++k)
//		{
//			auto& reference = subpass.pInputAttachments[k];
//			// Set it only if not defined yet
//			if (attachment_descriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
//			{
//				attachment_descriptions[reference.attachment].initialLayout = reference.layout;
//			}
//		}
//
//		if (subpass.pDepthStencilAttachment)
//		{
//			auto& reference = *subpass.pDepthStencilAttachment;
//			// Set it only if not defined yet
//			if (attachment_descriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
//			{
//				attachment_descriptions[reference.attachment].initialLayout = reference.layout;
//			}
//		}
//
//		if (subpass.pResolveAttachments)
//		{
//			for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
//			{
//				auto& reference = subpass.pResolveAttachments[k];
//				// Set it only if not defined yet
//				if (attachment_descriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
//				{
//					attachment_descriptions[reference.attachment].initialLayout = reference.layout;
//				}
//			}
//		}
//
//		if (const auto depth_resolve = get_depth_resolve_reference(subpass))
//		{
//			// Set it only if not defined yet
//			if (attachment_descriptions[depth_resolve->attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
//			{
//				attachment_descriptions[depth_resolve->attachment].initialLayout = depth_resolve->layout;
//			}
//		}
//	}
//
//	// Make the final layout same as the last subpass layout
//	{
//		auto& subpass = subpass_descriptions.back();
//
//		for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
//		{
//			const auto& reference = subpass.pColorAttachments[k];
//
//			attachment_descriptions[reference.attachment].finalLayout = reference.layout;
//		}
//
//		for (size_t k = 0U; k < subpass.inputAttachmentCount; ++k)
//		{
//			const auto& reference = subpass.pInputAttachments[k];
//
//			attachment_descriptions[reference.attachment].finalLayout = reference.layout;
//
//			// Do not use depth attachment if used as input
//			if (vkTool:is_depth_stencil_format(attachment_descriptions[reference.attachment].format))
//			{
//				subpass.pDepthStencilAttachment = nullptr;
//			}
//		}
//
//		if (subpass.pDepthStencilAttachment)
//		{
//			const auto& reference = *subpass.pDepthStencilAttachment;
//
//			attachment_descriptions[reference.attachment].finalLayout = reference.layout;
//		}
//
//		if (subpass.pResolveAttachments)
//		{
//			for (size_t k = 0U; k < subpass.colorAttachmentCount; ++k)
//			{
//				const auto& reference = subpass.pResolveAttachments[k];
//
//				attachment_descriptions[reference.attachment].finalLayout = reference.layout;
//			}
//		}
//
//		if (const auto depth_resolve = get_depth_resolve_reference(subpass))
//		{
//			attachment_descriptions[depth_resolve->attachment].finalLayout = depth_resolve->layout;
//		}
//	}
//}
//
//template <typename T>
//std::vector<T> get_subpass_dependencies(const size_t subpass_count)
//{
//	std::vector<T> dependencies(subpass_count - 1);
//
//	if (subpass_count > 1)
//	{
//		for (uint32_t i = 0; i < sp::to_u32(dependencies.size()); ++i)
//		{
//			// Transition input attachments from color attachment to shader read
//			dependencies[i].srcSubpass = i;
//			dependencies[i].dstSubpass = i + 1;
//			dependencies[i].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//			dependencies[i].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
//			dependencies[i].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
//			dependencies[i].dstAccessMask = VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
//			dependencies[i].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
//		}
//	}
//
//	return dependencies;
//}
//
//template <typename T>
//T get_attachment_reference(const uint32_t attachment, const VkImageLayout layout)
//{
//	T reference{};
//	set_structure_type(reference);
//
//	reference.attachment = attachment;
//	reference.layout = layout;
//
//	return reference;
//}
//
//template<typename T_SubpassDescription, typename T_AttachmentDescription, typename T_AttachmentReference, typename T_SubpassDependency, typename T_RenderPassCreateInfo>
//inline void SPRenderPass::create_renderpass(const std::vector<SPAttachment>& attachments, const std::vector<SPLoadStoreInfo>& load_store_infos, const std::vector<SPSubpassInfo>& subpasses)
//{
//	auto attachment_descriptions = get_attachment_descriptions<T_AttachmentDescription>(attachments, load_store_infos);
//
//	// Store attachments for every subpass
//	std::vector<std::vector<T_AttachmentReference>> input_attachments{ subpass_count_ };
//	std::vector<std::vector<T_AttachmentReference>> color_attachments{ subpass_count_ };
//	std::vector<std::vector<T_AttachmentReference>> depth_stencil_attachments{ subpass_count_ };
//	std::vector<std::vector<T_AttachmentReference>> color_resolve_attachments{ subpass_count_ };
//	std::vector<std::vector<T_AttachmentReference>> depth_resolve_attachments{ subpass_count_ };
//
//	for (size_t i = 0; i < subpasses.size(); ++i)
//	{
//		auto& subpass = subpasses[i];
//
//		// Fill color attachments references
//		for (auto o_attachment : subpass.output_attachments_)
//		{
//			auto  initial_layout = attachments[o_attachment].initial_layout_ == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : attachments[o_attachment].initial_layout_;
//			auto& description = attachment_descriptions[o_attachment];
//			if (!vkTool::is_depth_stencil_format(description.format))
//			{
//				color_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(o_attachment, initial_layout));
//			}
//		}
//
//		// Fill input attachments references
//		for (auto i_attachment : subpass.input_attachments_)
//		{
//			auto default_layout = vkTool::is_depth_stencil_format(attachment_descriptions[i_attachment].format) ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
//			auto initial_layout = attachments[i_attachment].initial_layout_ == VK_IMAGE_LAYOUT_UNDEFINED ? default_layout : attachments[i_attachment].initial_layout_;
//			input_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(i_attachment, initial_layout));
//		}
//
//		for (auto r_attachment : subpass.color_resolve_attachments_)
//		{
//			auto initial_layout = attachments[r_attachment].initial_layout_ == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : attachments[r_attachment].initial_layout_;
//			color_resolve_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(r_attachment, initial_layout));
//		}
//
//		if (!subpass.disable_depth_stencil_attachment_)
//		{
//			// Assumption: depth stencil attachment appears in the list before any depth stencil resolve attachment
//			auto it = find_if(attachments.begin(), attachments.end(), [](const SPAttachment attachment) { return vkTool::is_depth_stencil_format(attachment.format_); });
//			if (it != attachments.end())
//			{
//				auto i_depth_stencil = sp::to_u32(std::distance(attachments.begin(), it));
//				auto initial_layout = it->initial_layout_ == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : it->initial_layout_;
//				depth_stencil_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(i_depth_stencil, initial_layout));
//
//				if (subpass.depth_stencil_resolve_mode_ != VK_RESOLVE_MODE_NONE)
//				{
//					auto i_depth_stencil_resolve = subpass.depth_stencil_resolve_attachment_;
//					initial_layout = attachments[i_depth_stencil_resolve].initial_layout_ == VK_IMAGE_LAYOUT_UNDEFINED ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : attachments[i_depth_stencil_resolve].initial_layout_;
//					depth_resolve_attachments[i].push_back(get_attachment_reference<T_AttachmentReference>(i_depth_stencil_resolve, initial_layout));
//				}
//			}
//		}
//	}
//
//	std::vector<T_SubpassDescription> subpass_descriptions;
//	subpass_descriptions.reserve(subpass_count_);
//	VkSubpassDescriptionDepthStencilResolveKHR depth_resolve{};
//	for (size_t i = 0; i < subpasses.size(); ++i)
//	{
//		auto& subpass = subpasses[i];
//
//		T_SubpassDescription subpass_description{};
//		set_structure_type(subpass_description);
//		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//
//		subpass_description.pInputAttachments = input_attachments[i].empty() ? nullptr : input_attachments[i].data();
//		subpass_description.inputAttachmentCount = sp::to_u32(input_attachments[i].size());
//
//		subpass_description.pColorAttachments = color_attachments[i].empty() ? nullptr : color_attachments[i].data();
//		subpass_description.colorAttachmentCount = sp::to_u32(color_attachments[i].size());
//
//		subpass_description.pResolveAttachments = color_resolve_attachments[i].empty() ? nullptr : color_resolve_attachments[i].data();
//
//		subpass_description.pDepthStencilAttachment = nullptr;
//		if (!depth_stencil_attachments[i].empty())
//		{
//			subpass_description.pDepthStencilAttachment = depth_stencil_attachments[i].data();
//
//			if (!depth_resolve_attachments[i].empty())
//			{
//				// If the pNext list of VkSubpassDescription2 includes a VkSubpassDescriptionDepthStencilResolve structure,
//				// then that structure describes multisample resolve operations for the depth/stencil attachment in a subpass
//				depth_resolve.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE_KHR;
//				depth_resolve.depthResolveMode = subpass.depth_stencil_resolve_mode_;
//				set_pointer_next(subpass_description, depth_resolve, depth_resolve_attachments[i][0]);
//
//				auto& reference = depth_resolve_attachments[i][0];
//				// Set it only if not defined yet
//				if (attachment_descriptions[reference.attachment].initialLayout == VK_IMAGE_LAYOUT_UNDEFINED)
//				{
//					attachment_descriptions[reference.attachment].initialLayout = reference.layout;
//				}
//			}
//		}
//
//		subpass_descriptions.push_back(subpass_description);
//	}
//
//	// Default subpass
//	if (subpasses.empty())
//	{
//		T_SubpassDescription subpass_description{};
//		set_structure_type(subpass_description);
//		subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//		uint32_t default_depth_stencil_attachment{ VK_ATTACHMENT_UNUSED };
//
//		for (uint32_t k = 0U; k < sp::to_u32(attachment_descriptions.size()); ++k)
//		{
//			if (vkTool::is_depth_stencil_format(attachments[k].format_))
//			{
//				if (default_depth_stencil_attachment == VK_ATTACHMENT_UNUSED)
//				{
//					default_depth_stencil_attachment = k;
//				}
//				continue;
//			}
//
//			color_attachments[0].push_back(get_attachment_reference<T_AttachmentReference>(k, VK_IMAGE_LAYOUT_GENERAL));
//		}
//
//		subpass_description.pColorAttachments = color_attachments[0].data();
//
//		if (default_depth_stencil_attachment != VK_ATTACHMENT_UNUSED)
//		{
//			depth_stencil_attachments[0].push_back(get_attachment_reference<T_AttachmentReference>(default_depth_stencil_attachment, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL));
//
//			subpass_description.pDepthStencilAttachment = depth_stencil_attachments[0].data();
//		}
//
//		subpass_descriptions.push_back(subpass_description);
//	}
//
//	set_attachment_layouts<T_SubpassDescription, T_AttachmentDescription, T_AttachmentReference>(subpass_descriptions, attachment_descriptions);
//
//	color_output_count_.reserve(subpass_count_);
//	for (size_t i = 0; i < subpass_count_; i++)
//	{
//		color_output_count_.push_back(sp::to_u32(color_attachments[i].size()));
//	}
//
//	const auto& subpass_dependencies = get_subpass_dependencies<T_SubpassDependency>(subpass_count_);
//
//	T_RenderPassCreateInfo create_info{};
//	set_structure_type(create_info);
//	create_info.attachmentCount = sp::to_u32(attachment_descriptions.size());
//	create_info.pAttachments = attachment_descriptions.data();
//	create_info.subpassCount = sp::to_u32(subpass_descriptions.size());
//	create_info.pSubpasses = subpass_descriptions.data();
//	create_info.dependencyCount = sp::to_u32(subpass_dependencies.size());
//	create_info.pDependencies = subpass_dependencies.data();
//
//	auto result = create_vk_renderpass(device_.get_handle(), create_info, &handle_);
//
//	if (result != VK_SUCCESS)
//	{
//		throw VulkanException{ result, "Cannot create RenderPass" };
//	}
//
//}


SPRenderPass::SPRenderPass(SPDevice& device, const std::vector<SPAttachment>& attachments, const std::vector<SPLoadStoreInfo>& load_store_infos, const std::vector<SPSubpassInfo>& subpasses)
	: device_{ device }
	, subpass_count_{ std::max<uint64_t>(1, subpasses.size()) }
	, color_output_count_{}
{
	/*if (device.is_enabled(VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME))
	{
		create_renderpass<VkSubpassDescription2KHR, VkAttachmentDescription2KHR, VkAttachmentReference2KHR, VkSubpassDependency2KHR, VkRenderPassCreateInfo2KHR>(attachments, load_store_infos, subpasses);
	}
	else
	{
		create_renderpass<VkSubpassDescription, VkAttachmentDescription, VkAttachmentReference, VkSubpassDependency, VkRenderPassCreateInfo>(attachments, load_store_infos, subpasses);
	}*/

}

SPRenderPass::SPRenderPass(SPRenderPass&& other) noexcept
	: device_{ other.device_}
	, subpass_count_{ other.subpass_count_ }
	, color_output_count_{ other.color_output_count_}
	, handle_{other.handle_}
{
	other.handle_ = VK_NULL_HANDLE;
}

SPRenderPass::~SPRenderPass()
{
	// Destroy render pass
	if (handle_ != VK_NULL_HANDLE)
	{
		vkDestroyRenderPass(device_.get_handle(), handle_, nullptr);
	}
}

const VkRenderPass& SPRenderPass::get_handle() const
{
	return handle_;
}
