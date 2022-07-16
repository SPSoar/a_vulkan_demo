#include "SPComponent.h"

SPComponent::SPComponent(const std::string& name)
	: name_{ name }
{
}

const std::string& SPComponent::get_name() const
{
	return name_;
}
