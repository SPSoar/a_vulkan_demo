#include "SPSTransform.h"

#include <glm/gtx/matrix_decompose.hpp>

SPSTransform::SPSTransform(SPSNode& node)
	: node_ { node }
{
}

std::type_index SPSTransform::get_type()
{
	return typeid(SPSTransform);
}

void SPSTransform::invalidate_world_matrix()
{
	update_world_matrix_ = true;
}

void SPSTransform::set_translation(const glm::vec3& translation)
{
	translation_ = translation;
	invalidate_world_matrix();
}

void SPSTransform::set_rotation(const glm::quat& rotation)
{
	rotation_ = rotation;
	invalidate_world_matrix();
}

void SPSTransform::set_scale(const glm::vec3& scale)
{
	scale_ = scale;
	invalidate_world_matrix();
}

void SPSTransform::set_matrix(const glm::mat4& matrix)
{
	glm::vec3 skew;
	glm::vec4 perspective;
	glm::decompose(matrix, scale_, rotation_, translation_, skew, perspective);
	rotation_ = glm::conjugate(rotation_);

	invalidate_world_matrix();
}
