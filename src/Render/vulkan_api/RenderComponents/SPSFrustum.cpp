#include "SPSFrustum.h"

void SPSFrustum::update(const spm_dmat4& matrix)
{
	planes_[LEFT].x = matrix(0, 3) + matrix(0, 0);
	planes_[LEFT].y = matrix(1, 3) + matrix(1, 0);
	planes_[LEFT].z = matrix(2, 3) + matrix(2, 0);
	planes_[LEFT].w = matrix(3, 3) + matrix(3, 0);

	planes_[RIGHT].x = matrix(0, 3) - matrix(0, 0);
	planes_[RIGHT].y = matrix(1, 3) - matrix(1, 0);
	planes_[RIGHT].z = matrix(2, 3) - matrix(2, 0);
	planes_[RIGHT].w = matrix(3, 3) - matrix(3, 0);

	planes_[TOP].x = matrix(0, 3) - matrix(0, 1);
	planes_[TOP].y = matrix(1, 3) - matrix(1, 1);
	planes_[TOP].z = matrix(2, 3) - matrix(2, 1);
	planes_[TOP].w = matrix(3, 3) - matrix(3, 1);

	planes_[BOTTOM].x = matrix(0, 3) + matrix(0, 1);
	planes_[BOTTOM].y = matrix(1, 3) + matrix(1, 1);
	planes_[BOTTOM].z = matrix(2, 3) + matrix(2, 1);
	planes_[BOTTOM].w = matrix(3, 3) + matrix(3, 1);

	planes_[BACK].x = matrix(0, 3) + matrix(0, 2);
	planes_[BACK].y = matrix(1, 3) + matrix(1, 2);
	planes_[BACK].z = matrix(2, 3) + matrix(2, 2);
	planes_[BACK].w = matrix(3, 3) + matrix(3, 2);

	planes_[FRONT].x = matrix(0, 3) - matrix(0, 2);
	planes_[FRONT].y = matrix(1, 3) - matrix(1, 2);
	planes_[FRONT].z = matrix(2, 3) - matrix(2, 2);
	planes_[FRONT].w = matrix(3, 3) - matrix(3, 2);

	for (size_t i = 0; i < planes_.size(); i++)
	{
		float length = sqrtf(planes_[i].x * planes_[i].x + planes_[i].y * planes_[i].y + planes_[i].z * planes_[i].z);
		planes_[i] /= length;
	}
}

bool SPSFrustum::check_sphere(glm::vec3 pos, float radius)
{
	for (size_t i = 0; i < planes_.size(); i++)
	{
		if ((planes_[i].x * pos.x) + (planes_[i].y * pos.y) + (planes_[i].z * pos.z) + planes_[i].w <= -radius)
		{
			return false;
		}
	}
	return true;
}

const std::array<glm::vec4, 6>& SPSFrustum::get_planes() const
{
	return planes_;
}
