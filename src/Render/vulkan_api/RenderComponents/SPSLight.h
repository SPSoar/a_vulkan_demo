#ifndef S_SPSLIGHT_H_
#define S_SPSLIGHT_H_

#include "SPComponent.h"

enum class SPSLightType : uint8_t
{
	Directional = 0,
	Point = 1,
	Spot = 2,
	// Insert new lightype here
	Max
};

struct SPSLightProperties
{
	glm::vec3 direction_{ 0.0f, 0.0f, -1.0f };

	glm::vec3 color_{ 1.0f, 1.0f, 1.0f };

	float intensity_{ 1.0f };

	float range_{ 0.0f };

	float inner_cone_angle_{ 0.0f };

	float outer_cone_angle_{ 0.0f };
};


class SPSNode;
class SPSLight : public SPComponent
{
public:
	SPSLight(const std::string& name);
	SPSLight(SPSLight&& other) = default;
	virtual ~SPSLight() = default;
	virtual std::type_index get_type() override;
public:
	void set_node(SPSNode& node);
	void set_light_type(const SPSLightType& type);
	void set_properties(const SPSLightProperties& properties);
private:
	SPSNode* node_;
	SPSLightType light_type_;
	SPSLightProperties properties_;
};

#endif