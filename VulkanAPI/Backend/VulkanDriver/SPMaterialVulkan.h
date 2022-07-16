#ifndef S_SPMATERIALVULKAN_H_
#define S_SPMATERIALVULKAN_H_
#include "SPVulkanWindowsDriver.h"
#include "../../Resource/SPMaterial.h"
class  SPMaterialVulkan : public SPMaterial
{
public:
	SPMaterialVulkan(SPVulkanWindowsDriver& driver);
	virtual ~SPMaterialVulkan();
	void setDescriptorImageInfo(VkDescriptorImageInfo& imageInfo);
	void setShaderStageCreateInfo(VkPipelineShaderStageCreateInfo& shaderStageInfo, ShaderType type);
protected:

private:
	SPVulkanWindowsDriver& s_manager;
public:

};

#endif
