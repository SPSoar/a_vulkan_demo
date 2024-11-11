#pragma once
#ifndef SSR_SSRRENDERBLOCK_H_
#define SSR_SSRRENDERBLOCK_H_

#include "../common/SSRStudioHeadIn.h"
#include "SSRRenderBatch.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRRenderBlock);
_SSR_PR_DEF(SSRRenderManager);

enum class E_RenderBlockType : SENUM8
{
    eSurface,
    eWireFrame,
    eTransparentSurface,
    eTransparentWireFrame,
    eCount
};

class SSRRenderBlock
{
    friend class SSRSceneGraphNode;
public:
    using PrimitiveEntity  = SSRPrimitiveManager::Entity;
    using RenderableEntity = SSRRenderableManager::Entity;
public:
    SSRRenderBlock() noexcept;
    ~SSRRenderBlock() noexcept;

    _SSR_CLASS_DELETE_COPY(SSRRenderBlock);
    _SSR_CLASS_DELETE_MOVE(SSRRenderBlock);
public:
    auto clear(SSRRenderManagerRef renderManager) noexcept -> void;
    auto add_entity(SSRRenderableManagerRef renderableManager, SSRMaterialInstanceSPtr materialInstance, PrimitiveEntity entity) noexcept ->void;

    auto build_blocks(SSRPrimitiveManagerInRef  primitiveManager, SSRRenderManagerRef renderManager) noexcept -> void;

    auto prepare_render_info(SSRRenderManagerRef renderManager) noexcept -> void;
    auto build_render_command(SSRPrimitiveManagerInRef  primitiveManager, SSRRenderManagerRef renderManager) noexcept -> void;
    auto pike_draw_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRDrawCommandChunkPtr commands, SUINT32 pipelineIndex, SUINT32 passIndex) noexcept -> void;
    auto pike_transfer_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRTransferCommandChunkPtr commands) noexcept -> void;
private:
    RenderBatch _SSR_ATOM_VAL;
    SBOOL       mDirty{ false };
private:
};

_SSR_END

#endif

