#pragma once
#ifndef SSR_SSRCAMERAIN_H_
#define SSR_SSRCAMERAIN_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRCameraIn);

class SSRCameraIn : public SSRCamera
{
    _SSR_DELETER_FRIEND_DEF(SSRCamera)
protected:
    SSRCameraIn() = default;
    //explicit SSRCameraIn() noexcept;
    ~SSRCameraIn()noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRCameraIn);
    _SSR_CLASS_DELETE_MOVE(SSRCameraIn);
    _SSR_CAMERA_PUB_MEMBER_FUNC_DEF()
public:
    auto get_view_matrix() const noexcept -> const SMAT4X4&;
    auto get_projection_matrix() const noexcept -> const SMAT4X4&;
    auto get_culling_projection_matrix() const noexcept -> const SMAT4X4&;
protected:
    auto set_custom_projection(const SMAT4X4& projection, const SMAT4X4& projectionForCulling, SFLOAT64 near, SFLOAT64 far) noexcept -> void;
private:
    SMAT4X4  mLocalTransform{};

    SFLOAT32 mAperture{ 16.0f };
    SFLOAT32 mShutterSpeed{ 1.0f / 125.0f };
    SFLOAT32 mSensitivity{ 100.0f };

    SMAT4X4  mProjection{};
    SMAT4X4  mProjectionForCulling{};
    SFLOAT64 mNear{ 0.0 };
    SFLOAT64 mFar{ 0.0 };

};

_SSR_END

#endif

