#pragma once
#ifndef SSR_SSRRENDERPASS_H_
#define SSR_SSRRENDERPASS_H_

#include "../common/SSRStudioHeadIn.h"
#include "manager/SSRRenderManager.h"

#define _SSR_RENDER_PASS_OBJECT(_SSR_DEFINER)\
    _SSR_DEFINER(DebugSingleColor)\
    _SSR_DEFINER(DebugSubpassColor)\
    _SSR_DEFINER(DebugSubpassMix)\
    _SSR_DEFINER(DepthOnly)\
    _SSR_DEFINER(Opaque)\
    _SSR_DEFINER(OitCollect)\
    _SSR_DEFINER(OitColor)\
    _SSR_DEFINER(Fxaa)\

#define _SSR_RENDER_PASS_DETAIL_CLASS_DEF(Pass)\
    public:\
        PassDetail() noexcept = default;\
        ~PassDetail() noexcept = default;\
        _SSR_CLASS_DELETE_COPY(PassDetail);\
        _SSR_CLASS_DELETE_MOVE(PassDetail);\

#define _SSR_RENDER_PASS_INIT_FUNC_DEF(Pass)\
    mPipelineIndex = pipelineIndex;\
    mPassIndex = index;\
    PassInitPackage package{ mPipelineIndex,\
                             mPassIndex,\
                             mPassInfo,\
                             mPassDetail,\
                             renderTargetInfo,\
                             renderGraphInfo };\
    _SSR::SSRFlowMotionlessWrapper(package) | CF_##Pass##_Init\

#define _SSR_RENDER_PASS_PREPARE_FUNC_DEF(Pass)\
    PassPreparePackage package{ mPipelineIndex,\
                                mPassIndex,\
                                mPassInfo,\
                                *mPassDetail,\
                                mRenderEntity,\
                                renderGraphInfo,\
                                renderManager };\
    _SSR::SSRFlowMotionlessWrapper(package) | CF_##Pass##_Prepare\

#define _SSR_RENDER_PASS_BUILD_FUNC_DEF(Pass)\
    PassBuildPackage package{ mPipelineIndex,\
                              mPassIndex,\
                              mPassInfo,\
                              * mPassDetail,\
                              mRenderEntity,\
                              renderManager };\
    _SSR::SSRFlowMotionlessWrapper(package) | CF_##Pass##_Build\


#define _SSR_RENDER_PASS_UNINIT_FUNC_DEF(Pass)\
    PassUninitPackage package{ mPipelineIndex,\
                               mPassIndex,\
                               mPassInfo,\
                               mPassDetail,\
                               mRenderEntity,\
                               renderManager };\
    _SSR::SSRFlowMotionlessWrapper(package) | CF_##Pass##_Uninit\

#define _SSR_RENDER_PASS_CLASS_DEF(Pass)\
    class SSRRenderPass##Pass final : public SSRRenderPass\
    {\
    public:\
         class PassDetail;\
         using RenderEntity = SSRRenderManager::Entity;\
    _SSR_PACKAGE_DEF(PassInit,\
                     const SUINT32, PipelineIndex,\
                     const SUINT32, PassIndex,\
                     SI_PassInfo&, PassInfo,\
                     PassDetail*&, PassDetail,\
                     _SSR_CORE::RI_RenderTargetInfo&, RenderTargetInfo,\
                     _SSR_CORE::RI_RenderGraphInfo&, RenderGraphInfo);\
    _SSR_PACKAGE_DEF(PassPrepare,\
                     const SUINT32, PipelineIndex,\
                     const SUINT32, PassIndex,\
                     SI_PassInfo&, PassInfo,\
                     PassDetail&, PassDetail,\
                     RenderEntity&, RenderEntity,\
                     const _SSR_CORE::RI_RenderGraphInfo&, RenderGraphInfo,\
                     SSRRenderManagerRef, RenderManager);\
    _SSR_PACKAGE_DEF(PassBuild,\
                     const SUINT32, PipelineIndex,\
                     const SUINT32, PassIndex,\
                     SI_PassInfo&, PassInfo,\
                     PassDetail&, PassDetail,\
                     RenderEntity&, RenderEntity,\
                     SSRRenderManagerRef, RenderManager);\
    _SSR_PACKAGE_DEF(PassUninit,\
                     const SUINT32, PipelineIndex,\
                     const SUINT32, PassIndex,\
                     SI_PassInfo&, PassInfo,\
                     PassDetail*&, PassDetail,\
                     RenderEntity&, RenderEntity,\
                     SSRRenderManagerRef, RenderManager);\
    public:\
        SSRRenderPass##Pass() noexcept = default;\
        ~SSRRenderPass##Pass() noexcept = default;\
        _SSR_CLASS_DELETE_COPY(SSRRenderPass##Pass);\
        _SSR_CLASS_DELETE_MOVE(SSRRenderPass##Pass);\
    public:\
        auto init(SUINT32 pipelineIndex, SUINT32 index, _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo, _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept -> void;\
        auto uninit(SSRRenderManagerRef renderManager) noexcept -> void;\
        auto prepare(SSRRenderManagerRef renderManager, const _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo) noexcept-> void; \
        auto build(SSRRenderManagerRef renderManager) noexcept -> void;\
        auto draw(SSRRenderManagerRef renderManager, _SSR_CORE::SSRDrawCommandRef drawCommand) noexcept-> void\
        {\
            if (mPassIndex > 0)\
            {\
                drawCommand.next();\
            }\
            renderManager.draw(mPipelineIndex, mPassIndex, drawCommand);\
        }\
    protected:\
        SUINT32                 mPipelineIndex{ 0xFFFFFFFF };\
        SUINT32                 mPassIndex{ 0xFFFFFFFF };\
        RenderEntity            mRenderEntity{};\
        SI_PassInfo             mPassInfo{};\
        PassDetail*             mPassDetail{nullptr};\
    };\
    _SSR_SASSERT(CC_IsRenderPass<SSRRenderPass##Pass>, _SSR_RDERROR);\

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRStudioIn);

template<typename RenderPass>
concept CC_IsRenderPass = requires(RenderPass renderPass,
                                   SSRRenderManagerRef renderManager,
                                   _SSR_CORE::SSRDrawCommandRef drawCommand,
                                   SUINT32 index,
                                   _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo,
                                   _SSR_CORE::RI_RenderGraphInfo& renderGraphInfo,
                                   const SSRViewPtr view,
                                   SFLOAT32 time)
{
    { renderPass.init(index, index, renderTargetInfo, renderGraphInfo) }  -> _SSR_STD::same_as<void>;
    { renderPass.uninit(renderManager) }                                  -> _SSR_STD::same_as<void>;
    { renderPass.prepare(renderManager, renderGraphInfo) }                -> _SSR_STD::same_as<void>;
    { renderPass.build(renderManager) }                                   -> _SSR_STD::same_as<void>;
    { renderPass.draw(renderManager, drawCommand) }                       -> _SSR_STD::same_as<void>;
};

class SSRRenderPass
{
public:
    SSRRenderPass() noexcept  = default;
    ~SSRRenderPass() noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRRenderPass);
    _SSR_CLASS_DELETE_MOVE(SSRRenderPass);

};

_SSR_RENDER_PASS_OBJECT(_SSR_RENDER_PASS_CLASS_DEF)

_SSR_END

#endif

