#include "renderer/SSRRenderPass.h"

_SSR_STUDIO_BEGIN

class SSRRenderPassDebugSubpassColor::PassDetail
{
    _SSR_RENDER_PASS_DETAIL_CLASS_DEF(DebugSubpassColor)
};

_SSR_END

#include "inl/SSRRenderPassDebugSubpassColorToolbox.inl"

_SSR_STUDIO_BEGIN

auto SSRRenderPassDebugSubpassColor::init(SUINT32 pipelineIndex, SUINT32 index, _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo, _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept -> void
{
    mPipelineIndex = pipelineIndex;
    mPassIndex = index;

    auto& flags = renderTargetInfo.mFlags;
    flags.set_color1(true);
    flags.set_depth(true);

    auto& rt = renderTargetInfo.mRenderTargets;
    
    auto& swapchain = rt[_SSR_CORE::RI_RenderTargetInfo::eIndexColor1];
    swapchain.mConfig.p.enable = true;
    swapchain.mConfig.p.managingLifetime = true;
    swapchain.mConfig.p.id = cast<SUINT8>(_SSR_CORE::RE_RenderTargetFlags::eColor1);
    swapchain.mConfig.p.index = 0xF;
    swapchain.mConfig.p.format = _SSR_CORE::RE_ImageFormat::eBGRA8U;
    swapchain.mConfig.p.usage = _SSR_CORE::RE_AttachmentUsage::eInput;
    swapchain.mConfig.p.load = _SSR_CORE::RE_LoadMode::eClear;
    swapchain.mConfig.p.store = _SSR_CORE::RE_StoreMode::eStore;
    swapchain.mConfig.p.sample = _SSR_CORE::RE_SampleCount::eCount1;
    swapchain.mConfig.p.windowSize = true;

    swapchain.mClearValue = { 0.22, 0.22, 0.22, 1.0 };
    swapchain.mHandle = nullptr;
    swapchain.mWidth = 0;
    swapchain.mWidth = 0;

    auto& depthAndStencil = rt[_SSR_CORE::RI_RenderTargetInfo::eIndexDepthAndStencil];
    depthAndStencil.mConfig.p.enable = true;
    depthAndStencil.mConfig.p.managingLifetime = true;
    depthAndStencil.mConfig.p.id = cast<SUINT8>(_SSR_CORE::RE_RenderTargetFlags::eDepthAndStencil);
    depthAndStencil.mConfig.p.index = 0xF;
    depthAndStencil.mConfig.p.format = _SSR_CORE::RE_ImageFormat::eD32;
    depthAndStencil.mConfig.p.usage = _SSR_CORE::RE_AttachmentUsage::eDontCare;
    depthAndStencil.mConfig.p.load = _SSR_CORE::RE_LoadMode::eClear;
    depthAndStencil.mConfig.p.store = _SSR_CORE::RE_StoreMode::eDontCare;
    depthAndStencil.mConfig.p.sample = _SSR_CORE::RE_SampleCount::eCount1;
    depthAndStencil.mConfig.p.windowSize = true;

    depthAndStencil.mClearValue = { 1.0, 0.0 };
    depthAndStencil.mHandle = nullptr;
    depthAndStencil.mWidth = 0;
    depthAndStencil.mWidth = 0;

    _SSR_ASSERT(mPassIndex < renderGraphInfo.mPassDescriptions.size(), _SSR_RDERROR);

    auto& passDesc = renderGraphInfo.mPassDescriptions[mPassIndex];
    if (passDesc.mBindPoint == _SSR_CORE::RE_BindPoint::eNone) 
    {
        passDesc.mBindPoint = _SSR_CORE::RE_BindPoint::eGraphics;
    }
    else 
    {
        _SSR_ASSERT(passDesc.mBindPoint == _SSR_CORE::RE_BindPoint::eGraphics, _SSR_RDERROR);
    }

    passDesc.mColorDescriptions.resize(1);
    passDesc.mColorDescriptions[0].p.attachmentIndex = cast<SUINT16>(_SSR_CORE::RI_RenderTargetInfo::eIndexColor1);
    passDesc.mColorDescriptions[0].p.enable = true;

    passDesc.mDepthDescription.p.attachmentIndex = cast<SUINT16>(_SSR_CORE::RI_RenderTargetInfo::eIndexDepthAndStencil);
    passDesc.mDepthDescription.p.enable = true;

    renderGraphInfo.mDependencies.push_back({});
    auto& dependency = renderGraphInfo.mDependencies.back();
    dependency.p.srcPassIndex = 0xF;
    dependency.p.dstPassIndex = cast<SUINT8>(index);
    dependency.p.srcPipelineStage = _SSR_CORE::RE_PipelineStage::eBottomOfPipe;
    dependency.p.dstPipelineStage = _SSR_CORE::RE_PipelineStage::eColorAttachmentOutput;
    dependency.p.srcAccessMode = _SSR_CORE::RE_AccessMode::eMemoryRead;
    dependency.p.dstAccessMode = _SSR_CORE::RE_AccessMode::eColorAttachmentReadAndWrite;

    renderGraphInfo.mDependencies.push_back({});
    auto& dependency1 = renderGraphInfo.mDependencies.back();
    dependency1.p.srcPassIndex = cast<SUINT8>(index);
    dependency1.p.dstPassIndex = 0xF;
    dependency1.p.srcPipelineStage = _SSR_CORE::RE_PipelineStage::eColorAttachmentOutput;
    dependency1.p.dstPipelineStage = _SSR_CORE::RE_PipelineStage::eBottomOfPipe;
    dependency1.p.srcAccessMode = _SSR_CORE::RE_AccessMode::eColorAttachmentReadAndWrite;
    dependency1.p.dstAccessMode = _SSR_CORE::RE_AccessMode::eMemoryRead;

    mPassInfo.mConfig.p.usage = SE_PassUsage::eColor;
    mPassInfo.mMaterialVariant.set_color(true);
}

auto SSRRenderPassDebugSubpassColor::prepare(SSRRenderManagerRef renderManager, const _SSR_CORE::RI_RenderGraphInfo& mRenderGraphInfo) noexcept-> void
{
    renderManager.clear_pass(mPipelineIndex, mPassIndex);
    renderManager.prepare_pass(mPipelineIndex, mPassIndex, mRenderGraphInfo, &mPassInfo);
}

auto SSRRenderPassDebugSubpassColor::build(SSRRenderManagerRef renderManager) noexcept -> void
{

}

auto SSRRenderPassDebugSubpassColor::uninit(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_DELETE_OBJECT_PTR(mPassDetail);
}

_SSR_END

