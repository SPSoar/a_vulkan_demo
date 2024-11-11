
_SSR_STUDIO_BEGIN

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Opaque_Init_Self)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(pipelineIndex, SSRRenderPassDebugSubpassMix::PassInit::ePipelineIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passIndex, SSRRenderPassDebugSubpassMix::PassInit::ePassIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passInfo, SSRRenderPassDebugSubpassMix::PassInit::ePassInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passDetail, SSRRenderPassDebugSubpassMix::PassInit::ePassDetail);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderGraphInfo, SSRRenderPassDebugSubpassMix::PassInit::eRenderGraphInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderTargetInfo, SSRRenderPassDebugSubpassMix::PassInit::eRenderTargetInfo);

    auto& flags = renderTargetInfo.mFlags;
    flags.set_color1(true);
    flags.set_depth(true);

    auto& rt = renderTargetInfo.mRenderTargets;
    
    auto& color = rt[_SSR_CORE::RI_RenderTargetInfo::eIndexColor1];
    color.mConfig.p.enable = true;
    color.mConfig.p.managingLifetime = true;
    color.mConfig.p.id = cast<SUINT8>(_SSR_CORE::RE_RenderTargetFlags::eColor1);
    color.mConfig.p.index = 0xF;
    color.mConfig.p.format = _SSR_CORE::RE_ImageFormat::eBGRA8U;
    color.mConfig.p.usage = _SSR_CORE::RE_AttachmentUsage::eInput;
    color.mConfig.p.load = _SSR_CORE::RE_LoadMode::eClear;
    color.mConfig.p.store = _SSR_CORE::RE_StoreMode::eStore;
    color.mConfig.p.sample = _SSR_CORE::RE_SampleCount::eCount1;
    color.mConfig.p.windowSize = true;

    color.mClearValue = { 0.22, 0.22, 0.22, 1.0 };
    color.mHandle = nullptr;
    color.mWidth = 0;
    color.mWidth = 0;

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

    passDesc.mColorDescriptions.resize(1);
    passDesc.mColorDescriptions[0].p.attachmentIndex = cast<SUINT16>(_SSR_CORE::RI_RenderTargetInfo::eIndexColor1);
    passDesc.mColorDescriptions[0].p.enable = true;

    passDesc.mDepthDescription.p.attachmentIndex = cast<SUINT16>(_SSR_CORE::RI_RenderTargetInfo::eIndexDepthAndStencil);
    passDesc.mDepthDescription.p.enable = true;

    renderGraphInfo.mDependencies.push_back({});
    auto& dependency1 = renderGraphInfo.mDependencies.back();
    dependency1.p.srcPassIndex = cast<SUINT8>(passIndex - 1);
    dependency1.p.dstPassIndex = cast<SUINT8>(passIndex);
    dependency1.p.srcPipelineStage = _SSR_CORE::RE_PipelineStage::eEarlyAndLateFragmentTests;
    dependency1.p.dstPipelineStage = _SSR_CORE::RE_PipelineStage::eEarlyFragmentTests;
    dependency1.p.srcAccessMode = _SSR_CORE::RE_AccessMode::eDepthStencilAttachmentWrite;
    dependency1.p.dstAccessMode = _SSR_CORE::RE_AccessMode::eDepthStencilAttachmentRead;

    passInfo.mConfig.p.usage = SE_PassUsage::eColor;
    passInfo.mConfig.p.depthWrite = false;
    passInfo.mConfig.p.depthEnable = true;
    passInfo.mConfig.p.colorWrite = true;
    passInfo.mMaterialVariant.set_color(true);
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Opaque_Init)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Opaque_Init_Self)
_SSR_MOTIONLESS_FLOW_END

_SSR_END

