#include "vulkan_common_tool.h"
#include <fstream>
#include <sstream>
#include "vk_tool/glsl_compiler.h"




std::string vkTool::get_extension(const std::string& uri)
{
	auto dot_pos = uri.find_last_of('.');
	if (dot_pos == std::string::npos)
	{
		throw std::runtime_error{ "Uri has no extension" };
	}

	return uri.substr(dot_pos + 1);
}

std::vector<uint8_t> vkTool::read_binary_file(const std::string& filename, const uint32_t count)
{

	std::vector<uint8_t> data;

	std::ifstream file;

	file.open(filename, std::ios::in | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + filename);
	}

	uint64_t read_count = count;
	if (count == 0)
	{
		file.seekg(0, std::ios::end);
		read_count = static_cast<uint64_t>(file.tellg());
		file.seekg(0, std::ios::beg);
	}

	data.resize(static_cast<size_t>(read_count));
	file.read(reinterpret_cast<char*>(data.data()), read_count);
	file.close();

	return data;
}

std::vector<std::string> vkTool::split(const std::string& input, char delim)
{
	std::vector<std::string> tokens;

	std::stringstream sstream(input);
	std::string       token;
	while (std::getline(sstream, token, delim))
	{
		tokens.push_back(token);
	}

	return tokens;
}

std::string vkTool::read_text_file(const std::string& filename)
{
	std::vector<std::string> data;

	std::ifstream file;

	file.open(filename, std::ios::in);

	if (!file.is_open())
	{
		throw std::runtime_error("Failed to open file: " + filename);
	}

	return std::string{ (std::istreambuf_iterator<char>(file)),
					   (std::istreambuf_iterator<char>()) };
}

bool vkTool::is_astc(const VkFormat format)
{
	return (format == VK_FORMAT_ASTC_4x4_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_4x4_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_5x4_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_5x4_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_5x5_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_5x5_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_6x5_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_6x5_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_6x6_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_6x6_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_8x5_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_8x5_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_8x6_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_8x6_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_8x8_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_8x8_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_10x5_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_10x5_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_10x6_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_10x6_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_10x8_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_10x8_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_10x10_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_10x10_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_12x10_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_12x10_SRGB_BLOCK ||
		format == VK_FORMAT_ASTC_12x12_UNORM_BLOCK ||
		format == VK_FORMAT_ASTC_12x12_SRGB_BLOCK);
}

bool vkTool::is_depth_only_format(VkFormat format)
{

	return format == VK_FORMAT_D16_UNORM || format == VK_FORMAT_D32_SFLOAT;
}

VkFormat vkTool::get_suitable_depth_format(VkPhysicalDevice physical_device, bool depth_only, const std::vector<VkFormat>& depth_format_priority_list)
{

	VkFormat depth_format{ VK_FORMAT_UNDEFINED };

	for (auto& format : depth_format_priority_list)
	{
		if (depth_only && !is_depth_only_format(format))
		{
			continue;
		}

		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &properties);

		// Format must support depth stencil attachment for optimal tiling
		if (properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
		{
			depth_format = format;
			break;
		}
	}

	if (depth_format != VK_FORMAT_UNDEFINED)
	{
		//LOGI("Depth format selected: {}", to_string(depth_format));
		return depth_format;
	}

	throw std::runtime_error("No suitable depth format could be determined");
}

bool vkTool::is_depth_stencil_format(VkFormat format)
{
	return format == VK_FORMAT_D16_UNORM_S8_UINT ||
		format == VK_FORMAT_D24_UNORM_S8_UINT    ||
		format == VK_FORMAT_D32_SFLOAT_S8_UINT   ||
		is_depth_only_format(format);
}

VkSemaphoreCreateInfo vkTool::semaphore_create_info()
{
	VkSemaphoreCreateInfo semaphore_create_info{};
	semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	return semaphore_create_info;
}

VkSubmitInfo vkTool::submit_info()
{
	VkSubmitInfo submit_info{};
	submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	return submit_info;
}

VkCommandBufferAllocateInfo vkTool::command_buffer_allocate_info(VkCommandPool command_pool, VkCommandBufferLevel level, uint32_t buffer_count)
{

	VkCommandBufferAllocateInfo command_buffer_allocate_info{};
	command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buffer_allocate_info.commandPool = command_pool;
	command_buffer_allocate_info.level = level;
	command_buffer_allocate_info.commandBufferCount = buffer_count;
	return command_buffer_allocate_info;
}

