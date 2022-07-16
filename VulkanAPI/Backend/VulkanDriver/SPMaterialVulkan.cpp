#include "SPMaterialVulkan.h"
#include "SPSamplerVulkan.h"
#include "SPShaderVulkan.h"
SPMaterialVulkan::SPMaterialVulkan(SPVulkanWindowsDriver& driver)
    : SPMaterial()
    , s_manager(driver)
{

}

SPMaterialVulkan::~SPMaterialVulkan()
{

}

void SPMaterialVulkan::setDescriptorImageInfo(VkDescriptorImageInfo& imageInfo)
{
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    SPSamplerVulkan* samplerBuffer = static_cast<SPSamplerVulkan*>(textures[0].textureSamplerBuffer);
    imageInfo.imageView = samplerBuffer->textureImageView;
    imageInfo.sampler = samplerBuffer->textureSampler;
}

void SPMaterialVulkan::setShaderStageCreateInfo(VkPipelineShaderStageCreateInfo& shaderStageInfo, ShaderType type)
{
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	switch (type)
	{
	case Vertex:
		shaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		shaderStageInfo.module = static_cast<SPShaderVulkan*>(vertexShaderBuffer)->shaderModule;
		break;
	case Fragment:
		shaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		shaderStageInfo.module = static_cast<SPShaderVulkan*>(fragmentShaderBuffer)->shaderModule;;
		break;
	default:
		break;
	}
	shaderStageInfo.pName = "main";
}

