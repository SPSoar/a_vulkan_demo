#pragma once
#ifndef SSR_SSRVIEWIN_H_
#define SSR_SSRVIEWIN_H_

#include "../common/SSRStudioHeadIn.h"
#include "core/SSRPerViewUniform.h"
#include "core/SSRShadowUniform.h"

_SSR_STUDIO_BEGIN


_SSR_PR_DEF(SSRViewIn);
_SSR_PR_DEF(SSRSceneIn);
_SSR_PR_DEF(SSRCameraIn);
_SSR_PR_DEF(SSRRenderManager);

class SSRViewIn : public SSRView
{
public:
    _SSR_PACKAGE_DEF(Prepare,
                     SSRCameraInPtr&, Camera,
                     SSRPerViewUniform&, PerViewUniform);

    _SSR_DELETER_FRIEND_DEF(SSRView)
protected:
    SSRViewIn()  noexcept;
    ~SSRViewIn() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRViewIn);
    _SSR_CLASS_DELETE_MOVE(SSRViewIn);
    _SSR_VIEW_PUB_MEMBER_FUNC_DEF()
    _SSR_CLASS_MEMBER_GET_FUNC(SSRSceneInPtr, Scene, scene, )
    _SSR_CLASS_MEMBER_GET_FUNC(SSRCameraInPtr, Camera, camera, )
public:
    auto update_view_info() noexcept -> void;
    auto collect_view_info(SSRRenderManagerRef renderManager) const noexcept -> void;

    auto width() const noexcept 
    {
        return mWidth;
    }
    auto height() const noexcept
    {
        return mHeight;
    }
protected:
    SSRPerViewUniform mPerViewUniform{};
    SSRShadowUniform  mShadowUniform{};
    SSRSceneInPtr     mScene{ nullptr };
    SSRCameraInPtr    mCamera{ nullptr };
    SSRViewport       mViewport{ 0, 0, _SSR_CFG::V_WINDOW_WIDTH(), _SSR_CFG::V_WINDOW_HEIGHT() };
    SUINT32           mWidth{ _SSR_CFG::V_WINDOW_WIDTH() };
    SUINT32           mHeight{ _SSR_CFG::V_WINDOW_HEIGHT() };
    SBOOL             mPostProcessingEnabled{ false };
};

_SSR_END

#endif

