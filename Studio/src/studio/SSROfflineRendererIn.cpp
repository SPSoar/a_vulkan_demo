#include "studio/SSROfflineRendererIn.h"

#define _SSR_OFFLINE_RENDERER_CREATE_FUNC_IN_FUNC_DEF(RP, Class)\
    auto Class##In::create(const SSROfflineRenderer::RP##Params& param) noexcept -> SSROfflineRenderer*\
    {\
        return nullptr;\
    }\

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSROfflineRenderer)

_SSR_OFFLINE_RENDERER_PUB_MEMBER_FUNC_IMPL(SSROfflineRenderer)

_SSR_OFFLINE_RENDERER_IN_PUB_MEMBER_FUNC_IMPL(SSROfflineRendererIn)

_SSR_OFFLINE_RENDERER_RENDERER_NAME_DEF(_SSR_OFFLINE_RENDERER_CREATE_FUNC_IMPL_DEF, SSROfflineRenderer)
_SSR_OFFLINE_RENDERER_RENDERER_NAME_DEF(_SSR_OFFLINE_RENDERER_CREATE_FUNC_IN_FUNC_DEF, SSROfflineRenderer)

_SSR_OFFLINE_RENDERER_RENDERER_NAME_DEF(_SSR_OFFLINE_RENDERER_CONSTRUCTOR_IN_DEF)

inline constexpr auto CF_CreateOfflineRenderer = [](SSRStudioRef studio, const E_OnlineRendererMode rendererMode)
    -> SSROfflineRendererIn::Renderer*
{
    switch (rendererMode)
    {
    case E_OnlineRendererMode::eDebugSingleColor:
        return new SSROfflineRendererIn::Renderer(_SSR_STD::in_place_index<cast<size_t>(E_OnlineRendererMode::eDebugSingleColor)>, studio);
    case E_OnlineRendererMode::eForwardRendering:
        return new SSROfflineRendererIn::Renderer(_SSR_STD::in_place_index<cast<size_t>(E_OnlineRendererMode::eForwardRendering)>, studio);
    case E_OnlineRendererMode::eDebugTwoSubpass:
    default:
        _SSR_ASSERT(0, _SSR_RDERROR);
        _SSR_STD::unreachable();
    }
};

auto SSROfflineRenderer::Create(SSRStudioRef studio) -> SSROfflineRendererUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSROfflineRenderer, studio);
}

SSROfflineRendererIn::SSROfflineRendererIn(SSRStudioRef studio) noexcept
    : mRenderer{ CF_CreateOfflineRenderer(studio, E_OnlineRendererMode::eDebugSingleColor) }
    , _SSR_ATOM_VAL{ *mRenderer }
{

}

SSROfflineRendererIn::~SSROfflineRendererIn() noexcept
{
    _SSR_DELETE_OBJECT_PTR(mRenderer);
}



_SSR_END