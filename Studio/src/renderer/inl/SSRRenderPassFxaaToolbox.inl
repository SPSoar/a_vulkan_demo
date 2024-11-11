#include <Shaders/SSRMaterialBucket.h>

_SSR_STUDIO_BEGIN

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Fxaa_Init_Self)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(pipelineIndex, SSRRenderPassFxaa::PassInit::ePipelineIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passIndex, SSRRenderPassFxaa::PassInit::ePassIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passInfo, SSRRenderPassFxaa::PassInit::ePassInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passDetail, SSRRenderPassFxaa::PassInit::ePassDetail);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderGraphInfo, SSRRenderPassFxaa::PassInit::eRenderGraphInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderTargetInfo, SSRRenderPassFxaa::PassInit::eRenderTargetInfo);

    auto& flags = renderTargetInfo.mFlags;
    flags.set_swapchain(true);

    auto& rt = renderTargetInfo.mRenderTargets;
    
    auto& swapchain = rt[_SSR_CORE::RI_RenderTargetInfo::eIndexSwapchain];
    swapchain.mConfig.p.enable = true;
    swapchain.mConfig.p.managingLifetime = false;
    swapchain.mConfig.p.id = cast<SUINT8>(_SSR_CORE::RE_RenderTargetFlags::eSwapchain);
    swapchain.mConfig.p.index = 0xF;
    swapchain.mConfig.p.format = _SSR_CORE::RE_ImageFormat::eUndefined;
    swapchain.mConfig.p.usage = _SSR_CORE::RE_AttachmentUsage::eOutput;
    swapchain.mConfig.p.load = _SSR_CORE::RE_LoadMode::eClear;
    swapchain.mConfig.p.store = _SSR_CORE::RE_StoreMode::eStore;
    swapchain.mConfig.p.sample = _SSR_CORE::RE_SampleCount::eCount1;
    swapchain.mConfig.p.windowSize = true;

    swapchain.mClearValue = { 0.22, 0.22, 0.22, 1.0 };
    swapchain.mHandle = nullptr;
    swapchain.mWidth = 0;
    swapchain.mWidth = 0;


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
    passDesc.mColorDescriptions[0].p.attachmentIndex = cast<SUINT16>(_SSR_CORE::RI_RenderTargetInfo::eIndexColor0);
    passDesc.mColorDescriptions[0].p.enable = true;

    passDesc.mInputDescriptions.resize(1);
    passDesc.mInputDescriptions[0].p.attachmentIndex = cast<SUINT16>(_SSR_CORE::RI_RenderTargetInfo::eIndexColor1);
    passDesc.mInputDescriptions[0].p.enable = true;

    renderGraphInfo.mDependencies.push_back({});
    auto& dependency1 = renderGraphInfo.mDependencies.back();
    dependency1.p.srcPassIndex = cast<SUINT8>(passIndex);
    dependency1.p.dstPassIndex = 0xF;
    dependency1.p.srcPipelineStage = _SSR_CORE::RE_PipelineStage::eColorAttachmentOutput;
    dependency1.p.dstPipelineStage = _SSR_CORE::RE_PipelineStage::eBottomOfPipe;
    dependency1.p.srcAccessMode = _SSR_CORE::RE_AccessMode::eColorAttachmentReadAndWrite;
    dependency1.p.dstAccessMode = _SSR_CORE::RE_AccessMode::eMemoryRead;

    _SSR_ASSERT(passIndex - 1 >= 0, _SSR_RDERROR);

    renderGraphInfo.mDependencies.push_back({});
    auto& dependency2 = renderGraphInfo.mDependencies.back();
    dependency2.p.srcPassIndex = cast<SUINT8>(passIndex - 1);
    dependency2.p.dstPassIndex = cast<SUINT8>(passIndex);
    dependency2.p.srcPipelineStage = _SSR_CORE::RE_PipelineStage::eColorAttachmentOutput;
    dependency2.p.dstPipelineStage = _SSR_CORE::RE_PipelineStage::eColorAttachmentOutput;
    dependency2.p.srcAccessMode = _SSR_CORE::RE_AccessMode::eColorAttachmentReadAndWrite;
    dependency2.p.dstAccessMode = _SSR_CORE::RE_AccessMode::eColorAttachmentReadAndWrite;

    passInfo.mConfig.p.usage = SE_PassUsage::ePostProcessing;
    passInfo.mConfig.p.depthEnable = false;
    passInfo.mConfig.p.depthWrite = false;
    passInfo.mConfig.p.colorWrite = true;

    _SSR_CREATE_OBJECT_PTR(passDetail, SSRRenderPassFxaa::PassDetail);

