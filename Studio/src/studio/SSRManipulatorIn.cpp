#include "studio/SSRManipulatorIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRManipulator)

_SSR_MANIPULATOR_PUB_MEMBER_FUNC_IMPL(SSRManipulator)

namespace 
{
    auto raycast_plane(const SVEC3& origin, const SVEC3& dir, const SVEC4& groundPlane, SFLOAT64& t) noexcept -> bool
    {
        const SVEC3 n     = SVEC3(groundPlane[0], groundPlane[1], groundPlane[2]);
        const SVEC3 p0    = n * groundPlane[3];
        const SFLOAT64 denom = -n.dot(dir);
        if (denom > 1e-6) 
        {
            const SVEC3 p0l0 = p0 - origin;
            t = p0l0.dot(n) / -denom;
            return t >= 0;
        }
        return false;
    }
}

auto SSRManipulator::Create() -> SSRManipulatorUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRManipulator);
}

SSRManipulatorIn::SSRManipulatorIn() noexcept
{

}

SSRManipulatorIn::~SSRManipulatorIn() noexcept
{

}

auto SSRManipulatorIn::get_look_at(SVEC3& eyePosition, SVEC3& targetPosition, SVEC3& upward) const noexcept -> void
{
    targetPosition = mTarget;
    eyePosition    = mEye;

    auto gaze = (mTarget - mEye).normalize();
    auto right = gaze.cross(mUpVector);
    upward = right.cross(gaze);
}

auto SSRManipulatorIn::set_target_position(const SVEC3& position) noexcept -> void
{
    mTarget = position;
    mPivot  = mTarget;

    auto d = mTarget.length();
    const SVEC3 n = (mOrbitHomePosition - mTarget).normalize();
    mGroundPlane = SVEC4(n, cast<SVEC4::ValueType>(-d));
}
auto SSRManipulatorIn::set_eye(const SVEC3& eye) noexcept -> void
{
    mEye = eye;
    mOrbitHomePosition = eye;

    auto d = mTarget.length();
    const SVEC3 n = (mOrbitHomePosition - mTarget).normalize();
    mGroundPlane = SVEC4(n, -d);

}
auto SSRManipulatorIn::set_up(const SVEC3& up) noexcept -> void
{
    mUpVector = up;
}
auto SSRManipulatorIn::set_viewport(SFLOAT64 width, SFLOAT64 height) noexcept -> void
{
    mViewport[0] = width;
    mViewport[1] = height;
}
auto SSRManipulatorIn::grab_begin(SFLOAT64 x, SFLOAT64 y, SBOOL strafe) noexcept -> void
{
    mGrabState = strafe ? ePanning : eOrbiting;
    mGrabPivot = mPivot;
    mGrabEye = mEye;
    mGrabTarget = mTarget;
    mGrabWinX = x;
    mGrabWinY = y;
    mGrabBookmark = get_current_bookmark();
    mGrabFar = raycast_far_plane(x, y);
    raycast(x, y, mGrabScene);
}
auto SSRManipulatorIn::grab_update(SFLOAT64 x, SFLOAT64 y) noexcept -> void
{
    const SFLOAT64 delx = mGrabWinX - x;
    const SFLOAT64 dely = mGrabWinY - y;

    if (mGrabState == eOrbiting) 
    {
        Bookmark bookmark = get_current_bookmark();

        const SFLOAT64 theta  = delx * mOrbitSpeed[0];
        const SFLOAT64 phi    = dely * mOrbitSpeed[1];
        constexpr SFLOAT64 maxPhi = _SSR_STD::numbers::pi / 2.0 - 0.001;

        bookmark.mOrbit.mPhi   = _SSR_STD::clamp(mGrabBookmark.mOrbit.mPhi + phi, -maxPhi, +maxPhi);
        bookmark.mOrbit.mTheta = mGrabBookmark.mOrbit.mTheta + theta;

        jump_to_bookmark(bookmark);
    }

    if (mGrabState == ePanning) 
    {
        const SFLOAT32 ulen = (mGrabScene - mGrabEye).length();
        const SFLOAT32 vlen = (mGrabFar - mGrabScene).length();
        const SVEC3 translation = (mGrabFar - raycast_far_plane(x, y)) * SVEC3(ulen / vlen);
        mPivot  = mGrabPivot  + translation;
        mEye    = mGrabEye    + translation;
        mTarget = mGrabTarget + translation;
    }
}
auto SSRManipulatorIn::scroll(SFLOAT64 x, SFLOAT64 y, SFLOAT64 scrolldelta) noexcept -> void
{
    const SVEC3 gaze = (mTarget - mEye).normalize();
    const SVEC3 movement = gaze * SVEC3(cast<SFLOAT32>(mZoomSpeed * -scrolldelta));
    const SVEC3 v0 = mPivot - mEye;

    mEye = mEye + movement;
    mTarget = mTarget + movement;

    const SVEC3 v1 = mPivot - mEye;

    if (v0.dot(v1) < 0)
    {
        mFlipped = !mFlipped;
    }
}
auto SSRManipulatorIn::grab_end() noexcept -> void
{
    mGrabState = eInactive;
}

auto SSRManipulatorIn::get_current_bookmark() const noexcept -> Bookmark
{
    Bookmark bookmark;
    const SVEC3 pivotToEye = mEye - mPivot;
    const SFLOAT64 d = pivotToEye.length();
    const SFLOAT64 x = pivotToEye[0] / d;
    const SFLOAT64 y = pivotToEye[1] / d;
    const SFLOAT64 z = pivotToEye[2] / d;

    bookmark.mOrbit.mPhi      = _SSR_STD::asin(y);
    bookmark.mOrbit.mTheta    = _SSR_STD::atan2(x, z);
    bookmark.mOrbit.mDistance = mFlipped ? -d : d;
    bookmark.mOrbit.mPivot    = mPivot;

    return bookmark;
}

