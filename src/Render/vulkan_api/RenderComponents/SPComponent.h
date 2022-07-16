#ifndef S_SPCOMPONENT_H_
#define S_SPCOMPONENT_H_

#include "../vulkan_common.h"

#include <typeindex>

class SPComponent
{
public:
	SPComponent() = default;

	SPComponent(const std::string& name);

	SPComponent(SPComponent&& other) = default;

	virtual ~SPComponent() = default;

	const std::string& get_name() const;

	virtual std::type_index get_type() = 0;

private:
	std::string name_;
};

#endif