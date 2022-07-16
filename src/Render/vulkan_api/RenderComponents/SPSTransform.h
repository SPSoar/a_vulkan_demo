#ifndef S_SPSTRANSFORM_H_
#define S_SPSTRANSFORM_H_

#include "SPComponent.h"

#include <glm/gtc/quaternion.hpp>


class SPSNode;
class SPSTransform : public SPComponent
{
public:
	SPSTransform(SPSNode& node);
	virtual ~SPSTransform() = default;
	virtual std::type_index get_type() override;
public:
	void invalidate_world_matrix();
	void set_translation(const glm::vec3& translation);
	void set_rotation(const glm::quat& rotation);
	void set_scale(const glm::vec3& scale);
	void set_matrix(const glm::mat4& matrix);
private:
	SPSNode& node_;
	bool update_world_matrix_ = false;
	glm::vec3 translation_ = glm::vec3(0.0, 0.0, 0.0);
	glm::quat rotation_ = glm::quat(1.0, 0.0, 0.0, 0.0);
	glm::vec3 scale_ = glm::vec3(1.0, 1.0, 1.0);

};

#endif