#include "inl/SSRSceneGraphNodeToolbox.inl"

_SSR_STUDIO_BEGIN

SSRSceneGraphNode::SSRSceneGraphNode(SSRPrimitiveManagerInRef primitiveManager) noexcept
    : mPrimitiveManager{ primitiveManager }
{
    mProcesseEntities.reserve(SSRSceneGraphNode::eContainerSize);
}

SSRSceneGraphNode::~SSRSceneGraphNode() noexcept
{
    _SSR_ASSERT(mEntities.empty(), _SSR_RDERROR);
}

auto SSRSceneGraphNode::add_entity(PrimitiveEntity entity) noexcept ->void
{
    _SSR_LIKELY_IF(!has(mEntities, entity))
    {
        mEntities.insert(entity);
        mProcesseEntities.push_back(entity);
    }
    else
    {
        _SSR_ASSERT(0, _SSR_RDERROR);
    }
}

auto SSRSceneGraphNode::modify_entity(PrimitiveEntity entity) noexcept -> void
{
    _SSR_LIKELY_IF(has(mEntities, entity))
    {
        mProcesseEntities.push_back(entity);
    }
    else
    {
        _SSR_ASSERT(0, _SSR_RDERROR);
    }
}
auto SSRSceneGraphNode::delete_entity(PrimitiveEntity entity) noexcept -> void
{
    _SSR_LIKELY_IF (has(mEntities, entity)) 
    {
#ifdef _SSR_STUDIO_DEBUG_PROCESSE
        for (auto e : mProcesseEntities) 
        {
            if (e == entity) 
            {
                _SSR_ASSERT(0, _SSR_RDERROR);
                break;
            }
        }
#endif
        mProcesseEntities.push_back(entity);
        mEntities.erase(entity);
        mPrimitiveManager.set_entity_removed(entity);
    }
    else
    {
        _SSR_ASSERT(0, _SSR_RDERROR);
    }
}

auto SSRSceneGraphNode::build_blocks(SSRRenderableManagerRef renderableManager, SSRRenderManagerRef renderManager) noexcept -> void
{

    BuildBlockPackage package{ mPrimitiveManager,
                               renderableManager,
                               renderManager,
                               mDirtyFlags,
                               mEntities,
                               mProcesseEntities,
                               mRenderBlockGroup };

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Build_Blocks;
}

auto SSRSceneGraphNode::prepare_render_info(SSRRenderManagerRef renderManager, SSRRenderableManagerRef renderableManager) noexcept -> void
{
    PrepareBlockPackage package{ renderManager,
                                 renderableManager,
                                 mRenderBlockGroup };

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Prepare_Blocks;
}

auto SSRSceneGraphNode::build_render_command(SSRRenderManagerRef renderManager) noexcept -> void
{
    BuildBlockCommandPackage package{ mPrimitiveManager,
                                      renderManager,
                                      mRenderBlockGroup };

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Build_Render_Commands;
}

auto SSRSceneGraphNode::pike_draw_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRDrawCommandChunkPtr commands, SUINT32 pipelineIndex, SUINT32 passIndex) noexcept -> void
{
    for (SUINT32 i = 0; i < cast<SUINT32>(SSRSceneGraphNode::E_PerRenderBlockGroup::eCount); i++)
    {
        auto& renderBlock = mRenderBlockGroup[i];
        renderBlock.pike_draw_command(renderManager, commands, pipelineIndex, passIndex);
    }
}

auto SSRSceneGraphNode::pike_transfer_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRTransferCommandChunkPtr commands) noexcept -> void
{
    for (SUINT32 i = 0; i < cast<SUINT32>(SSRSceneGraphNode::E_PerRenderBlockGroup::eCount); i++)
    {
        auto& renderBlock = mRenderBlockGroup[i];
        renderBlock.pike_transfer_command(renderManager, commands);
    }
}

auto SSRSceneGraphNode::clear(SSRRenderManagerRef renderManager) noexcept -> void
{
    for (SUINT32 i = 0; i < cast<SUINT32>(SSRSceneGraphNode::E_PerRenderBlockGroup::eCount); i++)
    {
        auto& renderBlock = mRenderBlockGroup[i];
        renderBlock.clear(renderManager);
    }

    for (auto&& e : mEntities)
    {
        mPrimitiveManager.destroy(e);
    }
    mEntities.clear();
}

_SSR_END

