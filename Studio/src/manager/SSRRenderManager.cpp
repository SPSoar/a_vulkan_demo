#include "manager/SSRRenderManager.h"
#include "scene/SSRSceneGraphNode.h"
#include "studio/SSRCameraIn.h"
#include "studio/SSRStudioIn.h"
#include "core/SSRPerViewUniform.h"

_SSR_STUDIO_BEGIN

SSRRenderManager::SSRRenderManager(SSRStudioInRef studio) noexcept
    : mStudio{ studio }
{
    mRenderTracker.init(mStudio);
    mBufferTracker.init(mStudio);
}

SSRRenderManager::~SSRRenderManager() noexcept
{
    for(auto&& pipe : mRenderPipelines)
    {
        _SSR_LIKELY_IF(pipe.mRenderGraph)
        {
            mStudio.get_token().delete_render_graph(pipe.mRenderGraph);
            pipe.mRenderGraph = nullptr;
        }

        for(auto& pass : pipe.mRenderPasses)
        {
            _SSR_LIKELY_IF(pass.mDrawCommandChunk)
            {
                mStudio.get_token().delete_draw_command_chunk(pass.mDrawCommandChunk);
                pass.mDrawCommandChunk = nullptr;
            }
        }
    }

    _SSR_LIKELY_IF(mTransferCommandChunk)
    {
        mStudio.get_token().delete_transfer_command_chunk(mTransferCommandChunk);
        mTransferCommandChunk = nullptr;
    }

    _SSR_LIKELY_IF(mPerViewUniformBuffer)
    {
        mStudio.get_token().delete_buffer(mPerViewUniformBuffer);
        mPerViewUniformBuffer = nullptr;
    }

    mRenderTracker.uninit(mStudio);
    mBufferTracker.uninit(mStudio);
}

auto SSRRenderManager::set_render_pipeline_count(SUINT32 count) noexcept -> void
{
    _SSR_ASSERT(count > 0, _SSR_RDERROR);
    _SSR_ASSERT(mRenderPipelines.empty(), _SSR_RDERROR);
    mRenderPipelines.resize(count);
}
auto SSRRenderManager::set_render_pass_count(SUINT32 index, SUINT32 count) noexcept -> void
{
    _SSR_ASSERT(count > 0, _SSR_RDERROR);
    _SSR_ASSERT(index < mRenderPipelines.size(), _SSR_RDERROR);
    _SSR_ASSERT(mRenderPipelines[index].mRenderPasses.empty(), _SSR_RDERROR);
    mRenderPipelines[index].mRenderPasses.resize(count);
}

auto SSRRenderManager::prepare_render_pipeline(SUINT32 pipelineIndex, const _SSR_CORE::RI_RenderGraphInfo& mRenderGraphInfo) noexcept -> void
{
    _SSR_ASSERT(pipelineIndex < mRenderPipelines.size(), _SSR_RDERROR);
    auto& pipe = mRenderPipelines[pipelineIndex];
    _SSR_ASSERT(!pipe.mRenderGraph, _SSR_RDERROR);
    
    auto copyInfo = mRenderGraphInfo;
    copyInfo.mRenderTargets = &mRenderTargets;
    pipe.mRenderGraph = mStudio.get_token().make_render_graph(_SSR_STD::move(copyInfo));
}

auto SSRRenderManager::prepare_pass(SUINT32 pipelineIndex, SUINT32 passIndex, const _SSR_CORE::RI_RenderGraphInfo& mRenderGraphInfo, const SI_PassInfo* passInfo, Entity entity) noexcept -> void
{
    _SSR_ASSERT(pipelineIndex < mRenderPipelines.size(), _SSR_RDERROR);
    auto& pipe = mRenderPipelines[pipelineIndex];
    _SSR_ASSERT(passIndex < pipe.mRenderPasses.size(), _SSR_RDERROR);
    pipe.mRenderPasses[passIndex].mPassInfo = passInfo;
    pipe.mRenderPasses[passIndex].mPassEntity = entity;
}

auto SSRRenderManager::clear_render_pipeline(SUINT32 pipelineIndex) noexcept -> void
{
    _SSR_ASSERT(pipelineIndex < mRenderPipelines.size(), _SSR_RDERROR);
    auto& pipe = mRenderPipelines[pipelineIndex];
    if (pipe.mRenderGraph) 
    {
        mStudio.get_token().delete_render_graph(pipe.mRenderGraph);
        pipe.mRenderGraph = nullptr;
    }
}

