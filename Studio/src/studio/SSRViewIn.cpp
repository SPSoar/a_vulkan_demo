#include "inl/SSRViewInToolbox.inl"
#include "manager/SSRRenderManager.h"
#include "studio/SSRSceneIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRView)

_SSR_VIEW_PUB_MEMBER_FUNC_IMPL(SSRView)

auto SSRView::Create() -> SSRViewUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRView);
}

SSRViewIn::SSRViewIn() noexcept
{

}

SSRViewIn::~SSRViewIn() noexcept
{

}

auto SSRViewIn::set_post_processing_enabled(bool enabled) noexcept -> void
{
    mPostProcessingEnabled = enabled;
}

auto SSRViewIn::set_camera(SSRCameraPtr camera) noexcept -> void
{
    mCamera = upcast<SSRCameraInPtr>(camera);
}

auto SSRViewIn::set_scene(SSRScenePtr scene) noexcept -> void
{
    mScene = upcast<SSRSceneInPtr>(scene);
}
auto SSRViewIn::set_viewport(const SSRViewport& viewport) noexcept -> void
{
    mViewport = viewport;
}

auto SSRViewIn::update_view_info() noexcept-> void
{
    _SSR_ASSERT(mCamera, _SSR_RDERROR);

    PreparePackage package{ mCamera, mPerViewUniform };

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Update_View_Info;
}

auto SSRViewIn::collect_view_info(SSRRenderManagerRef renderManager) const noexcept -> void
{
    renderManager.set_current_camera(mCamera);
    renderManager.set_current_viewport(mViewport);
    renderManager.add_per_view_info(&mPerViewUniform);
    renderManager.add_shadow_info(&mShadowUniform);
}

auto SSRViewIn::resize(const SUINT32 width, const SUINT32 height) noexcept -> void
{
    mWidth = width;
    mHeight = height;
}

_SSR_END

