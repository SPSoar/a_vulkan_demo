#pragma once
#ifndef SSR_SSRSCENEGRAPHNODE_H_
#define SSR_SSRSCENEGRAPHNODE_H_

#include "../common/SSRStudioHeadIn.h"
#include "SSRRenderBlock.h"


_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRSceneGraphNode);

class SSRSceneGraphNode
{
    friend class SSRSceneOcTreeNode;
    friend class SSRSceneGraphRootStatic;
    friend class SSRSceneGraphRootDynamic;
    friend class SSRSceneGraphRootFlash;
public:
    using PrimitiveEntity  = SSRPrimitiveManager::Entity;
    using RenderableEntity = SSRRenderableManager::Entity;
    using EntitySet        = SSRSet<PrimitiveEntity, PrimitiveEntity::Hasher>;
    enum class E_PerRenderBlockGroup : SENUM8
    {
        eOpaque = 0,
        eTranslucent,
        eCount
    };
    static constexpr SCINT eContainerSize = 100;

    using RenderBlockGroup = SSRFixVector<SSRRenderBlock, cast<size_t>(E_PerRenderBlockGroup::eCount)>;

#define  _SSR_FLAGS_COMBINATION_SCENE_BLOCK_DIRTY_DEF(_SSR_DEFINER)\
    _SSR_DEFINER(All, all, Opaque, Translucent)\

    _SSR_FLAGS_DEF(E_SceneNodeDirty,
                   _SSR_FLAGS_COMBINATION_SCENE_BLOCK_DIRTY_DEF(_SSR_FLAGS_COMBINATION_DEF),
                   PerRenderableSsbo, per_renderable_ssbo_dirty,
                   Opaque, opaque_dirty,
                   Translucent, translucent_dirty);

    _SSR_PACKAGE_DEF(BuildBlock,
                     SSRPrimitiveManagerInRef, PrimitiveManager,
                     SSRRenderableManagerRef, RenderableManager,
                     SSRRenderManagerRef, RenderManager,
                     E_SceneNodeDirtyFlags&, DirtyFlags,
                     EntitySet&, EntitySet,
                     SSRVector<PrimitiveEntity>&, ProcesseEntities,
                     RenderBlockGroup&, RenderBlockGroup);

    _SSR_PACKAGE_DEF(PrepareBlock,
                     SSRRenderManagerRef, RenderManager,
                     SSRRenderableManagerRef, RenderableManager,
                     RenderBlockGroup&, RenderBlockGroup);

    _SSR_PACKAGE_DEF(BuildBlockCommand,
                     SSRPrimitiveManagerInRef, PrimitiveManager,
                     SSRRenderManagerRef, RenderManager,
                     RenderBlockGroup&, RenderBlockGroup);

public:
    _SSR_CLASS_DELETE_COPY(SSRSceneGraphNode);
    _SSR_CLASS_DELETE_MOVE(SSRSceneGraphNode);
protected:
    SSRSceneGraphNode() noexcept = delete;
    explicit SSRSceneGraphNode(SSRPrimitiveManagerInRef primitiveManager) noexcept;
    ~SSRSceneGraphNode() noexcept;
protected:
    auto add_entity(PrimitiveEntity entity) noexcept ->void;
    auto modify_entity(PrimitiveEntity entity) noexcept ->void;
    auto delete_entity(PrimitiveEntity entity) noexcept ->void;
    auto is_block_empty() const noexcept -> bool 
    {
        return mEntities.empty();
    }
public:
    auto get_boundingbox() const noexcept -> SSRBoundingBox 
    {
        return SSRBoundingBox(-1, -1, -1, 1, 1, 1);
    }
    auto get_per_render_block_uniform() const noexcept -> SSRPerRenderBlockUniformCPtr
    {
        return nullptr;
    }
    auto get_per_renderable_uniform() const noexcept -> SSRPerRenderableUniformCPtr
    {
        return nullptr;
    }
    auto build_blocks(SSRRenderableManagerRef renderableManager, SSRRenderManagerRef renderManager) noexcept -> void;

    auto prepare_render_info(SSRRenderManagerRef renderManager, SSRRenderableManagerRef renderableManager) noexcept -> void;
    auto build_render_command(SSRRenderManagerRef renderManager) noexcept -> void;

    auto pike_draw_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRDrawCommandChunkPtr commands, SUINT32 pipelineIndex, SUINT32 passIndex) noexcept -> void;
    auto pike_transfer_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRTransferCommandChunkPtr commands) noexcept -> void;

    auto clear(SSRRenderManagerRef renderManager) noexcept -> void;
private:
    SSRPrimitiveManagerInRef   mPrimitiveManager;
    EntitySet                  mEntities{};
    SSRVector<PrimitiveEntity> mProcesseEntities{};
    RenderBlockGroup           mRenderBlockGroup{};
    E_SceneNodeDirtyFlags      mDirtyFlags{ 0 };
};

_SSR_END

#endif

