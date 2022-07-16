#include "SPSScript.h"

SPSScript::SPSScript(const std::string& name)
{
}

std::type_index SPSScript::get_type()
{
	return typeid(SPSScript);
}
