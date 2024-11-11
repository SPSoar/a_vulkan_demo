#include "studio/SSRCameraIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRCamera)

_SSR_CAMERA_PUB_MEMBER_FUNC_IMPL(SSRCamera)

namespace 
{
    constexpr SFLOAT32 SCCV_Min_Aperture = 0.5f;
    constexpr SFLOAT32 SCCV_Max_Aperture = 64.0f;
    constexpr SFLOAT32 SCCV_Min_Shutter_Speed = 1.0f / 25000.0f;
    constexpr SFLOAT32 SCCV_Max_Shutter_Speed = 60.0f;
    constexpr SFLOAT32 SCCV_Min_Sensitivity = 10.0f;
    constexpr SFLOAT32 SCCV_Max_Sensitivity = 204800.0f;
}

auto SSRCamera::Create() -> SSRCameraUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRCamera);
}

//SSRCameraIn::SSRCameraIn() noexcept
//{
//
//}

SSRCameraIn::~SSRCameraIn() noexcept
{

}

auto SSRCameraIn::set_look_at(const SVEC3& eye, const SVEC3& center, const SVEC3& up) noexcept -> void
{
    mLocalTransform = SMAT4X4::Look_At(eye, center, up);
}

auto SSRCameraIn::set_exposure(SFLOAT32 aperture, SFLOAT32 shutterSpeed, SFLOAT32 sensitivity) noexcept -> void
{
    mAperture     = _SSR_STD::clamp(aperture,     SCCV_Min_Aperture,      SCCV_Max_Aperture);
    mShutterSpeed = _SSR_STD::clamp(shutterSpeed, SCCV_Min_Shutter_Speed, SCCV_Max_Shutter_Speed);
    mSensitivity  = _SSR_STD::clamp(sensitivity,  SCCV_Min_Sensitivity,   SCCV_Max_Sensitivity);
}

auto SSRCameraIn::set_projection(SFLOAT64 fovInDegrees, 
                                 SFLOAT64 aspect, 
                                 SFLOAT64 near, 
                                 SFLOAT64 far, 
                                 SSRCamera::Fov direction) noexcept -> void
{
    
    SFLOAT64 w;
    SFLOAT64 h;
    SFLOAT64 s = _SSR_STD::tan(fovInDegrees * _SSR_STD::numbers::pi / 180.0 / 2.0) * near;
    if (direction == Fov::eVertical)
    {
        w = s * aspect;
        h = s;
    }
    else 
    {
        w = s;
        h = s / aspect;
    }
    set_projection(Projection::ePerspective, -w, w, -h, h, near, far);
}

auto SSRCameraIn::set_projection(SSRCamera::Projection projection, 
                                 SFLOAT64 left, 
                                 SFLOAT64 right, 
                                 SFLOAT64 bottom, 
                                 SFLOAT64 top, 
                                 SFLOAT64 near, 
                                 SFLOAT64 far) noexcept -> void
{
    _SSR_ASSERT(!(
        left == right ||
        bottom == top ||
        (projection == Projection::ePerspective && (near <= 0 || far <= near)) ||
        (projection == Projection::eOrtho && (near == far))), _SSR_RDERROR);
    //with far plane
    SMAT4X4 projectionForCulling{};
    //infinite far
    SMAT4X4 p{};
    switch (projection) 
    {
    case Projection::ePerspective:
        /*
         * The general perspective projection in Vulkan convention looks like this:
         *
         * P =  2N/r-l    0      r+l/r-l        0
         *       0      2N/t-b   t+b/t-b        0
         *       0        0       -F/F-N      -F/F-N
         *       0        0        -1           0
         */
        projectionForCulling = SMAT4X4::Frustum(left, right, bottom, top, near, far);
        p = projectionForCulling;

        /*
         * but we're using a far plane at infinity
         *
         * P =  2N/r-l      0    r+l/r-l        0
         *       0      2N/t-b   t+b/t-b        0
         *       0       0         -1        -2*N    <-- far at infinity
         *       0       0         -1           0
         */
        p(2, 2) = -1.0f;                           // lim(far->inf) = -1
        p(2, 3) = -2.0f * cast<SFLOAT32>(near);    // lim(far->inf) = -2*near
        break;

    case Projection::eOrtho:
        /*
         * The general orthographic projection in Vulkan convention looks like this:
         *
         * P =  2/r-l    0         0       - r+l/r-l
         *       0      2/t-b      0       - t+b/t-b
         *       0       0       -1/F-N    -   N/F-N
         *       0       0         0            1
         */
        projectionForCulling = SMAT4X4::Ortho(left, right, bottom, top, near, far);
        p = projectionForCulling;
        break;
    }
    set_custom_projection(p, projectionForCulling, near, far);
}
auto SSRCameraIn::get_view_matrix() const noexcept -> const SMAT4X4&
{
    return mLocalTransform;
}

auto SSRCameraIn::get_projection_matrix() const noexcept -> const SMAT4X4&
{
    return mProjection;
}
auto SSRCameraIn::get_culling_projection_matrix() const noexcept -> const SMAT4X4&
{
    return mProjectionForCulling;
}
auto SSRCameraIn::set_custom_projection(const SMAT4X4& projection, const SMAT4X4& projectionForCulling, SFLOAT64 near, SFLOAT64 far) noexcept -> void
{
    mProjection = projection;
    mProjectionForCulling = projectionForCulling;
    mNear = near;
    mFar = far;
}

auto SSRCameraIn::get_eye() const noexcept -> SVEC3
{
    return SVEC3(get_view_matrix()(3, 0), get_view_matrix()(3, 1), get_view_matrix()(3, 2));
}

auto SSRCameraIn::get_up() const noexcept -> SVEC3
{
    return SVEC3(get_view_matrix()(1, 0), get_view_matrix()(1, 1), get_view_matrix()(1, 2)).normalize();
}
_SSR_END