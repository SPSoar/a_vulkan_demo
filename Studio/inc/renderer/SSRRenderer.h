#pragma once
#ifndef SSR_SSRRENDERER_H_
#define SSR_SSRRENDERER_H_

#include "../common/SSRStudioHeadIn.h"
#include "SSRRenderPipeline.h"

#include "studio/SSRStudioIn.h"
#include "studio/SSRViewIn.h"
#include "studio/SSRSceneIn.h"

_SSR_STUDIO_BEGIN

template<typename Renderer>
concept CC_IsRenderer = requires(Renderer renderer,
                                 const SSRViewPtr view,
                                 SFLOAT32 time)
{
    { renderer.init() }            -> _SSR_STD::same_as<void>;
    { renderer.beginFrame(view) }  -> _SSR_STD::same_as<bool>;
    { renderer.frame(time) }       -> _SSR_STD::same_as<void>;
    { renderer.endFrame() }        -> _SSR_STD::same_as<void>;
};

_SSR_PACKAGE_DEF(RD_Renderer,
                 SSRStudioRef&, Studio,
                 _SSR_CORE::SSRSwapchainPtr&, Swapchain,
                 SSRFitVector<_SSR_CORE::SSRSemaphorePtr>&, PresentSemaphores,
                 SSRFitVector<_SSR_CORE::SSRSemaphorePtr>&, RenderSemaphores,
                 SSRFitVector<_SSR_CORE::SSRFencePtr>&, Fence,
                 _SSR_CORE::RI_RenderTargetInfo&, RenderTargetInfo,
                 SSRFitVector<_SSR_CORE::SSRRenderTargetPtr>&, RenderTargets);

template<CC_IsRenderPipeline... RenderPipeline>
class SSRRenderer
{
public:
    SSRRenderer() noexcept  = delete;
    explicit SSRRenderer(SSRStudioRef studio) noexcept
        : mStudioIn{ upcast<SSRStudioInRef>(studio)}
        , mRenderManager{ mStudioIn.get_render_manager() }
    {
    
    }
    ~SSRRenderer() noexcept;
    _SSR_CLASS_DELETE_COPY(SSRRenderer);
    _SSR_CLASS_DELETE_MOVE(SSRRenderer);
public:
    using RenderPipelineListType = _SSR_STD::tuple<RenderPipeline ...>;

    static constexpr const SUINT8 SCCV_RenderPipelineCount = sizeof...(RenderPipeline);

    template<SCINT Index>
        requires (Index < SCCV_RenderPipelineCount)
    auto get_render_pipeline() noexcept -> _SSR_STD::tuple_element_t<cast<size_t>(Index), RenderPipelineListType>&
    {
        return _SSR_STD::get<cast<size_t>(Index)>(mRenderPipelines);
    }
    template<SCINT Index>
        requires (Index < SCCV_RenderPipelineCount)
    auto get_render_pipeline() const noexcept -> const _SSR_STD::tuple_element_t<cast<size_t>(Index), RenderPipelineListType>&
    {
        return _SSR_STD::get<cast<size_t>(Index)>(mRenderPipelines);
    }

    auto init() noexcept -> void;
    auto beginFrame(SSRViewPtr view) noexcept -> bool;
    auto frame(SFLOAT32 time) noexcept -> void;
    auto endFrame() noexcept -> void;
protected:
    auto ensure_swapchain_size(SUINT32 width, SUINT32 height) noexcept -> void;
private:
    auto acquire_next_image() noexcept
    {
        return mStudioIn.get_token().acquire_next_image(mSwapchain,
                                                        _SSR_STD::numeric_limits<SCINT>::max(),
                                                        mPresentSemaphores[mFrameIndex],
                                                        mFences[mFrameIndex],
                                                        &mSwapChainIndex);
    }

