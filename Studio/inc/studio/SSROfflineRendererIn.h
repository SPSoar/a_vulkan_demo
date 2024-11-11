#pragma once
#ifndef SSR_SSRRENDERERIN_H_
#define SSR_SSRRENDERERIN_H_

#include "../common/SSRStudioHeadIn.h"
#include "renderer/SSRRenderer.h"

#define _SSR_OFFLINE_RENDERER_CONSTRUCTOR_DEF(RP, ...)\
    SSROfflineRendererIn(SSRStudioRef studio, const SSROfflineRenderer::RP##Params& params)  noexcept;

#define _SSR_OFFLINE_RENDERER_CONSTRUCTOR_IN_DEF(RP, ...)\
    SSROfflineRendererIn::SSROfflineRendererIn(SSRStudioRef studio, const SSROfflineRenderer::RP##Params& params)  noexcept\
        : SSROfflineRendererIn(studio)\
    {\
        init<SSROfflineRenderer::RP##Params>(params);\
    }\

_SSR_STUDIO_BEGIN

class SSROfflineRendererIn : public SSROfflineRenderer
{
    _SSR_DELETER_FRIEND_DEF(SSROfflineRenderer)
public:
    _SSR_VARIANT_DEF(Renderer,
                     DebugSingleColorRenderer,
                     DebugTwoSubpassRenderer,
                     ForwardRenderingRenderer)
protected:
    SSROfflineRendererIn() noexcept = delete;
    explicit SSROfflineRendererIn(SSRStudioRef studio)  noexcept;
    ~SSROfflineRendererIn() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSROfflineRendererIn);
    _SSR_CLASS_DELETE_MOVE(SSROfflineRendererIn);
    _SSR_OFFLINE_RENDERER_PUB_MEMBER_FUNC_DEF()
    _SSR_OFFLINE_RENDERER_RENDERER_NAME_DEF(_SSR_OFFLINE_RENDERER_CONSTRUCTOR_DEF)
    _SSR_OFFLINE_RENDERER_RENDERER_NAME_DEF(_SSR_OFFLINE_RENDERER_CREATE_FUNC_DEF)
public:
    template<typename Params>
    auto init(const Params& params) noexcept -> void 
    {
    
    }
private:
    Renderer* mRenderer;
    Renderer& _SSR_ATOM_VAL;
};


_SSR_END

#endif