VkFenceCreateInfo vkTool::fence_create_info(VkFenceCreateFlags flags)
{
	VkFenceCreateInfo fence_create_info{};
	fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fence_create_info.flags = flags;
	return fence_create_info;
}

VkDescriptorSetLayoutBinding vkTool::descriptor_set_layout_binding(VkDescriptorType type, VkShaderStageFlags flags, uint32_t binding, uint32_t count)
{
	VkDescriptorSetLayoutBinding set_layout_binding{};
	set_layout_binding.descriptorType = type;
	set_layout_binding.stageFlags = flags;
	set_layout_binding.binding = binding;
	set_layout_binding.descriptorCount = count;
	return set_layout_binding;
}

VkDescriptorSetLayoutCreateInfo vkTool::descriptor_set_layout_create_info(const VkDescriptorSetLayoutBinding* bindings, uint32_t binding_count)
{
	VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
	descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptor_set_layout_create_info.pBindings = bindings;
	descriptor_set_layout_create_info.bindingCount = binding_count;
	return descriptor_set_layout_create_info;
}

VkPipelineLayoutCreateInfo vkTool::pipeline_layout_create_info(const VkDescriptorSetLayout* set_layouts, uint32_t set_layout_count)
{
	VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
	pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeline_layout_create_info.setLayoutCount = set_layout_count;
	pipeline_layout_create_info.pSetLayouts = set_layouts;
	return pipeline_layout_create_info;
}

VkPipelineInputAssemblyStateCreateInfo vkTool::pipeline_input_assembly_state_create_info(VkPrimitiveTopology topology, VkPipelineInputAssemblyStateCreateFlags flags, VkBool32 primitive_restart_enable)
{
	VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{};
	pipeline_input_assembly_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	pipeline_input_assembly_state_create_info.topology = topology;
	pipeline_input_assembly_state_create_info.flags = flags;
	pipeline_input_assembly_state_create_info.primitiveRestartEnable = primitive_restart_enable;
	return pipeline_input_assembly_state_create_info;
}

VkPipelineRasterizationStateCreateInfo vkTool::pipeline_rasterization_state_create_info(VkPolygonMode polygon_mode, VkCullModeFlags cull_mode, VkFrontFace front_face, VkPipelineRasterizationStateCreateFlags flags)
{
	VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info{};
	pipeline_rasterization_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	pipeline_rasterization_state_create_info.polygonMode = polygon_mode;
	pipeline_rasterization_state_create_info.cullMode = cull_mode;
	pipeline_rasterization_state_create_info.frontFace = front_face;
	pipeline_rasterization_state_create_info.flags = flags;
	pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
	pipeline_rasterization_state_create_info.lineWidth = 1.0f;
	return pipeline_rasterization_state_create_info;
}

VkPipelineColorBlendAttachmentState vkTool::pipeline_color_blend_attachment_state(VkColorComponentFlags color_write_mask, VkBool32 blend_enable)
{
	VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state{};
	pipeline_color_blend_attachment_state.colorWriteMask = color_write_mask;
	pipeline_color_blend_attachment_state.blendEnable = blend_enable;
	return pipeline_color_blend_attachment_state;
}

VkPipelineColorBlendStateCreateInfo vkTool::pipeline_color_blend_state_create_info(uint32_t attachment_count, const VkPipelineColorBlendAttachmentState* attachments)
{
	VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info{};
	pipeline_color_blend_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	pipeline_color_blend_state_create_info.attachmentCount = attachment_count;
	pipeline_color_blend_state_create_info.pAttachments = attachments;
	return pipeline_color_blend_state_create_info;
}

VkPipelineDepthStencilStateCreateInfo vkTool::pipeline_depth_stencil_state_create_info(VkBool32 depth_test_enable, VkBool32 depth_write_enable, VkCompareOp depth_compare_op)
{
	VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info{};
	pipeline_depth_stencil_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	pipeline_depth_stencil_state_create_info.depthTestEnable = depth_test_enable;
	pipeline_depth_stencil_state_create_info.depthWriteEnable = depth_write_enable;
	pipeline_depth_stencil_state_create_info.depthCompareOp = depth_compare_op;
	pipeline_depth_stencil_state_create_info.front = pipeline_depth_stencil_state_create_info.back;
	pipeline_depth_stencil_state_create_info.back.compareOp = VK_COMPARE_OP_ALWAYS;
	return pipeline_depth_stencil_state_create_info;
}

