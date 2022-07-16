#include "SPSLight.h"
#include "SPSNode.h"

SPSLight::SPSLight(const std::string& name)
	: SPComponent{ name }
{
}

std::type_index SPSLight::get_type()
{
	return typeid(SPSLight);
}

void SPSLight::set_node(SPSNode& node)
{
	node_ = &node;
}

void SPSLight::set_light_type(const SPSLightType& type)
{
	this->light_type_ = type;
}

void SPSLight::set_properties(const SPSLightProperties& properties)
{
	this->properties_ = properties;
}
