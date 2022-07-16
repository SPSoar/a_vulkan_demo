#include "SPDrawingPadVulkan.h"
#include "SPVulkanDrawingSkill.h"
#include "../../SPNeMu.h"
#include "SPBufferVulkan.h"
#include "../../SPMangaka.h"
SINGLETON_CREATE(SPDrawingPadVulkan);

void SPDrawingPadVulkan::powerUp(SPStudioManager* manager)
{
	this->s_manager = static_cast<SPVulkanWindowsDriver*>(manager);
	surfaceFormatInfo = s_manager->getSurfaceFormatInfo();
	presentModeInfo = s_manager->getPresentModeInfo();

	SPVulkanArtist* artist = new SPVulkanArtist(*SPDrawingPadVulkan::get());
	artist->init();
	SPDrawingPadVulkan::get()->rigestArtist(artist);
	SPDrawingPadVulkan::get()->init();
	SPVulkanDrawingSkill* skill = new SPVulkanDrawingSkill(*s_manager, *artist);
	skill->init();
	artist->addDrawingSkill(skill);

}

void SPDrawingPadVulkan::powerOff()
{
}

void SPDrawingPadVulkan::submitGenkoDesign(SPNeMu* nemu)
{
	this->nemu = nemu;
}

void SPDrawingPadVulkan::creatGenko(SPWorkFlow* workFlow)
{

	s_artist->skill->startDraw();
	s_artist->skill->draw();
	s_artist->skill->endDraw();
}


void SPDrawingPadVulkan::createSwapChain(const Canvas& canvas)
{
	VkSurfaceFormatKHR surfaceFormat = surfaceFormatInfo[0];
	for (const auto& availableFormat : surfaceFormatInfo) {
		if (availableFormat.format == s_manager->getVulkanImgFormat(canvas.format)
			&& availableFormat.colorSpace == s_manager->getVulkanColorSpace(canvas.colorSpace)) {
			surfaceFormat = availableFormat;
			break;
		}
	}

	VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto& availablePresentMode : presentModeInfo) {
		if (availablePresentMode == s_manager->getVulkanPresentMode(canvas.presentMode)) {
			presentMode = availablePresentMode;
			break;
		}
	}

	VkSurfaceCapabilitiesKHR capabilities = s_manager->getSurfaceCapabilitiesInfo();
	spuint imageCount = capabilities.minImageCount + 1;
	if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount) {
		imageCount = capabilities.maxImageCount;
	}


	s_actualExtent.width = capabilities.maxImageExtent.width;
	s_actualExtent.height = capabilities.maxImageExtent.height;


	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = s_manager->getVulkanContext().surface;

	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = s_actualExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	uint32_t queueFamilyIndices[] =
	{
		s_manager->getVulkanContext().graphicsQueueFamilyIndex,
		s_manager->getVulkanContext().presentQueueFamilyIndex
	};
	auto& device = s_manager->getVulkanContext().device;
	if (s_manager->getVulkanContext().graphicsQueueFamilyIndex != s_manager->getVulkanContext().presentQueueFamilyIndex)
	{
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}

	createInfo.preTransform = capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

	swapChainImageFormat = surfaceFormat.format;

	swapChainImageViews.resize(imageCount);

	for (uint32_t i = 0; i < imageCount; i++) {
		swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
	}
}

void SPDrawingPadVulkan::clearSwapChain()
{
	auto& device = s_manager->getVulkanContext().device;
	if (swapChain != VK_NULL_HANDLE)
		vkDestroySwapchainKHR(device, swapChain, nullptr);
	if (swapChainImageViews.size())
	{
		for (uint32_t i = 0; i < swapChainImageViews.size(); i++) {
			vkDestroyImageView(device, swapChainImageViews[i], nullptr);
		}
		swapChainImageViews.clear();
		swapChainImageViews.resize(0);
	}

	if (swapChainImages.size())
	{
		swapChainImageViews.clear();
		swapChainImageViews.resize(0);
	}
}

void SPDrawingPadVulkan::rigestArtist(SPVulkanArtist* artist)
{
	this->s_artist = artist;

}