auto SSRRenderManager::clear_pass(SUINT32 pipelineIndex, SUINT32 passIndex) noexcept -> void
{
    _SSR_ASSERT(pipelineIndex < mRenderPipelines.size(), _SSR_RDERROR);
    auto& pipe = mRenderPipelines[pipelineIndex];
    _SSR_ASSERT(passIndex < pipe.mRenderPasses.size(), _SSR_RDERROR);
    pipe.mRenderPasses[passIndex].mPassInfo = nullptr;

    if (auto& cmdChunk = pipe.mRenderPasses[passIndex].mDrawCommandChunk)
    {
        mStudio.get_token().delete_draw_command_chunk(cmdChunk);
        cmdChunk = nullptr;
    }
}

auto SSRRenderManager::get_render_graph(SUINT32 pipelineIndex) noexcept -> _SSR_CORE::SSRRenderGraphPtr
{
    _SSR_ASSERT(pipelineIndex < mRenderPipelines.size(), _SSR_RDERROR);
    auto& pipe = mRenderPipelines[pipelineIndex];
    _SSR_ASSERT(pipe.mRenderGraph, _SSR_RDERROR);
    return pipe.mRenderGraph;
}

auto SSRRenderManager::get_render_graph(SUINT32 pipelineIndex) const noexcept -> _SSR_CORE::SSRRenderGraphCPtr
{
    _SSR_ASSERT(pipelineIndex < mRenderPipelines.size(), _SSR_RDERROR);
    auto& pipe = mRenderPipelines[pipelineIndex];
    _SSR_ASSERT(pipe.mRenderGraph, _SSR_RDERROR);
    return pipe.mRenderGraph;
}


auto SSRRenderManager::get_render_pipelines() const noexcept -> const SSRFitVector<RenderPipeline>&
{
    return mRenderPipelines;
}

auto SSRRenderManager::draw(SUINT32 pipelineIndex, SUINT32 passIndex, _SSR_CORE::SSRDrawCommandRef drawCommand) noexcept -> void
{
    _SSR_ASSERT(pipelineIndex < mRenderPipelines.size(), _SSR_RDERROR);
    auto& pipe = mRenderPipelines[pipelineIndex];
    _SSR_ASSERT(passIndex < pipe.mRenderPasses.size(), _SSR_RDERROR);
    _SSR_UNUSED_P(pipe);

    auto&& commands = collect_draw_commands(pipelineIndex, passIndex);

    if (!commands->empty())
    {
        drawCommand.execute_sub_commands(commands);
    }
}

auto SSRRenderManager::set_current_camera(SSRCameraInPtr camera) noexcept -> void
{
    mCamera = camera;
}

auto SSRRenderManager::clear_dynamic_scene_node() noexcept -> void
{
    mDynamicSceneNodes.clear();
}

auto SSRRenderManager::clear_all_scene_node() noexcept -> void
{
    clear_dynamic_scene_node();
    mSceneNodes.clear();
}

auto SSRRenderManager::add_scene_node(SSRSceneGraphNodeCPtr sceneNode) noexcept -> void
{
    _SSR_ASSERT(sceneNode, _SSR_RDERROR);
    mSceneNodes.push_back(sceneNode);
}

auto SSRRenderManager::add_dynamic_scene_node(SSRSceneGraphNodeCPtr sceneNode) noexcept -> void
{
    _SSR_ASSERT(sceneNode, _SSR_RDERROR);
    mDynamicSceneNodes.push_back(sceneNode);
}

auto SSRRenderManager::add_per_view_info(SSRPerViewUniformCPtr perViewUniform) noexcept -> void
{
    _SSR_ASSERT(perViewUniform, _SSR_RDERROR);
    mPerViewUniform = perViewUniform;
}

auto SSRRenderManager::add_light_info(SSRLightUniformCPtr lightUniform) noexcept -> void
{
    _SSR_ASSERT(lightUniform, _SSR_RDERROR);
    mLightUniform = lightUniform;
}
auto SSRRenderManager::add_shadow_info(SSRShadowUniformCPtr shadowUniform) noexcept -> void
{
    _SSR_ASSERT(shadowUniform, _SSR_RDERROR);
    mShadowUniform = shadowUniform;
}
auto SSRRenderManager::get_per_view_uniform_buffer() noexcept -> _SSR_CORE::SSRBufferPtr
{
    _SSR_ASSERT(mPerViewUniformBuffer, _SSR_RDERROR);
    return mPerViewUniformBuffer;
}