VkPipelineViewportStateCreateInfo vkTool::pipeline_viewport_state_create_info(uint32_t viewport_count, uint32_t scissor_count, VkPipelineViewportStateCreateFlags flags)
{
	VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info{};
	pipeline_viewport_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	pipeline_viewport_state_create_info.viewportCount = viewport_count;
	pipeline_viewport_state_create_info.scissorCount = scissor_count;
	pipeline_viewport_state_create_info.flags = flags;
	return pipeline_viewport_state_create_info;
}

VkPipelineMultisampleStateCreateInfo vkTool::pipeline_multisample_state_create_info(VkSampleCountFlagBits rasterization_samples, VkPipelineMultisampleStateCreateFlags flags)
{
	VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info{};
	pipeline_multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	pipeline_multisample_state_create_info.rasterizationSamples = rasterization_samples;
	pipeline_multisample_state_create_info.flags = flags;
	return pipeline_multisample_state_create_info;
}

VkPipelineDynamicStateCreateInfo vkTool::pipeline_dynamic_state_create_info(const std::vector<VkDynamicState>& dynamic_states, VkPipelineDynamicStateCreateFlags flags)
{
	VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info{};
	pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipeline_dynamic_state_create_info.pDynamicStates = dynamic_states.data();
	pipeline_dynamic_state_create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
	pipeline_dynamic_state_create_info.flags = flags;
	return pipeline_dynamic_state_create_info;
}


VkPipelineDynamicStateCreateInfo vkTool::pipeline_dynamic_state_create_info(const VkDynamicState* dynamic_states, uint32_t dynamicStateCount, VkPipelineDynamicStateCreateFlags flags)
{
	VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info{};
	pipeline_dynamic_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	pipeline_dynamic_state_create_info.pDynamicStates = dynamic_states;
	pipeline_dynamic_state_create_info.dynamicStateCount = dynamicStateCount;
	pipeline_dynamic_state_create_info.flags = flags;
	return pipeline_dynamic_state_create_info;
}