void SPDrawingPadVulkan::init()
{
	createDescriptorSetLayout();

}

SPVulkanArtist* SPDrawingPadVulkan::getArtist()
{
	return s_artist;
}



void SPDrawingPadVulkan::cleanUpDrawingPad(const Canvas& canvas)
{
	clearSwapChain();
	//createSwapChain(canvas);
}

void SPDrawingPadVulkan::design()
{
	createSwapChain(Canvas());
	createRenderPass();
	createGraphicsPipeline();
	prepareResource();
	createCommandBuffers();
}

void SPDrawingPadVulkan::draw()
{

}

void SPDrawingPadVulkan::clearDrawingBoard()
{
	auto& device = s_manager->getVulkanContext().device;

	vkDestroyImageView(device, depthImageView, nullptr);
	vkDestroyImage(device, depthImage, nullptr);
	vkFreeMemory(device, depthImageMemory, nullptr);

	vkDestroyImageView(device, colorImageView, nullptr);
	vkDestroyImage(device, colorImage, nullptr);
	vkFreeMemory(device, colorImageMemory, nullptr);

	for (auto framebuffer : swapChainFramebuffers) {
		vkDestroyFramebuffer(device, framebuffer, nullptr);
	}
	auto commandPool = s_manager->getVulkanContext().commandPool;

	vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

	vkDestroyPipeline(device, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
	vkDestroyRenderPass(device, renderPass, nullptr);
	
	for (auto imageView : swapChainImageViews) {
		vkDestroyImageView(device, imageView, nullptr);
	}
	
	vkDestroySwapchainKHR(device, swapChain, nullptr);
	
	for (size_t i = 0; i < swapChainImages.size(); i++) {
		vkDestroyBuffer(device, uniformBuffers[i], nullptr);
		vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
	}

	vkDestroyDescriptorPool(device, descriptorPool, nullptr);
}

void SPDrawingPadVulkan::updateUniformBuffer(spuint currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	//ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	Eigen::AngleAxisf rotationVector(time * 3.141592687 / 2, Eigen::Vector3f(0, 0, 1));
	Eigen::Matrix3f rotationMatrix = Eigen::Matrix3f::Identity();
	rotationMatrix = rotationVector.matrix();
	ubo.model = spmat4::Identity();
	ubo.model.topLeftCorner<3, 3>() = rotationMatrix;

	//ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	spvec3 position(2.0f, 2.0f, 2.0f);
	spvec3 target(0.0f, 0.0f, 0.0f);
	spvec3 up(0.0f, 0.0f, 1.0f);
	spmat3 R;
	R.col(2) = (position - target).normalized();
	R.col(0) = up.cross(R.col(2)).normalized();
	R.col(1) = R.col(2).cross(R.col(0));

	ubo.view.topLeftCorner<3, 3>() = R.transpose();
	ubo.view.topRightCorner<3, 1>() = -(position * R).transpose();
	ubo.view(3, 3) = 1.0f;

	float fovY = 3.14 / 4;
	float aspect = s_actualExtent.width / (float)s_actualExtent.height;
	float zNear = 0.1f;
	float zFar = 10.0f;

	float const tanHalfFovy = tan(fovY / 2);

	ubo.proj = spmat4::Zero();
	ubo.proj(0, 0) = 1 / (aspect * tanHalfFovy);
	ubo.proj(1, 1) = -1 / (tanHalfFovy);
	ubo.proj(2, 2) = -(zFar + zNear) / (zFar - zNear);
	ubo.proj(3, 2) = -1.0;
	ubo.proj(2, 3) = -(2 * zFar * zNear) / (zFar - zNear);



	auto device = s_manager->get()->getVulkanContext().device;

	void* data;
	vkMapMemory(device, uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(device, uniformBuffersMemory[currentImage]);
}


void SPDrawingPadVulkan::createRenderPass()
{

	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapChainImageFormat;
	colorAttachment.samples = s_manager->getVulkanContext().maxMSAASamples;
	colorAttachment.loadOp = s_artist->colorAttachment.loadOp;
	colorAttachment.storeOp = s_artist->colorAttachment.storeOp;
	colorAttachment.stencilLoadOp = s_artist->colorAttachment.stencilLoadOp;
	colorAttachment.stencilStoreOp = s_artist->colorAttachment.stencilStoreOp;
	colorAttachment.initialLayout = s_artist->colorAttachment.initialLayout;
	colorAttachment.finalLayout = s_artist->colorAttachment.finalLayout;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = s_manager->findDepthFormat();
	depthAttachment.samples = s_manager->getVulkanContext().maxMSAASamples;
	depthAttachment.loadOp = s_artist->depthAttachment.loadOp;
	depthAttachment.storeOp = s_artist->depthAttachment.storeOp;
	depthAttachment.stencilLoadOp = s_artist->depthAttachment.stencilLoadOp;
	depthAttachment.stencilStoreOp = s_artist->depthAttachment.stencilStoreOp;
	depthAttachment.initialLayout = s_artist->depthAttachment.initialLayout;
	depthAttachment.finalLayout = s_artist->depthAttachment.finalLayout;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = swapChainImageFormat;
	colorAttachmentResolve.samples = s_artist->colorAttachmentResolve.samples;
	colorAttachmentResolve.loadOp = s_artist->colorAttachmentResolve.loadOp;
	colorAttachmentResolve.storeOp = s_artist->colorAttachmentResolve.storeOp;
	colorAttachmentResolve.stencilLoadOp = s_artist->colorAttachmentResolve.stencilLoadOp;
	colorAttachmentResolve.stencilStoreOp = s_artist->colorAttachmentResolve.stencilStoreOp;
	colorAttachmentResolve.initialLayout = s_artist->colorAttachmentResolve.initialLayout;
	colorAttachmentResolve.finalLayout = s_artist->colorAttachmentResolve.finalLayout;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = s_artist->colorAttachmentRef.attachment;
	colorAttachmentRef.layout = s_artist->colorAttachmentRef.layout;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = s_artist->depthAttachmentRef.attachment;
	depthAttachmentRef.layout = s_artist->depthAttachmentRef.layout;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = s_artist->colorAttachmentResolveRef.attachment;
	colorAttachmentResolveRef.layout = s_artist->colorAttachmentResolveRef.layout;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = s_artist->subpass.pipelineBindPoint;
	if (s_artist->subpass.pColorAttachments) 
	{
		subpass.colorAttachmentCount = s_artist->subpass.colorAttachmentCount;	
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
	}
 }

void SPDrawingPadVulkan::createGraphicsPipeline()
{
	SPMaterialVulkan* mat = static_cast<SPMaterialVulkan*>(nemu->drafts["0"]->material);

	
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
	mat->setShaderStageCreateInfo(vertShaderStageInfo, ShaderType::Vertex);
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
	mat->setShaderStageCreateInfo(fragShaderStageInfo, ShaderType::Fragment);
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

	VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	SPDraftVulkan* sl = static_cast<SPDraftVulkan*>(nemu->drafts["0"]);

	auto bindingDescription = SPDraftVulkan::getBindingDescription();
	auto attributeDescriptions = SPDraftVulkan::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = sl->topology;
	//inputAssembly.primitiveRestartEnable = s_artist->still->primitiveRestartEnable;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	//后期加入设置项
	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)s_actualExtent.width;
	viewport.height = (float)s_actualExtent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = s_actualExtent;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer{};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = s_manager->get()->getVulkanContext().maxMSAASamples;

	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.stencilTestEnable = VK_FALSE;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlending{};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;

	auto device = s_manager->get()->getVulkanContext().device;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo{};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
	
	VkResult asd = vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline);
	
	//if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		//throw std::runtime_error("failed to create graphics pipeline!");
	//}

	//vkDestroyShaderModule(device, moduleFragment, nullptr);
	//vkDestroyShaderModule(device, moduleVertex, nullptr);

}

