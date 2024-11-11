#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSROFFLINERENDERER_H_
#define SSR_SSROFFLINERENDERER_H_

_SSR_STUDIO_BEGIN


_SSR_UNIQUE_DEF_BASE(SSROfflineRenderer, _SSR_STUDIO_EXPORT);

#define _SSR_OFFLINE_RENDERER_PUB_MEMBER_FUNC_DEF()\
    auto beginFrame(SSRViewPtr view) noexcept -> bool;\
    auto frame(const SFLOAT32 time) noexcept -> void;\
    auto endFrame() noexcept -> void;\
    auto init() noexcept -> void;\
   
#define _SSR_OFFLINE_RENDERER_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, bool, beginFrame, SSRViewPtr, view)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, frame, const SFLOAT32, time)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, endFrame,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, init,,)\
   
#define _SSR_OFFLINE_RENDERER_IN_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_DERIVE_FUNC_OUT_NOEXCEPT(Class, bool, beginFrame, SSRViewPtr, view)\
    _SSR_CLASS_DERIVE_FUNC_OUT_NOEXCEPT(Class, void, frame, const SFLOAT32, time)\
    _SSR_CLASS_DERIVE_FUNC_OUT_NOEXCEPT(Class, void, endFrame,,)\
    _SSR_CLASS_DERIVE_FUNC_OUT_NOEXCEPT(Class, void, init,,)\
  
#define _SSR_OFFLINE_RENDERER_RENDERER_NAME_DEF(__DEF, ...)\
    __DEF(FRender, __VA_ARGS__)

#define _SSR_OFFLINE_RENDERER_RENDERER_PARAM_DEF(RP, ...)\
    using RP##Params = SSRMap<RP##Pid, SUINT32>;

#define _SSR_OFFLINE_RENDERER_CREATE_FUNC_DEF(RP, ...)\
    auto create(const SSROfflineRenderer::RP##Params& params) noexcept -> SSROfflineRenderer*;

#define _SSR_OFFLINE_RENDERER_CREATE_FUNC_IMPL_DEF(RP, Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSROfflineRenderer*, create, const SSROfflineRenderer::RP##Params&, params)\

class _SSR_STUDIO_EXPORT SSROfflineRenderer
{

public:
    enum class FRenderPid : SENUM8 
    {
        eCount
    };
public:
    _SSR_OFFLINE_RENDERER_RENDERER_NAME_DEF(_SSR_OFFLINE_RENDERER_RENDERER_PARAM_DEF)
    _SSR_OFFLINE_RENDERER_RENDERER_NAME_DEF(_SSR_OFFLINE_RENDERER_CREATE_FUNC_DEF)
protected:
    SSROfflineRenderer() noexcept = default;
    ~SSROfflineRenderer()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSROfflineRenderer);
    _SSR_CLASS_DELETE_MOVE(SSROfflineRenderer);
    _SSR_OFFLINE_RENDERER_PUB_MEMBER_FUNC_DEF()

public:
    static auto Create(SSRStudioRef studio) -> SSROfflineRendererUPtr;
};


_SSR_END

#endif

