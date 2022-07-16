#ifndef S_SPDRAFTVULKAN_H_
#define S_SPDRAFTVULKAN_H_
#include "vulkan_common.h"
#include "../../Interface/SPDraft.h"

class  SPDraftVulkan : public SPDraft
{
public:
	static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
	static VkVertexInputBindingDescription getBindingDescription();
	SPDraftVulkan();
	virtual ~SPDraftVulkan();
	virtual void setPrimitiveTopology(PrimitiveTopology topology) override;
	virtual void setPrimitiveRestartEnable(spbool enable) override;
public:

	VkPrimitiveTopology  topology;
	VkBool32  primitiveRestartEnable;

};	

#endif
