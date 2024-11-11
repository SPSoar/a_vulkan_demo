#include "renderer/SSRRenderPass.h"

_SSR_STUDIO_BEGIN

class SSRRenderPassOitCollect::PassDetail
{
    _SSR_RENDER_PASS_DETAIL_CLASS_DEF(OitCollect)
};

_SSR_END


#include "inl/SSRRenderPassOitCollectToolbox.inl"

_SSR_STUDIO_BEGIN

auto SSRRenderPassOitCollect::init(SUINT32 pipelineIndex, SUINT32 index, _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo, _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept -> void
{

}

auto SSRRenderPassOitCollect::prepare(SSRRenderManagerRef renderManager, const _SSR_CORE::RI_RenderGraphInfo& mRenderGraphInfo) noexcept-> void
{

}
auto SSRRenderPassOitCollect::build(SSRRenderManagerRef renderManager) noexcept -> void
{

}

auto SSRRenderPassOitCollect::uninit(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_DELETE_OBJECT_PTR(mPassDetail);
}

_SSR_END