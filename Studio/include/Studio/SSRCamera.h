#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRCAMERA_H_
#define SSR_SSRCAMERA_H_

#undef near
#undef far

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_DEF_BASE(SSRCamera, _SSR_STUDIO_EXPORT);

#define _SSR_CAMERA_PUB_MEMBER_FUNC_DEF()\
    auto set_look_at(const SVEC3& eye, const SVEC3& center, const SVEC3& up) noexcept -> void;\
    auto set_exposure(SFLOAT32 aperture, SFLOAT32 shutterSpeed, SFLOAT32 sensitivity) noexcept -> void;\
    auto set_projection(SFLOAT64 fovInDegrees, SFLOAT64 aspect, SFLOAT64 near, SFLOAT64 far, SSRCamera::Fov direction) noexcept -> void;\
    auto set_projection(SSRCamera::Projection projection, SFLOAT64 left, SFLOAT64 right, SFLOAT64 bottom, SFLOAT64 top, SFLOAT64 near, SFLOAT64 far) noexcept -> void;\
    auto get_eye() const noexcept -> SVEC3;\
    auto get_up() const noexcept -> SVEC3;\

#define _SSR_CAMERA_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_look_at, const SVEC3&, eye, const SVEC3&, center, const SVEC3&, up)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_exposure, SFLOAT32, aperture, SFLOAT32, shutterSpeed, SFLOAT32, sensitivity)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_projection, SFLOAT64, fovInDegrees, SFLOAT64, aspect, SFLOAT64, near, SFLOAT64, far, SSRCamera::Fov, direction)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_projection, SSRCamera::Projection, projection, SFLOAT64, left, SFLOAT64, right, SFLOAT64, bottom, SFLOAT64, top, SFLOAT64, near, SFLOAT64, far)\
    _SSR_CLASS_BASE_FUNC_OUT_CONST_NOEXCEPT(Class, SVEC3, get_eye,,)\
    _SSR_CLASS_BASE_FUNC_OUT_CONST_NOEXCEPT(Class, SVEC3, get_up,,)\

class _SSR_STUDIO_EXPORT SSRCamera
{
public:
    enum class Fov : SENUM8 
    {
        eVertical,
        eHorizontal
    };
    enum class Projection : SENUM8
    {
        ePerspective,
        eOrtho
    };
protected:
    SSRCamera() noexcept = default;
    ~SSRCamera()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRCamera);
    _SSR_CLASS_DELETE_MOVE(SSRCamera);
    _SSR_CAMERA_PUB_MEMBER_FUNC_DEF()

public:
    static auto Create() -> SSRCameraUPtr;
};


_SSR_END

#endif

