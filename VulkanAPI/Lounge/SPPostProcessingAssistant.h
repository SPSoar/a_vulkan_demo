#ifndef S_SPPOSTPROCESSINGASSISTANT_H_
#define S_SPPOSTPROCESSINGASSISTANT_H_
#include "common.h"
class SPMangaka;
class SPStudioManager;
class SPNeMu;
class RHIEXPORT SPPostProcessingAssistant
{
	friend SPNeMu;
public:
	SPPostProcessingAssistant(SPMangaka* boss);
	virtual ~SPPostProcessingAssistant();

protected:


private:
	SPMangaka* boss;
	SPStudioManager* manager;

};

#endif