auto SSRRenderManager::get_light_info() noexcept -> SSRLightUniformCPtr
{
    return mLightUniform;
}
auto SSRRenderManager::get_shadow_info() noexcept -> SSRShadowUniformCPtr
{
    return mShadowUniform;
}

auto SSRRenderManager::add_background_color(const SSRColor& color) noexcept -> void
{
    mBackgroundColor = color;
}

auto SSRRenderManager::add_current_frame_index(SINT32 index) noexcept -> void
{
    mFrameIndex = index;
}

auto SSRRenderManager::add_render_targets(const SSRFitVector<_SSR_CORE::SSRRenderTargetPtr>& renderTarget) noexcept -> void
{
    _SSR_ASSERT(!renderTarget.empty(), _SSR_RDERROR);
    mRenderTargets = renderTarget;
}

auto SSRRenderManager::get_current_frame_index() const noexcept -> SUINT32
{
    return mFrameIndex;
}

auto SSRRenderManager::begin() noexcept -> void
{
    mBufferTracker.begin(mStudio);
}

auto SSRRenderManager::build_scene_nodes(SSRRenderableManagerRef renderableManager) noexcept -> void
{
    //开线程创建
    for (auto&& node : mSceneNodes)
    {
        auto nodePtr = const_cast<SSRSceneGraphNodePtr>(node);
        _SSR_ASSERT(nodePtr, _SSR_RDERROR);
        nodePtr->build_blocks(renderableManager, *this);
    }

    for (auto&& node : mDynamicSceneNodes)
    {
        auto nodePtr = const_cast<SSRSceneGraphNodePtr>(node);
        _SSR_ASSERT(nodePtr, _SSR_RDERROR);
        nodePtr->build_blocks(renderableManager, *this);
    }
}

auto SSRRenderManager::prepare_render_info(SSRRenderableManagerRef renderableManager) noexcept -> void
{
    if (!mPerViewUniformBuffer) 
    {
        _SSR_CORE::RI_BufferInfo bufferInfo{};
        bufferInfo.mDataSize = mPerViewUniform->get_uniform_cpu_buffer()->get_size();
        bufferInfo.mBufferUsage = _SSR_CORE::RE_BufferUsage::eUniformBuffer;
        bufferInfo.mMemoryUsage = _SSR_CORE::RE_MemoryUsage::eCpuToGpu;
        mPerViewUniformBuffer = mStudio.get_token().make_buffer(_SSR_STD::move(bufferInfo));
    }

    if (!mSceneNodes.empty())
    {
        for (auto&& node : mSceneNodes)
        {
            auto nodePtr = const_cast<SSRSceneGraphNodePtr>(node);
            _SSR_ASSERT(nodePtr, _SSR_RDERROR);
            nodePtr->prepare_render_info(*this, renderableManager);
        }
    }
}

auto SSRRenderManager::build_render_command() noexcept -> void
{
    for (auto&& node : mSceneNodes)
    {
        auto nodePtr = const_cast<SSRSceneGraphNodePtr>(node);
        _SSR_ASSERT(nodePtr, _SSR_RDERROR);
        nodePtr->build_render_command(*this);
    }

    _SSR_LIKELY_IF (mPerViewUniformBuffer)
    {

        mPerViewUniformBuffer->upload_buffer(
            mPerViewUniform->get_uniform_cpu_buffer()->get_buffer(),
            mPerViewUniform->get_uniform_cpu_buffer()->get_size());
    }
}

auto SSRRenderManager::collect_transfer_commands() noexcept -> _SSR_CORE::SSRTransferCommandChunkPtr
{
    if (mTransferCommandChunk == nullptr)
    {
        _SSR_CORE::RI_TransferCommandChunkInfo info{};
        mTransferCommandChunk = mStudio.get_token().make_transfer_command_chunk(_SSR_STD::move(info));
    }
    mTransferCommandChunk->clear();

    for (auto&& block : mSceneNodes)
    {
        auto blokePtr = const_cast<SSRSceneGraphNodePtr>(block);
        _SSR_ASSERT(blokePtr, _SSR_RDERROR);
        blokePtr->pike_transfer_command(*this, mTransferCommandChunk);
    }
    return mTransferCommandChunk;
}

