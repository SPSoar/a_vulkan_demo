#ifndef S_SPSFRUSTUM_H_
#define S_SPSFRUSTUM_H_

#include "SPSTransform.h"

enum Side
{
	LEFT = 0,
	RIGHT = 1,
	TOP = 2,
	BOTTOM = 3,
	BACK = 4,
	FRONT = 5
};

class SPSFrustum
{
public:
	void update(const spm_dmat4& matrix);
	bool check_sphere(glm::vec3 pos, float radius);
	const std::array<glm::vec4, 6>& get_planes() const;
private:
	std::array<glm::vec4, 6> planes_;
};

#endif