#pragma once
#ifndef SSR_SSRRENDERMANAGER_H_
#define SSR_SSRRENDERMANAGER_H_

#include "../common/SSRStudioHeadIn.h"

#include "manager/SSRBufferTracker.h"
#include "manager/SSRRenderTracker.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRSceneGraphNode);
_SSR_PR_DEF(SSRPerViewUniform);
_SSR_PR_DEF(SSRLightUniform);
_SSR_PR_DEF(SSRShadowUniform);
_SSR_PR_DEF(SSRPerRenderBlockUniform);
_SSR_PR_DEF(SSRPerRenderableUniform);
_SSR_PR_DEF(SSRCameraIn);
_SSR_PR_DEF(SSRRenderableManager);
_SSR_PR_DEF(SSRStudioIn);
_SSR_PR_DEF(SSRRenderManager);

class SSRRenderManager 
{
#define  _SSR_FLAGS_RENDER_STATE_DEF(_SSR_DEFINER)\

    _SSR_FLAGS_DEF(E_RenderState,
                   _SSR_FLAGS_RENDER_STATE_DEF(_SSR_FLAGS_COMBINATION_DEF),
                   WindowChanged, window_changed)
public:
    struct RenderTransferCommandInfo
    {
        SC_SubTransferCmd  mSubDrawCommands{ nullptr };
        SBOOL              mNeedTransfer{ false };
    };

    struct RenderableBufferInfo
    {
        _SSR_CORE::SSRBufferPtr  mVertexBuffer{ nullptr };
        _SSR_CORE::SSRBufferPtr  mIndexBuffer{ nullptr };
        _SSR_CORE::SSRBufferPtr  mRenderableSsbo{ nullptr };
        SSRBufferTracker::Id     mVertexTrackerId{ ~0U };
        SSRBufferTracker::Id     mIndexTrackerId{ ~0U };
        SSRBufferTracker::Id     mSsboTrackerId{ ~0U };
    };

    struct PrimitiveInfo
    {
        _SSR_CORE::RE_PrimitiveType   mPrimitiveType{ _SSR_CORE::RE_PrimitiveType::eUndefined };
        SUINT32                       mPatchControlPoints{0};
        _SSR_CORE::RI_InputVertexInfo mInputVertexInfo{};
    };

    struct DrawInfo
    {
        _SSR_CORE::RI_DrawArguments mDrawArguments{};
    };

    struct RenderPassInfo
    {
        _SSR_CORE::SSRRenderGraphPtr        mRenderGraph{ nullptr };
        SC_GraphicsPipeline                 mPipeline{};
        SC_BindingSet                       mBindingSet{};
        SC_BindingLayout                    mBindingLayout{};
        SC_SubDrawCmd                       mSubDrawCommand{ nullptr };
        SBOOL                               mDirty{ true };
        SUINT32                             mPiplineIndex{ 0 };
        SUINT32                             mPassIndex{ 0 };
    };

private:
    // 注意: RenderPassInfos 有冗余(前处理与后处理 Pass 数据分离出来了)，但问题不大
    // 1. 场景树相关的渲染信息冗余前处理和后处理 Pass 的空类
    // 2. 前处理和后处理 Pass 会冗余其他类型 Pass 的空类
    _SSR_PLACEMENT_ARRAY_DEF(RenderManager,
                             _SSR::SSRSlice<RenderPassInfo>, RenderPassInfos,
                             RenderTransferCommandInfo, SubTransferCommands,
                             PrimitiveInfo, PrimitiveInfo,
                             _SSR::SSRSlice<DrawInfo>, DrawInfos,
                             RenderableBufferInfo, RenderableBufferInfo);
public:
    using Entity           = SSRRenderManagerArray::Seal;

    struct RenderPass
    {
        _SSR_CORE::SSRDrawCommandChunkPtr mDrawCommandChunk{};
        const SI_PassInfo*                mPassInfo{ nullptr };
        Entity                            mPassEntity{};
    };

    struct RenderPipeline
    {
        SSRFitVector<RenderPass>     mRenderPasses{};
        _SSR_CORE::SSRRenderGraphPtr mRenderGraph{ nullptr };
    };

public:
    SSRRenderManager() noexcept = delete;
    explicit SSRRenderManager(SSRStudioInRef studio) noexcept;
    ~SSRRenderManager()noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRRenderManager);
    _SSR_CLASS_DELETE_MOVE(SSRRenderManager);
    _SSR_CLASS_MEMBER_GET_FUNC(SSRColor, BackgroundColor, background_color,)
    _SSR_CLASS_MEMBER_GET_FUNC(SSRCameraInPtr, Camera, current_camera, )
    _SSR_CLASS_MEMBER(SSRViewport, current_viewport, CurrentViewport, );
public:
    auto begin() noexcept -> void;
    auto build_scene_nodes(SSRRenderableManagerRef renderableManager) noexcept -> void;
    auto prepare_render_info(SSRRenderableManagerRef renderableManager) noexcept -> void;
    auto build_render_command() noexcept -> void;
    auto end() noexcept -> void;
