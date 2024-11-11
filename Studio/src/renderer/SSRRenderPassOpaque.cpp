#include "renderer/SSRRenderPass.h"

_SSR_STUDIO_BEGIN

class SSRRenderPassOpaque::PassDetail
{
    _SSR_RENDER_PASS_DETAIL_CLASS_DEF(Opaque)
};

_SSR_END


#include "inl/SSRRenderPassOpaqueToolbox.inl"

_SSR_STUDIO_BEGIN

auto SSRRenderPassOpaque::init(SUINT32 pipelineIndex, SUINT32 index, _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo, _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept -> void
{
    _SSR_RENDER_PASS_INIT_FUNC_DEF(Opaque);
}

auto SSRRenderPassOpaque::prepare(SSRRenderManagerRef renderManager, const _SSR_CORE::RI_RenderGraphInfo& mRenderGraphInfo) noexcept-> void
{
    renderManager.clear_pass(mPipelineIndex, mPassIndex);
    renderManager.prepare_pass(mPipelineIndex, mPassIndex, mRenderGraphInfo, &mPassInfo);
}
auto SSRRenderPassOpaque::build(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_UNUSED_P(renderManager);
}

auto SSRRenderPassOpaque::uninit(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_UNUSED_P(renderManager);
    _SSR_DELETE_OBJECT_PTR(mPassDetail);
}

_SSR_END