    auto present() noexcept -> void 
    {
        auto res = mStudioIn.get_token().present(mSwapchain,
                                                 mRenderSemaphores[mFrameIndex],
                                                 &mSwapChainIndex);

        mFrameIndex = (mFrameIndex + 1) % mSwapchain->get_config().mImageCount;

        _SSR_ASSERT(res, _SSR_RDERROR);
    }
protected:
    SSRViewInPtr mCurrentViewIn{ nullptr };
private:
    SSRStudioInRef                              mStudioIn;
    SSRRenderManagerRef                         mRenderManager;
    RenderPipelineListType                      mRenderPipelines{};
    _SSR_CORE::SSRSwapchainPtr                  mSwapchain{ nullptr };
    SSRFitVector<_SSR_CORE::SSRSemaphorePtr>    mPresentSemaphores{};
    SSRFitVector<_SSR_CORE::SSRSemaphorePtr>    mRenderSemaphores{};
    SSRFitVector<_SSR_CORE::SSRFencePtr>        mFences{};
    SSRFitVector<_SSR_CORE::SSRRenderTargetPtr> mRenderTargets{};
    _SSR_CORE::RI_RenderTargetInfo              mRenderTargetInfo{};
    SUINT32                                     mSwapChainIndex{ ~0U };
    SUINT32                                     mFrameIndex{ ~0U };
};

using DebugSingleColorRenderer = SSRRenderer<DebugPipelineSingleColor>;
using DebugTwoSubpassRenderer  = SSRRenderer<DebugPipelineTwoSubpass>;
using ForwardRenderingRenderer = SSRRenderer<PipelineForwardRendering>;

_SSR_SASSERT(CC_IsRenderer<DebugSingleColorRenderer>, _SSR_RDERROR);
_SSR_SASSERT(CC_IsRenderer<DebugTwoSubpassRenderer>, _SSR_RDERROR);
_SSR_SASSERT(CC_IsRenderer<ForwardRenderingRenderer>, _SSR_RDERROR);

_SSR_END

#include "inl/SSRRendererToolbox.inl"

_SSR_STUDIO_BEGIN

template<CC_IsRenderPipeline ...RenderPipeline>
inline SSRRenderer<RenderPipeline...>::~SSRRenderer() noexcept
{
    _SSR_STD::apply(
        [&](auto &... renderPipeline) {
            (renderPipeline.uninit(mRenderManager), ...);
        },
        mRenderPipelines
    );

    RD_RendererPackage package{ mStudioIn,
                                mSwapchain,
                                mPresentSemaphores,
                                mRenderSemaphores,
                                mFences,
                                mRenderTargetInfo,
                                mRenderTargets };

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Destory_Render;
}

template<CC_IsRenderPipeline ...RenderPipeline>
inline auto SSRRenderer<RenderPipeline...>::init() noexcept -> void
{
    mRenderManager.set_render_pipeline_count(SCCV_RenderPipelineCount);

    SUINT32 index{ 0 };

    _SSR_STD::apply(
        [&](auto &... renderPipeline) {
            (renderPipeline.init(index++, mRenderTargetInfo, mRenderManager), ...);
        },
        mRenderPipelines
    );

    RD_RendererPackage package{ mStudioIn,
                                mSwapchain,
                                mPresentSemaphores,
                                mRenderSemaphores,
                                mFences,
                                mRenderTargetInfo,
                                mRenderTargets};
    _SSR::SSRFlowMotionlessWrapper(package) | CF_Init_Render;

    _SSR_ASSERT(mSwapchain != nullptr, _SSR_RDERROR);
    _SSR_ASSERT(mRenderTargets.size() != 0, _SSR_RDERROR);

    mRenderManager.set_frame_count(cast<SUINT32>(mRenderTargets.size()));

    mSwapChainIndex = 0;
    mFrameIndex     = 0;
}

template<CC_IsRenderPipeline ...RenderPipeline>
inline auto SSRRenderer<RenderPipeline...>::beginFrame(SSRViewPtr view) noexcept -> bool
{
    _SSR_ASSERT(view, _SSR_RDERROR);
    _SSR_ASSERT(mFences.size() > mSwapChainIndex, _SSR_RDERROR);

    mCurrentViewIn = upcast<SSRViewInPtr>(view);

    ensure_swapchain_size(mCurrentViewIn->width(), mCurrentViewIn->height());

    if (!acquire_next_image())
    {
        return false;
    }

    return true;
}

