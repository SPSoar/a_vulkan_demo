#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRVIEW_H_
#define SSR_SSRVIEW_H_

_SSR_STUDIO_BEGIN


_SSR_UNIQUE_DEF_BASE(SSRView, _SSR_STUDIO_EXPORT);

#define _SSR_VIEW_PUB_MEMBER_FUNC_DEF()\
    auto set_post_processing_enabled(bool enabled) noexcept -> void;\
    auto set_camera(SSRCameraPtr camera) noexcept -> void;\
    auto set_scene(SSRScenePtr scene) noexcept -> void;\
    auto set_viewport(const SSRViewport& viewport) noexcept -> void;\
    auto resize(const SUINT32 width, const SUINT32 height) noexcept -> void;\

#define _SSR_VIEW_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_post_processing_enabled, bool, enabled)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_camera, SSRCameraPtr, camera)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_scene, SSRScenePtr, scene)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_viewport, const SSRViewport&, viewport)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, resize, const SUINT32, width, const SUINT32, height)\

class _SSR_STUDIO_EXPORT SSRView
{

protected:
    SSRView() noexcept = default;
    ~SSRView()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRView);
    _SSR_CLASS_DELETE_MOVE(SSRView);
    _SSR_VIEW_PUB_MEMBER_FUNC_DEF()

public:
    static auto Create() -> SSRViewUPtr;
};


_SSR_END

#endif