_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Fxaa_Prepare_Self)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(pipelineIndex, SSRRenderPassFxaa::PassPrepare::ePipelineIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passIndex, SSRRenderPassFxaa::PassPrepare::ePassIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passInfo, SSRRenderPassFxaa::PassPrepare::ePassInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passDetail, SSRRenderPassFxaa::PassPrepare::ePassDetail);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderGraphInfo, SSRRenderPassFxaa::PassPrepare::eRenderGraphInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderPassFxaa::PassPrepare::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderPassFxaa::PassPrepare::eRenderManager);

    renderManager.clear_pass(pipelineIndex, passIndex);
    
    if (passInfo.mConfig.p.usage == SE_PassUsage::ePostProcessing)
    {
        if (!renderEntity)
        {
            renderEntity = renderManager.make_render_info();
        }

        auto& token = renderManager.get_studio().get_token();

        if (!passDetail.mVs)
        {
            _SSR_CORE::RI_ShaderInfo infoVs{};
            infoVs.mShaderType = _SSR_CORE::RE_ShaderType::eVertex;
            infoVs.mData = _SSR_FULLSCREEN_VERT_DATA;
            infoVs.mSize = _SSR_FULLSCREEN_VERT_SIZE * sizeof(SUINT32);
            infoVs.mShaderName = "main";
            passDetail.mVs = token.make_shader(_SSR_STD::move(infoVs));
        }
        if (!passDetail.mFs)
        {
            _SSR_CORE::RI_ShaderInfo infoFs{};
            infoFs.mShaderType = _SSR_CORE::RE_ShaderType::eFragment;
            infoFs.mData = _SSR_MIX_FRAG_DATA;
            infoFs.mSize = _SSR_MIX_FRAG_SIZE * sizeof(SUINT32);
            infoFs.mShaderName = "main";
            passDetail.mFs = token.make_shader(_SSR_STD::move(infoFs));

            _SSR_ASSERT(passDetail.mVs != nullptr, _SSR_RDERROR);
            _SSR_ASSERT(passDetail.mFs != nullptr, _SSR_RDERROR);
        }

        auto& renderPassInfos = renderManager.get_render_pass_infos(renderEntity);
        auto&& renderPipelines = renderManager.get_render_pipelines();

        auto& pass = renderPassInfos[passIndex + renderPipelines.size() * pipelineIndex];
        auto& pipe = renderPipelines[pipelineIndex];

        _SSR_ASSERT(passIndex < pipe.mRenderPasses.size(), _SSR_RDERROR);
        _SSR_ASSERT(pipelineIndex < renderPipelines.size(), _SSR_RDERROR);
        _SSR_ASSERT(renderManager.get_frame_count() <= pass.mSubDrawCommand.size(), _SSR_RDERROR);

        _SSR_ASSERT(pipe.mRenderGraph, _SSR_RDERROR);
        auto&& rts = pipe.mRenderGraph->get_config().mRenderTargets;
        _SSR_ASSERT(rts, _SSR_RDERROR);

        auto& passDesc = renderGraphInfo.mPassDescriptions[passIndex];

        for (UINT32 i = 0; i < renderManager.get_frame_count(); i++)
        {
            auto& cmd = pass.mSubDrawCommand[i];

            if (!cmd) 
            {
                _SSR_CORE::RI_SubDrawCommandInfo info{};
                cmd = token.make_sub_draw_command(_SSR_STD::move(info));
                _SSR_ASSERT(cmd != nullptr, _SSR_RDERROR);
            }
            auto&& rt = rts->at(i);
            _SSR_ASSERT(rt, _SSR_RDERROR);

            auto& bindingLayout = pass.mBindingLayout[i];
            auto& bindingSet    = pass.mBindingSet[i];

            if (bindingSet) 
            {
                token.delete_binding_set(bindingSet);
                bindingSet = nullptr;
            }

            if (bindingLayout)
            {
                token.delete_binding_layout(bindingLayout);
                bindingLayout = nullptr;
            }

            _SSR_CORE::RI_BindingSetInfo setInfo{};

            for (UINT32 j = 0; j < passDesc.mInputDescriptions.size(); j++) 
            {
                auto&& input = passDesc.mInputDescriptions[j];
                _SSR_ASSERT(input.p.attachmentIndex < rt->get_config().mRenderTargets.size(), _SSR_RDERROR);
                auto& img = rt->get_config().mRenderTargets[input.p.attachmentIndex].mHandle;
                _SSR_STUDIO_INPUT_BINDING_FUNC(img, 0)
            }
            if (!setInfo.mBindings.empty())
            {
                _SSR_CORE::RI_BindingLayoutInfo setLayoutInfo{};
                setLayoutInfo.mVisibility = _SSR_CORE::RE_ShaderType::eFragment;
                setLayoutInfo.mRegisterSpace = 0;
                S_CF_Convert_Set_To_Layout(setInfo.mBindings, setLayoutInfo.mBindings);

                bindingLayout = token.make_binding_layout(_SSR_STD::move(setLayoutInfo));
                setInfo.mBindingLayout = bindingLayout;
                bindingSet = token.make_binding_set(_SSR_STD::move(setInfo));
            }

            _SSR_ASSERT(bindingLayout, _SSR_RDERROR);
            _SSR_ASSERT(bindingSet, _SSR_RDERROR);

            auto& pipeline = pass.mPipeline[i];
            if (pipeline)
            {
                token.delete_graphics_pipeline(pipeline);
                pipeline = nullptr;
            }

            _SSR_CORE::RI_GraphicsPipelineInfo info{};
            info.mPrimType = _SSR_CORE::RE_PrimitiveType::eTriangleList;
            info.mVertexShader   = passDetail.mVs;
            info.mFragmentShader = passDetail.mFs;
            info.mRenderGraph = renderManager.get_render_graph(pipelineIndex);
            info.mInputVertexInfo = nullptr;
            info.mPatchControlPoints = 0;
            info.mSubpassIndex = passIndex;
            info.mRenderState.mDepthStencilState.mDepthEnable = passInfo.mConfig.p.depthEnable;
            info.mRenderState.mDepthStencilState.mDepthWriteEnable = passInfo.mConfig.p.depthWrite;

            if (bindingLayout)
            {
                info.mBindingLayouts.push_back(bindingLayout);
            }
            pipeline = token.make_graphics_pipeline(_SSR_STD::move(info));
            _SSR_ASSERT(pipeline, _SSR_RDERROR);
        }
        passDetail.mDrawArguments.mVertexCount = 3;
    }

    renderManager.prepare_pass(pipelineIndex, passIndex, renderGraphInfo, &passInfo, renderEntity);
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Fxaa_Build_Self)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(pipelineIndex, SSRRenderPassFxaa::PassBuild::ePipelineIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passIndex, SSRRenderPassFxaa::PassBuild::ePassIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passInfo, SSRRenderPassFxaa::PassBuild::ePassInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passDetail, SSRRenderPassFxaa::PassBuild::ePassDetail);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderPassFxaa::PassBuild::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderPassFxaa::PassBuild::eRenderManager);

    if (passInfo.mConfig.p.usage == SE_PassUsage::ePostProcessing)
    {
        auto& renderPassInfos = renderManager.get_render_pass_infos(renderEntity);
       
        auto&& renderPipelines = renderManager.get_render_pipelines();
        _SSR_ASSERT(pipelineIndex < renderPipelines.size(), _SSR_RDERROR);

        auto& pass = renderPassInfos[passIndex + renderPipelines.size() * pipelineIndex];

        _SSR_ASSERT(renderManager.get_frame_count() <= pass.mSubDrawCommand.size(), _SSR_RDERROR);

        for (UINT32 i = 0; i < renderManager.get_frame_count(); i++)
        {
            auto& drawCommand = pass.mSubDrawCommand[i];
            _SSR_ASSERT(drawCommand, _SSR_RDERROR);

            auto& bindingSet    = pass.mBindingSet[i];
            auto& pipeline = pass.mPipeline[i];
            _SSR_ASSERT(bindingSet, _SSR_RDERROR);
            _SSR_ASSERT(pipeline, _SSR_RDERROR);

            drawCommand->open(renderManager.get_render_graph(pipelineIndex), i, passIndex);
            drawCommand->set_graphics_state(pipeline, bindingSet);
            drawCommand->draw(passDetail.mDrawArguments);
            drawCommand->close();
        }
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Fxaa_Uninit_Self)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(pipelineIndex, SSRRenderPassFxaa::PassUninit::ePipelineIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passIndex, SSRRenderPassFxaa::PassUninit::ePassIndex);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passInfo, SSRRenderPassFxaa::PassUninit::ePassInfo);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(passDetail, SSRRenderPassFxaa::PassUninit::ePassDetail);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderEntity, SSRRenderPassFxaa::PassUninit::eRenderEntity);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRRenderPassFxaa::PassUninit::eRenderManager);


    if (passInfo.mConfig.p.usage == SE_PassUsage::ePostProcessing)
    {
        auto& token = renderManager.get_studio().get_token();

        _SSR_LIKELY_IF(passDetail->mVs)
        {
            token.delete_shader(passDetail->mVs);
            passDetail->mVs = nullptr;
        }
        _SSR_LIKELY_IF(passDetail->mFs)
        {
            token.delete_shader(passDetail->mFs);
            passDetail->mFs = nullptr;
        }
        _SSR_LIKELY_IF (renderEntity)
        {
            renderManager.destroy(renderEntity);
            renderEntity.reset();
        }
    }
    _SSR_DELETE_OBJECT_PTR(passDetail);
_SSR_MOTIONLESS_FLOW_IFUNC_END


_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Fxaa_Init)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Fxaa_Init_Self)
_SSR_MOTIONLESS_FLOW_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Fxaa_Prepare)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Fxaa_Prepare_Self)
_SSR_MOTIONLESS_FLOW_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Fxaa_Build)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Fxaa_Build_Self)
_SSR_MOTIONLESS_FLOW_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Fxaa_Uninit)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Fxaa_Uninit_Self)
_SSR_MOTIONLESS_FLOW_END

_SSR_END