template<CC_IsRenderPipeline ...RenderPipeline>
inline auto SSRRenderer<RenderPipeline...>::frame(const SFLOAT32 time) noexcept -> void
{
    _SSR_ASSERT(mSwapChainIndex >= 0 && mSwapChainIndex < mRenderTargets.size(), _SSR_RDERROR);

    _SSR_ASSERT(mCurrentViewIn, _SSR_RDERROR);

    mCurrentViewIn->update_view_info();

    mCurrentViewIn->collect_view_info(mRenderManager);

    auto sceneIn = mCurrentViewIn->get_scene();
    _SSR_ASSERT(sceneIn, _SSR_RDERROR);

    sceneIn->prepare_scene_graph(&mStudioIn);

    sceneIn->collect_scene_node(mRenderManager);

    mRenderManager.add_current_frame_index(cast<SINT32>(mSwapChainIndex));
    mRenderManager.add_render_targets(mRenderTargets);

    auto& renderableManager = upcast<SSRRenderableManagerRef>(mStudioIn.get_renderable_manager());

    mRenderManager.build_scene_nodes(renderableManager);

    mStudioIn.get_token().add_current_wait_semaphore(_SSR_CORE::RE_QueueType::eGraphics, mPresentSemaphores[mFrameIndex], 0);
    mStudioIn.get_token().add_current_signal_semaphore(_SSR_CORE::RE_QueueType::eGraphics, mRenderSemaphores[mFrameIndex], 0);
    mStudioIn.get_token().add_current_signal_fence(_SSR_CORE::RE_QueueType::eGraphics, mFences[mFrameIndex]);

    mRenderManager.begin();

    _SSR_STD::apply(
        [&](auto &... renderPipeline) {
            (renderPipeline.prepare(mRenderManager), ...);
        },
        mRenderPipelines
    );

    mRenderManager.prepare_render_info(renderableManager);

    mRenderManager.build_render_command();

    _SSR_STD::apply(
        [&](auto &... renderPipeline) {
            (renderPipeline.build(mRenderManager), ...);
        },
        mRenderPipelines
    );

    mRenderManager.end();

    auto& token = mStudioIn.get_token();

    auto&& transferCommands = mRenderManager.collect_transfer_commands();

    if (!transferCommands->empty()) 
    {
        auto& primaryCommand = mStudioIn.get_transfer_command(mRenderManager.get_current_frame_index());
        primaryCommand.open();
        primaryCommand.execute_sub_commands(transferCommands);
        primaryCommand.close();

        auto& pimaryCommand = mStudioIn.get_transfer_command(mRenderManager.get_current_frame_index());

        token.execute_command(&pimaryCommand);
    }

    auto& primaryCommand = mStudioIn.get_primary_draw_command(mRenderManager.get_current_frame_index());

    primaryCommand.open();

    _SSR_STD::apply(
        [&](auto &... renderPipeline) {
            (renderPipeline.draw(mRenderManager, primaryCommand), ...);
        },
        mRenderPipelines
    );

    primaryCommand.close();

}

template<CC_IsRenderPipeline ...RenderPipeline>
inline auto SSRRenderer<RenderPipeline...>::endFrame() noexcept -> void
{
    mRenderManager.get_state().reset();

    auto& token = mStudioIn.get_token();

    auto& primaryCommand = mStudioIn.get_primary_draw_command(mRenderManager.get_current_frame_index());

    token.execute_command(&primaryCommand);

    present();

    mCurrentViewIn = nullptr;
}

template<CC_IsRenderPipeline ...RenderPipeline>
inline auto SSRRenderer<RenderPipeline...>::ensure_swapchain_size(const SUINT32 width, const SUINT32 height) noexcept -> void
{
    auto& cfg = mSwapchain->get_config();
    if (cfg.mSwapchainWidth != width || cfg.mSwapchainHeight != height) 
    {
        cfg.mSwapchainWidth  = width;
        cfg.mSwapchainHeight = height;

        _SSR_STD::apply(
            [this](auto &... renderPipeline) {
                (renderPipeline.dirty(), ...);
            },
            mRenderPipelines
        );

        mStudioIn.get_token().wait_idle();
        
        RD_RendererPackage package{ mStudioIn,
                                    mSwapchain,
                                    mPresentSemaphores,
                                    mRenderSemaphores,
                                    mFences,
                                    mRenderTargetInfo,
                                    mRenderTargets};
        _SSR::SSRFlowMotionlessWrapper(package) | CF_Rebulid_Render;

        mRenderManager.get_state().set_window_changed(true);

        _SSR_ASSERT(mSwapchain != nullptr, _SSR_RDERROR);
        _SSR_ASSERT(mRenderTargets.size() != 0, _SSR_RDERROR);
    }
}

_SSR_END

#endif