void SPDrawingPadVulkan::prepareResource()
{
	createColorResources();
	createDepthResources();
	createFramebuffers();
	createUniformBuffers();
	createDescriptorPool();
	createDescriptorSets();
}

void SPDrawingPadVulkan::createCommandBuffers()
{
	auto device = s_manager->get()->getVulkanContext().device;
	auto commandPool = s_manager->get()->getVulkanContext().commandPool;

	SPDraft* sl = nemu->drafts["0"];
	SPBufferVulkan* vertexBuffer = static_cast<SPBufferVulkan*>(sl->vertexBuffer);
	SPBufferVulkan* indexBuffer = static_cast<SPBufferVulkan*>(sl->indexBuffer);


	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

	if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < commandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[i];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = s_actualExtent;

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkBuffer vertexBuffers[] = { vertexBuffer->buffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer->buffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSets[i], 0, nullptr);

		vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(indexBuffer->elementCount), 1, 0, 0, 0);

		vkCmdEndRenderPass(commandBuffers[i]);

		if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void SPDrawingPadVulkan::createDescriptorSetLayout()
{
	VkDescriptorSetLayoutBinding uboLayoutBinding{};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.pImmutableSamplers = nullptr;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

	VkDescriptorSetLayoutBinding samplerLayoutBinding{};
	samplerLayoutBinding.binding = 1;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.pImmutableSamplers = nullptr;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

	std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
	layoutInfo.pBindings = bindings.data();

	auto& device = s_manager->getVulkanContext().device;

	if (vkCreateDescriptorSetLayout(device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor set layout!");
	}
}


VkImageView SPDrawingPadVulkan::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
	auto& device = s_manager->getVulkanContext().device;

	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = mipLevels;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;
	VkImageView imageView;
	if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture image view!");
	}

	return imageView;
}

