#include "renderer/SSRRenderPass.h"
#include "studio/SSRStudioIn.h"
_SSR_STUDIO_BEGIN

class SSRRenderPassDebugSubpassMix::PassDetail
{
    _SSR_RENDER_PASS_DETAIL_CLASS_DEF(DebugSubpassMix)
public:
    _SSR_CORE::SSRShaderPtr     mVs{ nullptr };
    _SSR_CORE::SSRShaderPtr     mFs{ nullptr };
    _SSR_CORE::RI_DrawArguments mDrawArguments{};
};

_SSR_END

#include "inl/SSRRenderPassDebugSubpassMixToolbox.inl"

_SSR_STUDIO_BEGIN

auto SSRRenderPassDebugSubpassMix::init(SUINT32 pipelineIndex, SUINT32 index, _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo, _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept -> void
{
    _SSR_RENDER_PASS_INIT_FUNC_DEF(DebugSubpassMix);
}

auto SSRRenderPassDebugSubpassMix::prepare(SSRRenderManagerRef renderManager, const _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept-> void
{
    _SSR_RENDER_PASS_PREPARE_FUNC_DEF(DebugSubpassMix);
}

auto SSRRenderPassDebugSubpassMix::build(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_RENDER_PASS_BUILD_FUNC_DEF(DebugSubpassMix);
}

auto SSRRenderPassDebugSubpassMix::uninit(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_RENDER_PASS_UNINIT_FUNC_DEF(DebugSubpassMix);
}

_SSR_END

