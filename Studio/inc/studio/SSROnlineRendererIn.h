#pragma once
#ifndef SSR_SSRONLINERENDERERIN_H_
#define SSR_SSRONLINERENDERERIN_H_

#include "../common/SSRStudioHeadIn.h"
#include "renderer/SSRRenderer.h"

#define _SSR_ONLINE_RENDERER_CONSTRUCTOR_DEF(RP, ...)\
    SSROnlineRendererIn(SSRStudioRef studio, const E_OnlineRendererMode rendererMode, const SSROnlineRenderer::RP##Params& params)  noexcept;

#define _SSR_ONLINE_RENDERER_CONSTRUCTOR_IN_DEF(RP, ...)\
    SSROnlineRendererIn::SSROnlineRendererIn(SSRStudioRef studio, const E_OnlineRendererMode rendererMode, const SSROnlineRenderer::RP##Params& params)  noexcept\
        : SSROnlineRendererIn(studio, rendererMode)\
    {\
        init<SSROnlineRenderer::RP##Params>(params);\
    }\

_SSR_STUDIO_BEGIN

class SSROnlineRendererIn : public SSROnlineRenderer
{
    _SSR_DELETER_FRIEND_DEF(SSROnlineRenderer)
public:
    _SSR_VARIANT_DEF(Renderer,
                     DebugSingleColorRenderer,
                     DebugTwoSubpassRenderer,
                     ForwardRenderingRenderer)
protected:
    SSROnlineRendererIn() noexcept = delete;
    SSROnlineRendererIn(SSRStudioRef studio, E_OnlineRendererMode rendererMode)  noexcept;
    ~SSROnlineRendererIn() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSROnlineRendererIn);
    _SSR_CLASS_DELETE_MOVE(SSROnlineRendererIn);
    _SSR_ONLINE_RENDERER_PUB_MEMBER_FUNC_DEF()
    _SSR_ONLINE_RENDERER_RENDERER_NAME_DEF(_SSR_ONLINE_RENDERER_CONSTRUCTOR_DEF)
    _SSR_ONLINE_RENDERER_RENDERER_NAME_DEF(_SSR_ONLINE_RENDERER_CREATE_FUNC_DEF)
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

