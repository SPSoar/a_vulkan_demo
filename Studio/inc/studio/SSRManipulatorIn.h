#pragma once
#ifndef SSRManipulatorIn
#define SSR_SSRMANIPULATORIN_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

struct Bookmark 
{
    struct OrbitParams
    {
        SFLOAT64 mPhi{};
        SFLOAT64 mTheta{};
        SFLOAT64 mDistance{};
        SVEC3    mPivot{};
    };

    OrbitParams mOrbit{};
};

class SSRManipulatorIn : public SSRManipulator
{
    /** Optional raycasting function to enable perspective-correct panning. */
    using RayCallback = bool (*)(const SVEC3& origin, const SVEC3& dir, const SVEC4& groundPlane, SFLOAT64& t);

    enum GrabState
    { 
        eInactive,
        eOrbiting,
        ePanning
    };
    enum class Fov
    { 
        eVertical,
        eHorizontal
    };
    _SSR_DELETER_FRIEND_DEF(SSRManipulator)
protected:
    SSRManipulatorIn() noexcept;
    ~SSRManipulatorIn()noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRManipulatorIn);
    _SSR_CLASS_DELETE_MOVE(SSRManipulatorIn);
    _SSR_MANIPULATOR_PUB_MEMBER_FUNC_DEF()
protected:
    auto get_current_bookmark() const noexcept -> Bookmark;
    auto jump_to_bookmark(const Bookmark& bookmark) noexcept -> void;
    auto raycast_far_plane(SFLOAT64 x, SFLOAT64 y) const noexcept -> SVEC3;
    auto raycast(SFLOAT64 x, SFLOAT64 y, SVEC3& result) const noexcept -> SBOOL;
    auto get_ray(SFLOAT64 x, SFLOAT64 y, SVEC3& porigin, SVEC3& pdir) const noexcept -> void;
protected:
    SBOOL     mFlipped{ false };

    GrabState mGrabState{ eInactive };
    SVEC3     mGrabPivot{};
    SVEC3     mGrabScene{};
    SVEC3     mGrabFar{};
    SVEC3     mGrabEye{};
    SVEC3     mGrabTarget{};
    SFLOAT64  mGrabWinX{ 0.0 };
    SFLOAT64  mGrabWinY{ 0.0 };
    Bookmark  mGrabBookmark{};

    SVEC3    mEye{ 0.f, 0.f, 10.f };
    SVEC3    mOrbitHomePosition{ 0.f, 0.f, 10.f };

    SVEC3    mTarget{ 0.f, 0.f, 0.f };
    SVEC3    mPivot{ 0.f, 0.f, 0.f };

    SVEC3    mUpVector{ 0.f, 1.f, 0.f };
    SFLOAT64 mViewport[2]{};

    SVEC2    mOrbitSpeed{ 0.01f };
    SFLOAT64 mZoomSpeed{ 0.01 };
    SFLOAT64 mFovDegrees{ 45.0 };
    SFLOAT64 mFarPlane{ 10000.0 };
    Fov      mFovDirection{ Fov::eVertical };

    RayCallback mRayCallback{ nullptr };
    SVEC4       mGroundPlane{};
};

_SSR_END

#endif