auto SSRRenderManager::collect_draw_commands(SUINT32 pipelineIndex, SUINT32 passIndex) noexcept -> _SSR_CORE::SSRDrawCommandChunkPtr
{
    _SSR_ASSERT(pipelineIndex < mRenderPipelines.size(), _SSR_RDERROR);
    _SSR_ASSERT(passIndex < mRenderPipelines[pipelineIndex].mRenderPasses.size(), _SSR_RDERROR);

    auto& pipe = mRenderPipelines[pipelineIndex];
    auto& pass = pipe.mRenderPasses[passIndex];

    if (pass.mDrawCommandChunk == nullptr)
    {
        _SSR_CORE::RI_DrawCommandChunkInfo info{};
        pass.mDrawCommandChunk = mStudio.get_token().make_draw_command_chunk(_SSR_STD::move(info));
    }
    pass.mDrawCommandChunk->clear();

    if (pass.mPassInfo->mConfig.p.usage == SE_PassUsage::eColor || pass.mPassInfo->mConfig.p.usage == SE_PassUsage::eOther)
    {
        for (auto&& block : mSceneNodes)
        {
            auto blokePtr = const_cast<SSRSceneGraphNodePtr>(block);
            _SSR_ASSERT(blokePtr, _SSR_RDERROR);
            blokePtr->pike_draw_command(*this, pass.mDrawCommandChunk, pipelineIndex, passIndex);
        }
    }
    else
    {
        _SSR_ASSERT(pass.mPassEntity, _SSR_RDERROR);
        auto& renderPassInfos =  get_render_pass_infos(pass.mPassEntity);
        auto& passInfo = renderPassInfos[passIndex + mRenderPipelines.size() * pipelineIndex];
        _SSR_ASSERT(passInfo.mSubDrawCommand[get_current_frame_index()], _SSR_RDERROR);
        pass.mDrawCommandChunk->append(passInfo.mSubDrawCommand[get_current_frame_index()]);
    }

    return pass.mDrawCommandChunk;
}

auto SSRRenderManager::end() noexcept -> void
{
    mBufferTracker.end(mStudio);
}

auto SSRRenderManager::make_render_info() noexcept -> SSRRenderManager::Entity
{
    auto e = mRenderInfoManager.create();
    create_component(e);
    return e;
}

auto SSRRenderManager::destroy(Entity e) noexcept -> void
{    
    _SSR_LIKELY_IF(e)
    {
        destroy_component(e);
        mRenderInfoManager.free(e);
    }
}

auto SSRRenderManager::destroy_component(Entity e) noexcept -> void
{

    _SSR::SSRSlice<RenderPassInfo>& renderPassInfos = mRenderInfoManager[e].mRenderPassInfos;
    for (auto& pass : renderPassInfos)
    {
        for(auto& subCmd : pass.mSubDrawCommand)
        {
            if (subCmd)
            {
                mStudio.get_token().delete_sub_draw_command(subCmd);
                subCmd = nullptr;
            }
        }

        for (auto& pipe : pass.mPipeline)
        {
            if (pipe)
            {
                mStudio.get_token().delete_graphics_pipeline(pipe);
                pipe = nullptr;
            }
        }

        for (auto& set : pass.mBindingSet)
        {
            if (set)
            {
                mStudio.get_token().delete_binding_set(set);
                set = nullptr;
            }
        }


        for (auto& layout : pass.mBindingLayout)
        {
            if (layout)
            {
                mStudio.get_token().delete_binding_layout(layout);
                layout = nullptr;
            }
        }

        pass.mDirty = true;
        pass.mRenderGraph = nullptr;
        pass.mPiplineIndex = 0;
        pass.mPassIndex    = 0;
    }
    delete[] renderPassInfos.data();
    renderPassInfos = {};

    RenderTransferCommandInfo& subTransferCommands = mRenderInfoManager[e].mSubTransferCommands;

    for (auto& subCmd : subTransferCommands.mSubDrawCommands)
    {
        if (subCmd)
        {
            mStudio.get_token().delete_sub_transfer_command(subCmd);
            subCmd = nullptr;
        }
    }

    subTransferCommands.mNeedTransfer = true;

    _SSR::SSRSlice<DrawInfo>& drawInfos = mRenderInfoManager[e].mDrawInfos;
    delete[] drawInfos.data();
    drawInfos = {};

    RenderableBufferInfo& renderableBufferInfos = mRenderInfoManager[e].mRenderableBufferInfo;
    if (renderableBufferInfos.mIndexBuffer) 
    {
        mStudio.get_token().delete_buffer(renderableBufferInfos.mIndexBuffer);
        renderableBufferInfos.mIndexBuffer = nullptr;
    }
    if (renderableBufferInfos.mVertexBuffer)
    {
        mStudio.get_token().delete_buffer(renderableBufferInfos.mVertexBuffer);
        renderableBufferInfos.mVertexBuffer = nullptr;
    }
    if (renderableBufferInfos.mRenderableSsbo)
    {
        mStudio.get_token().delete_buffer(renderableBufferInfos.mRenderableSsbo);
        renderableBufferInfos.mRenderableSsbo = nullptr;
    }

    PrimitiveInfo& primitiveInfo = mRenderInfoManager[e].mPrimitiveInfo;
    primitiveInfo = {};
}

