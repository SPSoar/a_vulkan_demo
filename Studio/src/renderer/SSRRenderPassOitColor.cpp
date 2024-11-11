#include "renderer/SSRRenderPass.h"

_SSR_STUDIO_BEGIN

class SSRRenderPassOitColor::PassDetail
{
    _SSR_RENDER_PASS_DETAIL_CLASS_DEF(OitColor)
};

_SSR_END

#include "inl/SSRRenderPassOitColorToolbox.inl"

_SSR_STUDIO_BEGIN

auto SSRRenderPassOitColor::init(SUINT32 pipelineIndex, SUINT32 index, _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo, _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept -> void
{

}

auto SSRRenderPassOitColor::prepare(SSRRenderManagerRef renderManager, const _SSR_CORE::RI_RenderGraphInfo& mRenderGraphInfo) noexcept-> void
{

}
auto SSRRenderPassOitColor::build(SSRRenderManagerRef renderManager) noexcept -> void
{

}

auto SSRRenderPassOitColor::uninit(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_DELETE_OBJECT_PTR(mPassDetail);
}

_SSR_END