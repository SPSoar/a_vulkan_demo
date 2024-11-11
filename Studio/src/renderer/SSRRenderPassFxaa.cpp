#include "renderer/SSRRenderPass.h"
#include "studio/SSRStudioIn.h"

_SSR_STUDIO_BEGIN

class SSRRenderPassFxaa::PassDetail
{
    _SSR_RENDER_PASS_DETAIL_CLASS_DEF(Fxaa)
public:
    _SSR_CORE::SSRShaderPtr     mVs{ nullptr };
    _SSR_CORE::SSRShaderPtr     mFs{ nullptr };
    _SSR_CORE::RI_DrawArguments mDrawArguments{};
};

_SSR_END


#include "inl/SSRRenderPassFxaaToolbox.inl"

_SSR_STUDIO_BEGIN

auto SSRRenderPassFxaa::init(SUINT32 pipelineIndex, SUINT32 index, _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo, _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept -> void
{
    _SSR_RENDER_PASS_INIT_FUNC_DEF(Fxaa);
}

auto SSRRenderPassFxaa::prepare(SSRRenderManagerRef renderManager, const _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept-> void
{
    _SSR_RENDER_PASS_PREPARE_FUNC_DEF(Fxaa);
}
auto SSRRenderPassFxaa::build(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_RENDER_PASS_BUILD_FUNC_DEF(Fxaa);
}

auto SSRRenderPassFxaa::uninit(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_RENDER_PASS_UNINIT_FUNC_DEF(Fxaa);
}

_SSR_END