VkShaderModule vkTool::load_shader(const std::string& filename, VkDevice device, VkShaderStageFlagBits stage)
{
	vkb::GLSLCompiler glsl_compiler;

	auto buffer = vkTool::read_binary_file(filename, 0);

	std::string file_ext = filename;

	// Extract extension name from the glsl shader file
	file_ext = file_ext.substr(file_ext.find_last_of(".") + 1);

	std::vector<uint32_t> spirv;
	std::string           info_log;

	// Compile the GLSL source
	if (!glsl_compiler.compile_to_spirv(find_shader_stage(file_ext), buffer, "main", {}, spirv, info_log))
	{
		//LOGE("Failed to compile shader, Error: {}", info_log.c_str());
		return VK_NULL_HANDLE;
	}

	VkShaderModule           shader_module;
	VkShaderModuleCreateInfo module_create_info{};
	module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	module_create_info.codeSize = spirv.size() * sizeof(uint32_t);
	module_create_info.pCode = spirv.data();

	VK_CHECK(vkCreateShaderModule(device, &module_create_info, NULL, &shader_module));

	return shader_module;
}
VkShaderStageFlagBits vkTool::find_shader_stage(const std::string& ext)
{
	if (ext == "vert")
	{
		return VK_SHADER_STAGE_VERTEX_BIT;
	}
	else if (ext == "frag")
	{
		return VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	else if (ext == "comp")
	{
		return VK_SHADER_STAGE_COMPUTE_BIT;
	}
	else if (ext == "geom")
	{
		return VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	else if (ext == "tesc")
	{
		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	else if (ext == "tese")
	{
		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}
	else if (ext == "rgen")
	{
		return VK_SHADER_STAGE_RAYGEN_BIT_KHR;
	}
	else if (ext == "rahit")
	{
		return VK_SHADER_STAGE_ANY_HIT_BIT_KHR;
	}
	else if (ext == "rchit")
	{
		return VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR;
	}
	else if (ext == "rmiss")
	{
		return VK_SHADER_STAGE_MISS_BIT_KHR;
	}
	else if (ext == "rint")
	{
		return VK_SHADER_STAGE_INTERSECTION_BIT_KHR;
	}
	else if (ext == "rcall")
	{
		return VK_SHADER_STAGE_CALLABLE_BIT_KHR;
	}

	throw std::runtime_error("File extension `" + ext + "` does not have a vulkan shader stage.");
}

VkVertexInputBindingDescription vkTool::vertex_input_binding_description(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate)
{

	VkVertexInputBindingDescription vertex_input_binding_description{};
	vertex_input_binding_description.binding = binding;
	vertex_input_binding_description.stride = stride;
	vertex_input_binding_description.inputRate = input_rate;
	return vertex_input_binding_description;
}

VkVertexInputAttributeDescription vkTool::vertex_input_attribute_description(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset)
{
	VkVertexInputAttributeDescription vertex_input_attribute_description{};
	vertex_input_attribute_description.location = location;
	vertex_input_attribute_description.binding = binding;
	vertex_input_attribute_description.format = format;
	vertex_input_attribute_description.offset = offset;
	return vertex_input_attribute_description;
}

VkPipelineVertexInputStateCreateInfo vkTool::pipeline_vertex_input_state_create_info()
{
	VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info{};
	pipeline_vertex_input_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	return pipeline_vertex_input_state_create_info;
}

VkGraphicsPipelineCreateInfo vkTool::pipeline_create_info(VkPipelineLayout layout, VkRenderPass render_pass, VkPipelineCreateFlags flags)
{
	VkGraphicsPipelineCreateInfo pipeline_create_info{};
	pipeline_create_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeline_create_info.layout = layout;
	pipeline_create_info.renderPass = render_pass;
	pipeline_create_info.flags = flags;
	pipeline_create_info.basePipelineIndex = -1;
	pipeline_create_info.basePipelineHandle = VK_NULL_HANDLE;
	return pipeline_create_info;
}

VkDescriptorPoolSize vkTool::descriptor_pool_size(VkDescriptorType type, uint32_t count)
{

	VkDescriptorPoolSize descriptor_pool_size{};
	descriptor_pool_size.type = type;
	descriptor_pool_size.descriptorCount = count;
	return descriptor_pool_size;
}

VkDescriptorPoolCreateInfo vkTool::descriptor_pool_create_info(uint32_t count, VkDescriptorPoolSize* pool_sizes, uint32_t max_sets)
{
	VkDescriptorPoolCreateInfo descriptor_pool_info{};
	descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_info.poolSizeCount = count;
	descriptor_pool_info.pPoolSizes = pool_sizes;
	descriptor_pool_info.maxSets = max_sets;
	return descriptor_pool_info;
}

VkDescriptorPoolCreateInfo vkTool::descriptor_pool_create_info(const std::vector<VkDescriptorPoolSize>& pool_sizes, uint32_t max_sets)
{
	VkDescriptorPoolCreateInfo descriptor_pool_info{};
	descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptor_pool_info.poolSizeCount = static_cast<uint32_t>(pool_sizes.size());
	descriptor_pool_info.pPoolSizes = pool_sizes.data();
	descriptor_pool_info.maxSets = max_sets;
	return descriptor_pool_info;
}

void vkTool::set_image_layout(VkCommandBuffer command_buffer, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, VkImageSubresourceRange subresource_range, VkPipelineStageFlags src_mask, VkPipelineStageFlags dst_mask)
{
	// Create an image barrier object
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.oldLayout = old_layout;
	barrier.newLayout = new_layout;
	barrier.image = image;
	barrier.subresourceRange = subresource_range;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (old_layout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		barrier.srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		barrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source
		// Make sure any reads from the image have been finished
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		barrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (new_layout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from the image have been finished
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		barrier.dstAccessMask = barrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (barrier.srcAccessMask == 0)
		{
			barrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(
		command_buffer,
		src_mask,
		dst_mask,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier);
}

void vkTool::set_image_layout(VkCommandBuffer command_buffer, VkImage image, VkImageAspectFlags aspect_mask, VkImageLayout old_layout, VkImageLayout new_layout, VkPipelineStageFlags src_mask, VkPipelineStageFlags dst_mask)
{
	VkImageSubresourceRange subresource_range = {};
	subresource_range.aspectMask = aspect_mask;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = 1;
	subresource_range.layerCount = 1;
	set_image_layout(command_buffer, image, old_layout, new_layout, subresource_range, src_mask, dst_mask);

}