auto SSRManipulatorIn::jump_to_bookmark(const Bookmark& bookmark) noexcept -> void
{
    mPivot = bookmark.mOrbit.mPivot;
    const SFLOAT64 x = _SSR_STD::sin(bookmark.mOrbit.mTheta) * _SSR_STD::cos(bookmark.mOrbit.mPhi);
    const SFLOAT64 y = _SSR_STD::sin(bookmark.mOrbit.mPhi);
    const SFLOAT64 z = _SSR_STD::cos(bookmark.mOrbit.mTheta) * _SSR_STD::cos(bookmark.mOrbit.mPhi);
    mEye = mPivot + SVEC3(x, y, z) * SVEC3(cast<SFLOAT32>(_SSR_STD::abs(bookmark.mOrbit.mDistance)));
    mFlipped = bookmark.mOrbit.mDistance < 0;
    mTarget = mEye + SVEC3(x, y, z) * SVEC3((mFlipped ? 1.0f : -1.0f));

}

auto SSRManipulatorIn::raycast_far_plane(SFLOAT64 x, SFLOAT64 y) const noexcept -> SVEC3
{
    const SVEC3    gaze   = (mTarget - mEye).normalize();
    const SVEC3    right  = gaze.cross(mUpVector);
    const SVEC3    upward = right.cross(gaze);
    const SFLOAT64 width  = mViewport[0];
    const SFLOAT64 height = mViewport[1];
    const SFLOAT64 fov    = mFovDegrees * _SSR_STD::numbers::pi / 180.0;

    // Remap the grid coordinate into [-1, +1] and shift it to the pixel center.
    const SFLOAT64 u = 2.0 * (0.5 + x) / width - 1.0;
    const SFLOAT64 v = 2.0 * (0.5 + y) / height - 1.0;

    // Compute the tangent of the field-of-view angle as well as the aspect ratio.
    const SFLOAT64 tangent = _SSR_STD::tan(fov / 2.0);
    const SFLOAT64 aspect  = width / height;

    // Adjust the gaze so it goes through the pixel of interest rather than the grid center.
    SVEC3 dir = gaze;
    if (mFovDirection == Fov::eVertical)
    {
        dir = dir + right * SVEC3(cast<SFLOAT32>(tangent * u * aspect));
        dir = dir + upward * SVEC3(cast<SFLOAT32>(tangent * v));
    }
    else
    {
        dir = dir + right * SVEC3(cast<SFLOAT32>(tangent * u));
        dir = dir + upward * SVEC3(cast<SFLOAT32>(tangent * v / aspect));
    }
    return mEye + dir * SVEC3(cast<SFLOAT32>(mFarPlane));
}

auto SSRManipulatorIn::raycast(SFLOAT64 x, SFLOAT64 y, SVEC3& result) const noexcept -> SBOOL
{
    SVEC3 origin{};
    SVEC3 dir{};
    get_ray(x, y, origin, dir);

    // Choose either the user's callback function or the plane intersector.
    auto callback = mRayCallback;
    auto fallback = raycast_plane;
    if (!callback) 
    {
        return false;
    }

    // If the ray misses, then try the fallback function.
    SFLOAT64 t{ 0 };
    if (!callback(mEye, dir, mGroundPlane, t)) 
    {
        if (!fallback(mEye, dir, mGroundPlane , t))
        {
            return false;
        }
    }

    result = mEye + dir * SVEC3(cast<SFLOAT32>(t));
    return true;
}

auto SSRManipulatorIn::get_ray(SFLOAT64 x, SFLOAT64 y, SVEC3& porigin, SVEC3& pdir) const noexcept -> void
{
    const SVEC3 gaze   = (mTarget - mEye).normalize();
    const SVEC3 right  = gaze.cross(mUpVector).normalize();
    const SVEC3 upward = right.cross(gaze);
    const SFLOAT64 width  = mViewport[0];
    const SFLOAT64 height = mViewport[1];
    const SFLOAT64 fov = mFovDegrees * _SSR_STD::numbers::pi / 180.0;

    // Remap the grid coordinate into [-1, +1] and shift it to the pixel center.
    const SFLOAT64 u = 2.0 * (0.5 + x) / width - 1.0;
    const SFLOAT64 v = 2.0 * (0.5 + y) / height - 1.0;

    // Compute the tangent of the field-of-view angle as well as the aspect ratio.
    const SFLOAT64 tangent = _SSR_STD::tan(fov / 2.0);
    const SFLOAT64 aspect  = width / height;

    // Adjust the gaze so it goes through the pixel of interest rather than the grid center.
    SVEC3 dir = gaze;
    if (mFovDirection == Fov::eVertical) 
    {
        dir = dir + right * SVEC3(cast<SFLOAT32>(tangent * u * aspect));
        dir = dir + upward * SVEC3(cast<SFLOAT32>(tangent * v));
    }
    else 
    {
        dir = dir + right * SVEC3(cast<SFLOAT32>(tangent * u));
        dir = dir + upward * SVEC3(cast<SFLOAT32>(tangent * v / aspect));
    }
    dir = dir.normalize();

    porigin = mEye;
    pdir = dir;
}
_SSR_END