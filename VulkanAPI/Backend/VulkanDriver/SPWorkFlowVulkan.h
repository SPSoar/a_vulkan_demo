#ifndef S_SPWORKFLOWVULKAN_H_
#define S_SPWORKFLOWVULKAN_H_
#include "vulkan_common.h"
#include "../../Interface/SPWorkFlow.h"
class SPVulkanWindowsDriver;
class SPDrawingPad;
class  SPWorkFlowVulkan : public SPWorkFlow
{
public:
	SPWorkFlowVulkan(SPVulkanWindowsDriver* manager, WorkFlowType wfType);
	virtual ~SPWorkFlowVulkan();
	virtual void coloring(SPDrawingPad* drawingPad) override;
public:
	SPVulkanWindowsDriver* s_manager;
};	

#endif
