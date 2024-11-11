
_SSR_STUDIO_BEGIN

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_DepthOnly_Init_Self)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(pipelineIndex, SSRRenderPassDebugSubpassMix::PassInit::ePipelineIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passIndex, SSRRenderPassDebugSubpassMix::PassInit::ePassIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passInfo, SSRRenderPassDebugSubpassMix::PassInit::ePassInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passDetail, SSRRenderPassDebugSubpassMix::PassInit::ePassDetail);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderGraphInfo, SSRRenderPassDebugSubpassMix::PassInit::eRenderGraphInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderTargetInfo, SSRRenderPassDebugSubpassMix::PassInit::eRenderTargetInfo);

    auto& flags = renderTargetInfo.mFlags;
    flags.set_depth(true);

    auto& rt = renderTargetInfo.mRenderTargets;

    auto& depthAndStencil = rt[_SSR_CORE::RI_RenderTargetInfo::eIndexDepthAndStencil];
    depthAndStencil.mConfig.p.enable = true;
    depthAndStencil.mConfig.p.managingLifetime = true;
    depthAndStencil.mConfig.p.id = cast<SUINT8>(_SSR_CORE::RE_RenderTargetFlags::eDepthAndStencil);
    depthAndStencil.mConfig.p.index = 0xF;
    depthAndStencil.mConfig.p.format = _SSR_CORE::RE_ImageFormat::eD32;
    depthAndStencil.mConfig.p.usage = _SSR_CORE::RE_AttachmentUsage::eDontCare;
    depthAndStencil.mConfig.p.load = _SSR_CORE::RE_LoadMode::eClear;
    depthAndStencil.mConfig.p.store = _SSR_CORE::RE_StoreMode::eStore;
    depthAndStencil.mConfig.p.sample = _SSR_CORE::RE_SampleCount::eCount1;
    depthAndStencil.mConfig.p.windowSize = true;

    depthAndStencil.mClearValue = { 1.0, 0.0 };
    depthAndStencil.mHandle = nullptr;
    depthAndStencil.mWidth = 0;
    depthAndStencil.mWidth = 0;

    _SSR_ASSERT(passIndex < renderGraphInfo.mPassDescriptions.size(), _SSR_RDERROR);

    auto& passDesc = renderGraphInfo.mPassDescriptions[passIndex];
    if (passDesc.mBindPoint == _SSR_CORE::RE_BindPoint::eNone) 
    {
        passDesc.mBindPoint = _SSR_CORE::RE_BindPoint::eGraphics;
    }
    else 
    {
        _SSR_ASSERT(passDesc.mBindPoint == _SSR_CORE::RE_BindPoint::eGraphics, _SSR_RDERROR);
    }

    passDesc.mDepthDescription.p.attachmentIndex = cast<SUINT16>(_SSR_CORE::RI_RenderTargetInfo::eIndexDepthAndStencil);
    passDesc.mDepthDescription.p.enable = true;

    renderGraphInfo.mDependencies.push_back({});
    auto& dependency = renderGraphInfo.mDependencies.back();
    dependency.p.srcPassIndex = 0xF;
    dependency.p.dstPassIndex = cast<SUINT8>(passIndex);
    dependency.p.srcPipelineStage = _SSR_CORE::RE_PipelineStage::eEarlyFragmentTests;
    dependency.p.dstPipelineStage = _SSR_CORE::RE_PipelineStage::eEarlyFragmentTests;
    dependency.p.srcAccessMode = _SSR_CORE::RE_AccessMode::eDepthStencilAttachmentWrite;
    dependency.p.dstAccessMode = _SSR_CORE::RE_AccessMode::eDepthStencilAttachmentRead;

    passInfo.mConfig.p.usage = SE_PassUsage::eOther;
    passInfo.mConfig.p.depthEnable = true;
    passInfo.mConfig.p.depthWrite = true;
    passInfo.mConfig.p.colorWrite = false;
    passInfo.mMaterialVariant.set_depth_only(true);
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_DepthOnly_Init)
_SSR_MOTIONLESS_FLOW_APPEND(CF_DepthOnly_Init_Self)
_SSR_MOTIONLESS_FLOW_END

_SSR_END

