#include "studio/SSROnlineRendererIn.h"

#define _SSR_ONLINE_RENDERER_CREATE_FUNC_IN_FUNC_DEF(RP, Class)\
    auto Class##In::create(const SSROnlineRenderer::RP##Params& param) noexcept -> SSROnlineRenderer*\
    {\
        return nullptr;\
    }\

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSROnlineRenderer)

_SSR_ONLINE_RENDERER_PUB_MEMBER_FUNC_IMPL(SSROnlineRenderer)

_SSR_ONLINE_RENDERER_IN_PUB_MEMBER_FUNC_IMPL(SSROnlineRendererIn)

_SSR_ONLINE_RENDERER_RENDERER_NAME_DEF(_SSR_ONLINE_RENDERER_CREATE_FUNC_IMPL_DEF, SSROnlineRenderer)
_SSR_ONLINE_RENDERER_RENDERER_NAME_DEF(_SSR_ONLINE_RENDERER_CREATE_FUNC_IN_FUNC_DEF, SSROnlineRenderer)

_SSR_ONLINE_RENDERER_RENDERER_NAME_DEF(_SSR_ONLINE_RENDERER_CONSTRUCTOR_IN_DEF)

inline constexpr auto CF_CreateOnlineRenderer = [](SSRStudioRef studio, const E_OnlineRendererMode rendererMode)
    -> SSROnlineRendererIn::Renderer*
{
    switch (rendererMode)
    {
    case E_OnlineRendererMode::eDebugSingleColor:
        return new SSROnlineRendererIn::Renderer(_SSR_STD::in_place_index<cast<size_t>(E_OnlineRendererMode::eDebugSingleColor)>, studio);
    case E_OnlineRendererMode::eDebugTwoSubpass:
        return new SSROnlineRendererIn::Renderer(_SSR_STD::in_place_index<cast<size_t>(E_OnlineRendererMode::eDebugTwoSubpass)>, studio);
    case E_OnlineRendererMode::eForwardRendering:
        return new SSROnlineRendererIn::Renderer(_SSR_STD::in_place_index<cast<size_t>(E_OnlineRendererMode::eForwardRendering)>, studio);
    default:
        _SSR_ASSERT(0, _SSR_RDERROR);
        _SSR_STD::unreachable();
    }
};

auto SSROnlineRenderer::Create(SSRStudioRef studio, const E_OnlineRendererMode rendererMode) -> SSROnlineRendererUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSROnlineRenderer, studio, rendererMode);
}

SSROnlineRendererIn::SSROnlineRendererIn(SSRStudioRef studio, const E_OnlineRendererMode rendererMode) noexcept
    : mRenderer{ CF_CreateOnlineRenderer(studio, rendererMode)}
    , _SSR_ATOM_VAL{ *mRenderer }
{
}

SSROnlineRendererIn::~SSROnlineRendererIn() noexcept
{
    _SSR_DELETE_OBJECT_PTR(mRenderer);
}
_SSR_END