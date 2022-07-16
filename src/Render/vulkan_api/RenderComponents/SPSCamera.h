#ifndef S_SPSCAMERA_H_
#define S_SPSCAMERA_H_

#include "SPComponent.h"

enum class Fov : uint8_t 
{
	VERTICAL,
	HORIZONTAL
};

enum class Projection : uint8_t
{
	PERSPECTIVE,
	ORTHO 
};

class SPSNode;
class SPPCamera : public SPComponent
{
public:
	static spm_dmat4 Look_At(const spv_vec3& eye, const spv_vec3& center, const spv_vec3& up);
	static spm_dmat4 Frustum(spd_decimal left, spd_decimal right, spd_decimal bottom, spd_decimal top, spd_decimal z_near, spd_decimal z_far);
	static spm_dmat4 Ortho(spd_decimal left, spd_decimal right, spd_decimal bottom, spd_decimal top, spd_decimal z_near, spd_decimal z_far);
	static spm_dmat4 Perspective(spd_decimal fovy, spd_decimal aspect, spd_decimal zNear, spd_decimal zFar);
	static spv_vec3  Rotate_Vector(spd_decimal rx, spd_decimal ry, const spv_vec3& v);
public:
	SPPCamera(const std::string& name);
	~SPPCamera() = default;
	virtual std::type_index get_type() override;
public:
	void set_node(SPSNode& node);
	void set_near_plane(float znear);
	void set_far_plane(float zfar);
	void set_aspect_ratio(float aspect_ratio);
	void set_field_of_view(float fov);
	//1111111111111111111111111
	void track(const spv_vec2& delta);
	void dolly(spd_decimal delta, spd_decimal dolly_speed = 5.0);
	void rotate(const spv_vec2& delta, spd_decimal rotate_speed = 7.0);
public:
	void look_at(const spv_vec3& eye, const spv_vec3& center, const spv_vec3& up);
	void set_projection(spd_decimal fov_in_degrees, spd_decimal aspect, spd_decimal z_near, spd_decimal z_far, Fov direction = Fov::VERTICAL);
	void update_aspect_ratio(spd_decimal aspect);
private:
	void set_model_matrix(const spm_dmat4& view);
	void update_camera_transform();
public:
	Fov direction_;
	Projection projection_type_;

	spm_dmat4 model_matrix_;;
	spm_dmat4 projection_;

	spd_decimal near_;
	spd_decimal far_;
	spd_decimal fov_;
	spd_decimal aspect_ratio_;
	//------------------------------------------------
	spv_vec3 eye_;
	spv_vec3 center_;
	spv_vec3 rotation_;
	//------------------
	sp_bool update_;
private:
	SPSNode* node_;
};
#endif