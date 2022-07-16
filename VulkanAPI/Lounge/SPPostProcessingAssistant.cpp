#include "SPPostProcessingAssistant.h"
#include "../SPMangaka.h"
SPPostProcessingAssistant::SPPostProcessingAssistant(SPMangaka* boss)
	: boss(boss)
	, manager(boss->s_manager)
{
}

SPPostProcessingAssistant::~SPPostProcessingAssistant()
{
}