auto SSRRenderManager::create_component(Entity e) noexcept -> void
{
    _SSR::SSRSlice<RenderPassInfo>& renderPassInfos = mRenderInfoManager[e].mRenderPassInfos;

    _SSR_ASSERT(!mRenderPipelines.empty(), _SSR_RDERROR);

    SUINT32 passCount{ 0 };

    for (auto& pipe : mRenderPipelines)
    {
        _SSR_ASSERT(!pipe.mRenderPasses.empty(), _SSR_RDERROR);
        passCount += cast<SUINT32>(pipe.mRenderPasses.size());
    }

    auto* renderPasses = new RenderPassInfo[passCount];
    renderPassInfos = { renderPasses, passCount };

    for (SUINT32 i = 0; i < mRenderPipelines.size(); i++)
    {
        for (SUINT32 j = 0; j < mRenderPipelines[i].mRenderPasses.size(); j++)
        {
            auto& pass = renderPassInfos[j + (mRenderPipelines.size() - 1) * i];

            for (auto& sub : pass.mSubDrawCommand)
            {
                sub = nullptr;
            }

            for (auto& pipe : pass.mPipeline)
            {
                pipe = nullptr;
            }

            for (auto& layout : pass.mBindingLayout)
            {
                layout = nullptr;
            }

            for (auto& set : pass.mBindingSet)
            {
                set = nullptr;
            }

            pass.mRenderGraph = mRenderPipelines[i].mRenderGraph;
            pass.mDirty = true;
            pass.mPiplineIndex = i;
            pass.mPassIndex = j;
        }
    }

    RenderTransferCommandInfo& subTransferCommands = mRenderInfoManager[e].mSubTransferCommands;
    for (auto& subCmd : subTransferCommands.mSubDrawCommands)
    {
        subCmd = nullptr;
    }

    subTransferCommands.mNeedTransfer = false;

    _SSR::SSRSlice<DrawInfo>& drawInfos = mRenderInfoManager[e].mDrawInfos;
    drawInfos = {};

    RenderableBufferInfo& renderableBufferInfos = mRenderInfoManager[e].mRenderableBufferInfo;
    renderableBufferInfos.mRenderableSsbo = nullptr;
    renderableBufferInfos.mIndexBuffer = nullptr;
    renderableBufferInfos.mVertexBuffer = nullptr;

    PrimitiveInfo& primitiveInfo = mRenderInfoManager[e].mPrimitiveInfo;
    primitiveInfo = {};
}

auto SSRRenderManager::get_render_pass_infos(Entity e) noexcept -> _SSR::SSRSlice<RenderPassInfo>&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mRenderInfoManager[e].mRenderPassInfos;
}

auto SSRRenderManager::get_render_pass_infos(Entity e) const noexcept -> const _SSR::SSRSlice<RenderPassInfo>&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mRenderInfoManager[e].mRenderPassInfos;
}

auto SSRRenderManager::get_render_transfer_command_info(Entity e) noexcept -> RenderTransferCommandInfo&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mRenderInfoManager[e].mSubTransferCommands;
}
auto SSRRenderManager::get_draw_infos(Entity e) noexcept -> _SSR::SSRSlice<DrawInfo>&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mRenderInfoManager[e].mDrawInfos;
}
auto SSRRenderManager::get_renderable_buffer_info(Entity e) noexcept -> RenderableBufferInfo&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mRenderInfoManager[e].mRenderableBufferInfo;
}

auto SSRRenderManager::get_primitive_info(Entity e) noexcept -> PrimitiveInfo&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mRenderInfoManager[e].mPrimitiveInfo;
}

_SSR_END

