#include "SPSCamera.h"

spm_dmat4 SPPCamera::Look_At(const spv_vec3& eye, const spv_vec3& center, const spv_vec3& up)
{
	spv_vec3 f = (center - eye).normalized();
	spv_vec3 s = ((f.cross(up.normalized())).normalized());
	spv_vec3 u = s.cross(f);

	spm_dmat4 view = spm_dmat4::Identity();
	view.row(0) <<  s.x(),  s.y(),  s.z(), -s.dot(eye);
	view.row(1) <<  u.x(),  u.y(),  u.z(), -u.dot(eye);
	view.row(2) << -f.x(), -f.y(), -f.z(),  f.dot(eye);

	return view;
}

spm_dmat4 SPPCamera::Frustum(spd_decimal left, spd_decimal right, spd_decimal bottom, spd_decimal top, spd_decimal z_near, spd_decimal z_far)
{
	spm_dmat4 m = spm_dmat4::Zero();
	m.coeffRef(0, 0) = (2 * z_near) / (right - left);
	m.coeffRef(1, 1) = (2 * z_near) / (top - bottom);
	m.coeffRef(0, 2) = (right + left) / (right - left);
	m.coeffRef(1, 2) = (top + bottom) / (top - bottom);
	m.coeffRef(3, 2) = -1;
	m.coeffRef(3, 3) = 0;

	m.coeffRef(2, 2) = z_far / (z_near - z_far);
	m.coeffRef(2, 3) = -(z_far * z_near) / (z_far - z_near);

	return m;
}

spm_dmat4 SPPCamera::Ortho(spd_decimal left, spd_decimal right, spd_decimal bottom, spd_decimal top, spd_decimal z_near, spd_decimal z_far)
{
	spm_dmat4 m = spm_dmat4::Zero();

	m.coeffRef(0, 0) = static_cast<spd_decimal>(2) / (right - left);
	m.coeffRef(1, 1) = static_cast<spd_decimal>(2) / (top - bottom);
	m.coeffRef(0, 3) = -(right + left) / (right - left);
	m.coeffRef(1, 3) = -(top + bottom) / (top - bottom);

	m.coeffRef(2, 2) = -static_cast<spd_decimal>(1) / (z_far - z_near);
	m.coeffRef(2, 3) = -z_near / (z_far - z_near);

	return m;
}

spm_dmat4 SPPCamera::Perspective(spd_decimal fovy, spd_decimal aspect, spd_decimal zNear, spd_decimal zFar)
{
	spm_dmat4 m = spm_dmat4::Zero();
	spd_decimal const tan_half_fovy = std::tan(fovy / static_cast<spd_decimal>(2));

	m.coeffRef(0, 0) = static_cast<spd_decimal>(1) / (aspect * tan_half_fovy);
	m.coeffRef(1, 1) = static_cast<spd_decimal>(1) / (tan_half_fovy);
	m.coeffRef(3, 2) = -static_cast<spd_decimal>(1);

	m.coeffRef(2, 2) = zNear / (zFar - zNear);
	m.coeffRef(2, 3) = -(zNear * zFar) / (zNear - zFar);

	return m;
}

spv_vec3 SPPCamera::Rotate_Vector(spd_decimal rx, spd_decimal ry, const spv_vec3& v)
{
	Eigen::AngleAxis rotate_y(ry, spv_vec3{ 0, 1, 0 });
	Eigen::AngleAxis rotate_x(rx, spv_vec3{ 1, 0, 0 });

	return rotate_x.matrix() * rotate_y.matrix() * v.transpose();
}

SPPCamera::SPPCamera(const std::string& name)
	: SPComponent{ name }
	, model_matrix_{ spm_dmat4::Identity() }
	, projection_{ spm_dmat4::Zero() }
	, projection_type_{ Projection ::PERSPECTIVE}
	, direction_{ Fov::VERTICAL}
	, eye_{spv_vec3::Zero()}
	, rotation_{ spv_vec3::Zero() }
	, center_{ spv_vec3::Zero() }
	, update_{true}
{

}

void SPPCamera::set_projection(spd_decimal fov_in_degrees, spd_decimal aspect, spd_decimal z_near, spd_decimal z_far, Fov direction)
{
	fov_ = fov_in_degrees;
	direction_ = direction;
	projection_type_ = Projection::PERSPECTIVE;
	near_ = z_near;
	far_ = z_far;
	projection_ = SPPCamera::Perspective((spd_decimal) (fov_in_degrees * (std::numbers::pi / 180.0)), aspect, z_near, z_far);
	update_camera_transform();
}

void SPPCamera::look_at(const spv_vec3& eye, const spv_vec3& center, const spv_vec3& up)
{
	eye_ = eye;
	center_ = center;
	rotation_ = spv_vec3::Zero();
	update_camera_transform();
}

void SPPCamera::update_aspect_ratio(spd_decimal aspect)
{
	set_projection(fov_, aspect, near_, far_, direction_);
}
void SPPCamera::set_model_matrix(const spm_dmat4& view)
{
	model_matrix_ = view;
}

void SPPCamera::update_camera_transform()
{
	update_ = true;

	Eigen::AngleAxis rotate_x(rotation_.x(), spv_vec3(1, 0, 0));
	Eigen::AngleAxis rotate_y(rotation_.y(), spv_vec3(0, 1, 0));
	Eigen::AngleAxis rotate_z(rotation_.z(), spv_vec3(0, 0, 1));

	spm_dmat3 rotation_marix = rotate_x.matrix() * rotate_y.matrix() * rotate_z.matrix();
	Eigen::Affine3f affine = Eigen::Affine3f::Identity();
	affine.rotate(rotation_marix);
	affine.translation()[0] = -eye_.x();
	affine.translation()[1] = -eye_.y();
	affine.translation()[2] = -eye_.z();

	set_model_matrix(affine.matrix());
}

std::type_index SPPCamera::get_type()
{
	return typeid(SPPCamera);
	
}

void SPPCamera::set_node(SPSNode& node)
{
	node_ = &node;
}

void SPPCamera::set_near_plane(float znear)
{
	near_ = znear;
}

void SPPCamera::set_far_plane(float zfar)
{
	far_ = zfar;
}

void SPPCamera::set_aspect_ratio(float aspect_ratio)
{
	aspect_ratio_ = aspect_ratio;
}

void SPPCamera::set_field_of_view(float fov)
{
	fov_ = fov;
}

void SPPCamera::track(const spv_vec2& delta)
{
	//spv_vec3 d_s = SPPCamera::Rotate_Vector(rotation_.x(), rotation_.y(), { 1.0, 0.0, 0.0 });
	//spv_vec3 d_t = SPPCamera::Rotate_Vector(rotation_.x(), rotation_.y(), { 0.0, 1.0, 0.0 });

	//double mult_t = 2.0 * center_of_interest_ * std::tan(radians(mFovx) / 2.0);
	//double mult_s = mult_t / mWidth;
	//mult_t /= mHeight;

	//double s = mult_s * delta.x;
	//double t = -mult_t * delta.y;

	//mTranslation += (s * d_s) + (t * d_t);
	//updateCameraTransform();
}

void SPPCamera::dolly(spd_decimal delta, spd_decimal dolly_speed)
{
	spv_vec3 v = (center_ - eye_).normalized();
	eye_ = eye_ + (delta * dolly_speed * v);

	update_camera_transform();
}


void SPPCamera::rotate(const spv_vec2& delta, spd_decimal rotate_speed)
{
	rotation_.y() -= (rotate_speed * delta.x());
	rotation_.x() += (rotate_speed * delta.y());

	update_camera_transform();
}