void SPDrawingPadVulkan::createColorResources()
{
	VkFormat colorFormat = swapChainImageFormat;
	auto& device = s_manager->getVulkanContext().device;
	auto msaaSamples = s_manager->getVulkanContext().maxMSAASamples;

	s_manager->createImage(s_actualExtent.width, s_actualExtent.height, 1, msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, colorImage, colorImageMemory);
	colorImageView = createImageView(colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void SPDrawingPadVulkan::createDepthResources()
{
	VkFormat depthFormat = s_manager->findDepthFormat();
	auto msaaSamples = s_manager->getVulkanContext().maxMSAASamples;
	s_manager->createImage(s_actualExtent.width, s_actualExtent.height, 1, msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
	depthImageView = createImageView(depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);

}

void SPDrawingPadVulkan::createFramebuffers()
{

	swapChainFramebuffers.resize(swapChainImageViews.size());

	for (size_t i = 0; i < swapChainImageViews.size(); i++) {
		std::array<VkImageView, 3> attachments = {
			colorImageView,
			depthImageView,
			swapChainImageViews[i]
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = s_actualExtent.width;
		framebufferInfo.height = s_actualExtent.height;
		framebufferInfo.layers = 1;
		auto& device = s_manager->getVulkanContext().device;
		if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

void SPDrawingPadVulkan::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(swapChainImages.size());
	uniformBuffersMemory.resize(swapChainImages.size());

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		s_manager->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void SPDrawingPadVulkan::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 2> poolSizes{};
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChainImages.size());
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChainImages.size());

	VkDescriptorPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
	poolInfo.pPoolSizes = poolSizes.data();
	poolInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());
	auto& device = s_manager->getVulkanContext().device;
	if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void SPDrawingPadVulkan::createDescriptorSets()
{

	std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
	allocInfo.pSetLayouts = layouts.data();

	auto& device = s_manager->getVulkanContext().device;

	descriptorSets.resize(swapChainImages.size());
	if (vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < swapChainImages.size(); i++) {
		VkDescriptorBufferInfo bufferInfo{};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		SPMaterialVulkan* mat = static_cast<SPMaterialVulkan*>(nemu->drafts["0"]->material);
		VkDescriptorImageInfo imageInfo;
		mat->setDescriptorImageInfo(imageInfo);

		std::array<VkWriteDescriptorSet, 2> descriptorWrites{};

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].dstSet = descriptorSets[i];
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &bufferInfo;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].dstSet = descriptorSets[i];
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pImageInfo = &imageInfo;

		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
	}
}
