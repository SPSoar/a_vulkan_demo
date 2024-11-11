#pragma once
#ifndef SSR_SSRRENDERPIPELINE_H_
#define SSR_SSRRENDERPIPELINE_H_

#include "../common/SSRStudioHeadIn.h"
#include "SSRRenderPass.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRStudioIn);

template<typename RenderPipeline>
concept CC_IsRenderPipeline = requires(RenderPipeline renderPipeline,
                                       SSRRenderManagerRef renderManager,
                                       _SSR_CORE::SSRDrawCommandRef drawCommand,
                                       SUINT32 index,
                                       _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo,
                                       const SSRViewPtr view,
                                       SFLOAT32 time)
{
    { renderPipeline.init(index, renderTargetInfo, renderManager) } -> _SSR_STD::same_as<void>;
    { renderPipeline.uninit(renderManager) }                        -> _SSR_STD::same_as<void>;
    { renderPipeline.dirty() }                                      -> _SSR_STD::same_as<void>;
    { renderPipeline.prepare(renderManager) }                       -> _SSR_STD::same_as<void>;
    { renderPipeline.build(renderManager) }                         -> _SSR_STD::same_as<void>;
    { renderPipeline.draw(renderManager, drawCommand) }             -> _SSR_STD::same_as<void>;
};

template<CC_IsRenderPass... RenderPass>
class SSRRenderPipeline
{
public:
    SSRRenderPipeline() noexcept = default;
    ~SSRRenderPipeline() noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRRenderPipeline);
    _SSR_CLASS_DELETE_MOVE(SSRRenderPipeline);
public:
    using RenderPassListType = _SSR_STD::tuple<RenderPass ...>;

    static constexpr const SUINT8 SCCV_RenderPassCount = sizeof...(RenderPass);

    template<SCINT Index>
        requires (Index < SCCV_RenderPassCount)
    auto get_render_pass() noexcept -> _SSR_STD::tuple_element_t<cast<size_t>(Index), RenderPassListType>&
    {
        return _SSR_STD::get<cast<size_t>(Index)>(mRenderPasses);
    }
    template<SCINT Index>
        requires (Index < SCCV_RenderPassCount)
    auto get_render_pass() const noexcept -> const _SSR_STD::tuple_element_t<cast<size_t>(Index), RenderPassListType>&
    {
        return _SSR_STD::get<cast<size_t>(Index)>(mRenderPasses);
    }

    auto init(SUINT32 index, _SSR_CORE::RI_RenderTargetInfo& renderTargetInfo, SSRRenderManagerRef renderManager) noexcept
    {
        mPipelineIndex = index;

        renderManager.set_render_pass_count(mPipelineIndex, SCCV_RenderPassCount);

        mRenderGraphInfo.mPassDescriptions.resize(SCCV_RenderPassCount);

        SUINT32 passIndex{ 0 };

        _SSR_STD::apply(
            [&](auto &... renderPasses) {
                (renderPasses.init(mPipelineIndex, passIndex++, renderTargetInfo, mRenderGraphInfo), ...);
            },
            mRenderPasses
        );
    }

    auto uninit(SSRRenderManagerRef renderManager) noexcept
    {
        mDirty = true;

        _SSR_STD::apply(
            [&](auto &... renderPasses) {
                (renderPasses.uninit(renderManager), ...);
            },
            mRenderPasses
        );
    }

    auto dirty() noexcept
    {
        mDirty = true;
    }

    auto prepare(SSRRenderManagerRef renderManager) noexcept
    {
        if (!mDirty) 
        {
            return;
        }
        renderManager.clear_render_pipeline(mPipelineIndex);
        renderManager.prepare_render_pipeline(mPipelineIndex, mRenderGraphInfo);

        _SSR_STD::apply(
            [&](auto &... renderPasses) {
                (renderPasses.prepare(renderManager, mRenderGraphInfo), ...);
            },
            mRenderPasses
        );
    }

    auto build(SSRRenderManagerRef renderManager) noexcept
    {
        if (!mDirty)
        {
            return;
        }

        _SSR_STD::apply(
            [&](auto &... renderPasses) {
                (renderPasses.build(renderManager), ...);
            },
            mRenderPasses
        );

        mDirty = false;
    }

    auto draw(SSRRenderManagerRef renderManager, _SSR_CORE::SSRDrawCommandRef drawCommand) noexcept
    {
        drawCommand.begin(renderManager.get_render_graph(mPipelineIndex),
                          renderManager.get_current_frame_index());

        _SSR_STD::apply(
            [&](auto &... renderPasses) {
                (renderPasses.draw(renderManager, drawCommand), ...);
            },
            mRenderPasses
        );

        drawCommand.end();
    }
private:
    RenderPassListType            mRenderPasses{};
    _SSR_CORE::RI_RenderGraphInfo mRenderGraphInfo{};
    SUINT32                       mPipelineIndex{ 0xFFFFFFFF };
    SBOOL                         mDirty{ true };
};
using DebugPipelineSingleColor  = SSRRenderPipeline<SSRRenderPassDebugSingleColor>;
using DebugPipelineTwoSubpass   = SSRRenderPipeline<SSRRenderPassDebugSubpassColor, SSRRenderPassDebugSubpassMix>;

//using PipelineForwardRendering = SSRRenderPipeline<SSRRenderPassOpaque, SSRRenderPassOitCollect, SSRRenderPassOitColor, SSRRenderPassFxaa>;
using PipelineForwardRendering = SSRRenderPipeline<SSRRenderPassDepthOnly, SSRRenderPassOpaque, SSRRenderPassFxaa>;


_SSR_END

#endif

