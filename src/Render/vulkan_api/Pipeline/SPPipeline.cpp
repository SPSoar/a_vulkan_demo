//#include "SPPipeline.h"
//#include "vk_tool/glsl_compiler.h"
//SPPipeline::SPPipeline(Context& context)
//	: context_(context)
//{
//
//}
//
//void SPPipeline::init()
//{
//	// Create a blank pipeline layout.
//	// We are not binding any resources to the pipeline in this first sample.
//	VkPipelineLayoutCreateInfo layout_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };
//	VK_CHECK(vkCreatePipelineLayout(context_.device, &layout_info, nullptr, &context_.pipeline_layout));
//
//	VkPipelineVertexInputStateCreateInfo vertex_input{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };
//
//	// Specify we will use triangle lists to draw geometry.
//	VkPipelineInputAssemblyStateCreateInfo input_assembly{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
//	input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
//
//	// Specify rasterization state.
//	VkPipelineRasterizationStateCreateInfo raster{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
//	raster.cullMode = VK_CULL_MODE_BACK_BIT;
//	raster.frontFace = VK_FRONT_FACE_CLOCKWISE;
//	raster.lineWidth = 1.0f;
//
//	// Our attachment will write to all color channels, but no blending is enabled.
//	VkPipelineColorBlendAttachmentState blend_attachment{};
//	blend_attachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
//
//	VkPipelineColorBlendStateCreateInfo blend{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
//	blend.attachmentCount = 1;
//	blend.pAttachments = &blend_attachment;
//
//	// We will have one viewport and scissor box.
//	VkPipelineViewportStateCreateInfo viewport{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
//	viewport.viewportCount = 1;
//	viewport.scissorCount = 1;
//
//	// Disable all depth testing.
//	VkPipelineDepthStencilStateCreateInfo depth_stencil{ VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO };
//
//	// No multisampling.
//	VkPipelineMultisampleStateCreateInfo multisample{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };
//	multisample.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
//
//	// Specify that these states will be dynamic, i.e. not part of pipeline state object.
//	std::array<VkDynamicState, 2> dynamics{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
//
//	VkPipelineDynamicStateCreateInfo dynamic{ VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO };
//	dynamic.pDynamicStates = dynamics.data();
//	dynamic.dynamicStateCount = sp::to_u32(dynamics.size());
//
//	// Load our SPIR-V shaders.
//	std::array<VkPipelineShaderStageCreateInfo, 2> shader_stages{};
//
//	// Vertex stage of the pipeline
//	shader_stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	shader_stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
//	shader_stages[0].module = load_shader_module(context_, "..\\..\\shaders\\triangle.vert");
//	shader_stages[0].pName = "main";
//
//	// Fragment stage of the pipeline
//	shader_stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
//	shader_stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
//	shader_stages[1].module = load_shader_module(context_, "..\\..\\shaders\\triangle.frag");
//	shader_stages[1].pName = "main";
//
//	VkGraphicsPipelineCreateInfo pipe{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
//	pipe.stageCount = sp::to_u32(shader_stages.size());
//	pipe.pStages = shader_stages.data();
//	pipe.pVertexInputState = &vertex_input;
//	pipe.pInputAssemblyState = &input_assembly;
//	pipe.pRasterizationState = &raster;
//	pipe.pColorBlendState = &blend;
//	pipe.pMultisampleState = &multisample;
//	pipe.pViewportState = &viewport;
//	pipe.pDepthStencilState = &depth_stencil;
//	pipe.pDynamicState = &dynamic;
//
//	// We need to specify the pipeline layout and the render pass description up front as well.
//	pipe.renderPass = context_.render_pass;
//	pipe.layout = context_.pipeline_layout;
//
//	VK_CHECK(vkCreateGraphicsPipelines(context_.device, VK_NULL_HANDLE, 1, &pipe, nullptr, &context_.pipeline));
//
//	// Pipeline is baked, we can delete the shader modules now.
//	vkDestroyShaderModule(context_.device, shader_stages[0].module, nullptr);
//	vkDestroyShaderModule(context_.device, shader_stages[1].module, nullptr);
//}
//
//VkShaderModule SPPipeline::load_shader_module(Context& context, const char* path)
//{
//	vkb::GLSLCompiler glsl_compiler;
//
//	auto buffer = vkTool::read_binary_file(path, 0);
//
//	std::string file_ext = path;	
//
//	// Extract extension name from the glsl shader file
//	file_ext = file_ext.substr(file_ext.find_last_of(".") + 1);
//
//	std::vector<uint32_t> spirv;
//	std::string           info_log;
//
//	// Compile the GLSL source
//	if (!glsl_compiler.compile_to_spirv(find_shader_stage(file_ext), buffer, "main", {}, spirv, info_log))
//	{
//		SPError(info_log.c_str());
//		return VK_NULL_HANDLE;
//	}
//
//	VkShaderModuleCreateInfo module_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
//	module_info.codeSize = spirv.size() * sizeof(uint32_t);
//	module_info.pCode = spirv.data();
//
//	VkShaderModule shader_module;
//	VK_CHECK(vkCreateShaderModule(context.device, &module_info, nullptr, &shader_module));
//
//	return shader_module;
//}
//
//VkShaderStageFlagBits SPPipeline::find_shader_stage(const std::string& ext)
//{
//
//	if (ext == "vert")
//	{
//		return VK_SHADER_STAGE_VERTEX_BIT;
//	}
//	else if (ext == "frag")
//	{
//		return VK_SHADER_STAGE_FRAGMENT_BIT;
//	}
//	else if (ext == "comp")
//	{
//		return VK_SHADER_STAGE_COMPUTE_BIT;
//	}
//	else if (ext == "geom")
//	{
//		return VK_SHADER_STAGE_GEOMETRY_BIT;
//	}
//	else if (ext == "tesc")
//	{
//		return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
//	}
//	else if (ext == "tese")
//	{
//		return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
//	}
//
//	throw std::runtime_error("No Vulkan shader stage found for the file extension name.");
//	return VK_SHADER_STAGE_FLAG_BITS_MAX_ENUM;
//}
