#ifndef S_SPSSCRIPT_H_
#define S_SPSSCRIPT_H_

#include "SPComponent.h"

class SPSScript : public SPComponent
{
public:
	SPSScript(const std::string& name = "");
	virtual ~SPSScript() = default;
	virtual std::type_index get_type() override;
};

#endif