public:
    auto set_render_pipeline_count(SUINT32 count) noexcept -> void;
    auto set_render_pass_count(SUINT32 index, SUINT32 count) noexcept -> void;

    auto prepare_render_pipeline(SUINT32 pipelineIndex, const _SSR_CORE::RI_RenderGraphInfo& mRenderGraphInfo) noexcept -> void;
    auto prepare_pass(SUINT32 pipelineIndex, SUINT32 passIndex, const _SSR_CORE::RI_RenderGraphInfo& mRenderGraphInfo, const SI_PassInfo* passInfo, Entity entity = {}) noexcept -> void;

    auto clear_render_pipeline(SUINT32 pipelineIndex) noexcept -> void;
    auto clear_pass(SUINT32 pipelineIndex, SUINT32 passIndex) noexcept -> void;

    auto get_render_graph(SUINT32 pipelineIndex) noexcept -> _SSR_CORE::SSRRenderGraphPtr;
    auto get_render_graph(SUINT32 pipelineIndex) const noexcept -> _SSR_CORE::SSRRenderGraphCPtr;
    
    auto get_render_pipelines() const noexcept -> const SSRFitVector<RenderPipeline>&;

    auto draw(SUINT32 pipelineIndex, SUINT32 passIndex, _SSR_CORE::SSRDrawCommandRef drawCommand) noexcept -> void;

    auto set_current_camera(SSRCameraInPtr camera) noexcept -> void;
    auto set_frame_count(SUINT32 count) noexcept
    {
        mFrameCount = count;
    }

    auto clear_dynamic_scene_node() noexcept -> void;
    auto clear_all_scene_node() noexcept -> void;
    auto add_scene_node(SSRSceneGraphNodeCPtr sceneNode) noexcept -> void;
    auto add_dynamic_scene_node(SSRSceneGraphNodeCPtr sceneNode) noexcept -> void;
    auto add_per_view_info(SSRPerViewUniformCPtr perViewUniform) noexcept -> void;
    auto add_light_info(SSRLightUniformCPtr lightUniform) noexcept -> void;
    auto add_background_color(const SSRColor& color) noexcept -> void;
    auto add_shadow_info(SSRShadowUniformCPtr shadowUniform) noexcept -> void;
    auto add_current_frame_index(SINT32 index) noexcept -> void;
    auto add_render_targets(const SSRFitVector<_SSR_CORE::SSRRenderTargetPtr>& renderTarget) noexcept -> void;
public:
    auto get_per_view_uniform_buffer() noexcept -> _SSR_CORE::SSRBufferPtr;
    auto get_light_info() noexcept -> SSRLightUniformCPtr;
    auto get_shadow_info() noexcept -> SSRShadowUniformCPtr;
    //auto get_per_render_block_info() const noexcept -> SSRPerRenderBlockUniformCPtr;
    //auto get_per_renderable_info() const noexcept -> SSRPerRenderableUniformCPtr;
    auto get_current_frame_index() const noexcept -> SUINT32;
    auto get_frame_count() const noexcept
    {
        return mFrameCount;
    }
    auto get_buffer_tracker() noexcept -> SSRBufferTracker&
    {
        return mBufferTracker;
    }

    auto& get_studio() noexcept
    {
        return mStudio;
    }
    auto& get_state() noexcept
    {
        return mRenderState;
    }
public:
    auto make_render_info() noexcept -> Entity;
    auto destroy(Entity e) noexcept -> void;
    auto get_draw_infos(Entity e) noexcept -> _SSR::SSRSlice<DrawInfo>&;
    auto get_render_pass_infos(Entity e) noexcept -> _SSR::SSRSlice<RenderPassInfo>&;
    auto get_render_pass_infos(Entity e) const noexcept -> const _SSR::SSRSlice<RenderPassInfo>&;
    auto get_render_transfer_command_info(Entity e) noexcept -> RenderTransferCommandInfo&;
    auto get_renderable_buffer_info(Entity e) noexcept -> RenderableBufferInfo&;
public:
    auto collect_transfer_commands() noexcept -> _SSR_CORE::SSRTransferCommandChunkPtr;
    auto collect_draw_commands(SUINT32 pipelineIndex, SUINT32 passIndex) noexcept -> _SSR_CORE::SSRDrawCommandChunkPtr;
    auto get_primitive_info(Entity e) noexcept -> PrimitiveInfo&;
protected:
    auto create_component(Entity e) noexcept-> void;
    auto destroy_component(Entity e) noexcept-> void;
protected:
    _SSR_CORE::SSRBufferPtr  mPerViewUniformBuffer{ nullptr };
    _SSR_CORE::SSRBufferPtr  mShadowUniformBuffer{ nullptr };
    _SSR_CORE::SSRBufferPtr  mLightUniformBuffer{ nullptr };
private:
    SSRStudioInRef                              mStudio;
    SSRRenderManagerArray                       mRenderInfoManager{};
    SSRRenderTracker                            mRenderTracker{};
    SSRBufferTracker                            mBufferTracker{};
    _SSR_CORE::SSRTransferCommandChunkPtr       mTransferCommandChunk{};
    SSRVector<SSRSceneGraphNodeCPtr>            mSceneNodes{};
    SSRVector<SSRSceneGraphNodeCPtr>            mDynamicSceneNodes{};
    SSRPerViewUniformCPtr                       mPerViewUniform{ nullptr };
    SSRLightUniformCPtr                         mLightUniform{ nullptr };
    SSRShadowUniformCPtr                        mShadowUniform{ nullptr };
    SSRColor                                    mBackgroundColor{ 0.25f, 0.25f, 0.25f };
    SSRFitVector<_SSR_CORE::SSRRenderTargetPtr> mRenderTargets{ };
    SUINT32                                     mFrameIndex{ 0 };
    SUINT32                                     mFrameCount{ 0 };
    SSRCameraInPtr                              mCamera{ nullptr };
    E_RenderStateFlags                          mRenderState{};
    SSRFitVector<RenderPipeline>                mRenderPipelines{};
};


_SSR_END

#endif

