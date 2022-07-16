#ifndef S_SPWORKFLOW_H_
#define S_SPWORKFLOW_H_

class SPStudioManager;
class SPDrawingPad;
class RHIEXPORT SPWorkFlow
{
	
public:
	SPWorkFlow(WorkFlowType wfType) 
		:s_wfType(wfType) {}
	virtual ~SPWorkFlow() = default;
	virtual void coloring(SPDrawingPad* drawingPad) = 0;
protected:
	WorkFlowType s_wfType;
};

#endif
