#include "SPWorkFlowVulkan.h"
#include "SPVulkanWindowsDriver.h"
#include "SPDrawingPadVulkan.h"
SPWorkFlowVulkan::SPWorkFlowVulkan(SPVulkanWindowsDriver* manager, WorkFlowType wfType)
	: SPWorkFlow(wfType)
	, s_manager(manager)
{
}

SPWorkFlowVulkan::~SPWorkFlowVulkan()
{

}

void SPWorkFlowVulkan::coloring(SPDrawingPad* drawingPad)
{
	SPDrawingPadVulkan* vkDrawingPad = static_cast<SPDrawingPadVulkan*>(drawingPad);
	if (!vkDrawingPad) return;

	/*VkAttachmentDescription colorAttachment{};
	colorAttachment.format = vkDrawingPad->swapChainImageFormat;
	colorAttachment.samples = s_manager->getVulkanContext().maxMSAASamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = s_manager->findDepthFormat();
	depthAttachment.samples = s_manager->getVulkanContext().maxMSAASamples;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = vkDrawingPad->swapChainImageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 2;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	if (s_artist->subpass.pColorAttachments)
	{
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
	}
	if (s_artist->subpass.pDepthStencilAttachment)
	{
		subpass.pDepthStencilAttachment = &depthAttachmentRef;
	}

	if (s_artist->subpass.pResolveAttachments)
	{
		subpass.pResolveAttachments = &colorAttachmentResolveRef;
	}


	VkSubpassDependency dependency{};
	dependency.srcSubpass = s_artist->dependency.srcSubpass;
	dependency.dstSubpass = s_artist->dependency.dstSubpass;
	dependency.srcStageMask = s_artist->dependency.srcStageMask;
	dependency.srcAccessMask = s_artist->dependency.srcAccessMask;
	dependency.dstStageMask = s_artist->dependency.dstStageMask;
	dependency.dstAccessMask = s_artist->dependency.dstAccessMask;

	std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;
	if (vkCreateRenderPass(s_manager->getVulkanContext().device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}*/
}
