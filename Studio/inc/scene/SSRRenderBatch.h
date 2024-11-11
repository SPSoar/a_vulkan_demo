#pragma once
#ifndef SSR_SSRRENDERBATCH_H_
#define SSR_SSRRENDERBATCH_H_

#include "../common/SSRStudioHeadIn.h"
//#include "studio/SSRStudioIn.h"
#include "manager/SSRPrimitiveManagerIn.h"
#include "manager/SSRRenderManager.h"
#include "manager/SSRRenderableManager.h"
#include "studio/SSRMaterialInstanceIn.h"
#include "geometry/modeling/SSRGeometricModelingEngine.h"

_SSR_STUDIO_BEGIN

enum class BatchMode : SENUM8
{
    eUnknown = 0,
    eDefault,
    eInstance,
    eIndirect
};

enum class BatchDrawMode : SENUM8
{
    eUnknown = 0,
    eDefault,
    eBatch,
    eConsolidating
};

template<BatchMode Mode, BatchDrawMode DrawMode>
class SSRRenderBatch
{
public:
    using PrimitiveEntity  = SSRPrimitiveManager::Entity;
    using RenderableEntity = SSRRenderableManager::Entity;
    using RenderEntity     = SSRRenderManager::Entity;
    using BatchId          = _SSR_STD::conditional_t<Mode == BatchMode::eIndirect, SSRMaterialInSPtr, SSRMaterialInstanceSPtr>;

    struct BatchInfo
    {
        static constexpr BatchMode        eBatchMode = Mode;
        static constexpr BatchDrawMode    eBatchDrawMode = DrawMode;
    };

    _SSR_PACKAGE_DEF(Renderable,
                     BatchInfo, BatchInfo,
                     SSRGeometricModelingEngine&, ModelingEngine,
                     SSRPrimitiveManagerInRef, PrimitiveManager,
                     SSRRenderableManagerRef, RenderableManager,
                     SSRRenderManagerRef, RenderManager,
                     RenderEntity&, RenderEntity,
                     SSRVector<RenderableEntity>&, RenderableEntities,
                     const SSRVector<PrimitiveEntity>&, PrimitiveEntities);

    _SSR_PACKAGE_DEF(RenderRes,
                     BatchInfo, BatchInfo,
                     BatchId, BatchId,
                     const SSRVector<PrimitiveEntity>&, PrimitiveEntities,
                     SSRRenderManagerRef, RenderManager,
                     SSRRenderableManagerRef, RenderableManager,
                     RenderEntity&, RenderEntity,
                     const SSRVector<RenderableEntity>&, RenderableEntities);

    _SSR_PACKAGE_DEF(RenderCommand,
                     BatchInfo, BatchInfo,
                     BatchId, BatchId,
                     SSRVector<RenderableEntity>&, RenderableEntities,
                     SSRRenderableManagerRef, RenderableManager,
                     const SSRVector<PrimitiveEntity>&, PrimitiveEntities,
                     SSRPrimitiveManagerInRef, PrimitiveManager,
                     RenderEntity&, RenderEntity,
                     SSRRenderManagerRef, RenderManager);

    class BatchBlock
    {
        friend class SSRRenderBatch<Mode, DrawMode>;
    public:
        BatchBlock() noexcept = delete;
        BatchBlock(SSRRenderableManagerRef renderableManager, BatchId id) noexcept
            : mRenderableManager{ renderableManager }
            , mBatchId{ id }
        {
        
        }
        ~BatchBlock() noexcept
        {
            _SSR_ASSERT(mRenderableEntities.empty(), _SSR_RDERROR);
            _SSR_ASSERT(!mRenderEntity, _SSR_RDERROR);
        }
        _SSR_CLASS_DELETE_COPY(BatchBlock);
        _SSR_CLASS_DELETE_MOVE(BatchBlock);
    public:
        auto clear(SSRRenderManagerRef renderManager) noexcept 
        {
            for (auto e : mRenderableEntities)
            {
                mRenderableManager.destroy(e);
            }
            mRenderableEntities.clear();
            mRenderableEntities.shrink_to_fit();

            renderManager.destroy(mRenderEntity);
            mRenderEntity.reset();
        }
        auto add_primitive_entity(PrimitiveEntity e) noexcept 
        {
            _SSR_STD::scoped_lock<SSRMutex> lock(mLock);
            mPrimitiveEntities.push_back(e);
        }
        auto make_renderable(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept;

        auto prepare_render_info(SSRRenderManagerRef renderManager) noexcept;
        auto prepare_render_pipeline(SSRRenderManagerRef renderManager) noexcept;
        auto build_render_command(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept;
        auto build_transfer_command(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept;
        auto pike_draw_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRDrawCommandChunkPtr commands, SUINT32 pipelineIndex, SUINT32 passIndex) noexcept;
        auto pike_transfer_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRTransferCommandChunkPtr commands) noexcept;
    protected:
        SSRGeometricModelingEngine  mModelingEngine{};
        SSRRenderableManagerRef     mRenderableManager;
        BatchId                     mBatchId{};
        SSRMutex                    mLock{};
        SSRVector<PrimitiveEntity>  mPrimitiveEntities{};
        SSRVector<RenderableEntity> mRenderableEntities{};
        RenderEntity                mRenderEntity{};
    };
    _SSR_PR_DEF_S(BatchBlock);

    using BatchMap = SSRMap<BatchId, BatchBlockPtr>;
public:
    SSRRenderBatch() noexcept = default;
    ~SSRRenderBatch() noexcept
    {
        _SSR_ASSERT(mBatchMap.empty(), _SSR_RDERROR);
    }
    _SSR_CLASS_DELETE_COPY(SSRRenderBatch);
    _SSR_CLASS_DELETE_MOVE(SSRRenderBatch);
public:
    auto batch(SSRRenderableManagerRef renderableManager, SSRMaterialInstanceSPtr materialInstance, PrimitiveEntity entity) noexcept
    {
        _SSR_ASSERT(materialInstance, _SSR_RDERROR);

        BatchId id = nullptr;
        if constexpr (Mode == BatchMode::eIndirect) 
        {
            auto materialInstanceIn = upcast<SSRMaterialInstanceInSPtr>(materialInstance);
            id = materialInstanceIn->get_material();
        }
        else 
        {
            id = materialInstance;
        }

        BatchBlockPtr block = nullptr;
        {
            _SSR_STD::scoped_lock<SSRMutex> lock(mLock);
            if (mBatchMap.find(id) == mBatchMap.end()) 
            {
                _SSR_CREATE_OBJECT_PTR(block, BatchBlock, renderableManager, id);
                mBatchMap[id] = block;
            }
            else 
            {
                block = mBatchMap[id];
            }
        }
        _SSR_ASSERT(block, _SSR_RDERROR);
        block->add_primitive_entity(entity);
    }
    auto clear(SSRRenderManagerRef renderManager) noexcept
    {
        for (auto it = mBatchMap.begin(); it != mBatchMap.end(); ++it)
        {
            it.value()->clear(renderManager);
            _SSR_DELETE_OBJECT_PTR(it.value());
        }
        mBatchMap.clear();
    }
    auto empty() const noexcept
    {
        return mBatchMap.empty();
    }

    auto make_renderable(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept
    {
        for (auto it = mBatchMap.begin(); it != mBatchMap.end(); ++it)
        {
            _SSR_ASSERT(it.value(), _SSR_RDERROR);
            it.value()->make_renderable(primitiveManager, renderManager);
        }
    }

    auto prepare_render_info(SSRRenderManagerRef renderManager) noexcept
    {
        for (auto it = mBatchMap.begin(); it != mBatchMap.end(); ++it)
        {
            _SSR_ASSERT(it.value(), _SSR_RDERROR);
            it.value()->prepare_render_info(renderManager);
        }
    }

    auto prepare_render_pipeline(SSRRenderManagerRef renderManager) noexcept
    {
        for (auto it = mBatchMap.begin(); it != mBatchMap.end(); ++it)
        {
            _SSR_ASSERT(it.value(), _SSR_RDERROR);
            it.value()->prepare_render_pipeline(renderManager);
        }
    }
    auto build_render_command(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept -> void
    {
        for (auto it = mBatchMap.begin(); it != mBatchMap.end(); ++it)
        {
            _SSR_ASSERT(it.value(), _SSR_RDERROR);
            it.value()->build_render_command(primitiveManager, renderManager);
        }
    }

    auto build_transfer_command(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept -> void
    {
        for (auto it = mBatchMap.begin(); it != mBatchMap.end(); ++it)
        {
            _SSR_ASSERT(it.value(), _SSR_RDERROR);
            it.value()->build_transfer_command(primitiveManager, renderManager);
        }
    }

    auto pike_draw_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRDrawCommandChunkPtr commands, SUINT32 pipelineIndex, SUINT32 passIndex) noexcept
    {
        for (auto it = mBatchMap.begin(); it != mBatchMap.end(); ++it)
        {
            _SSR_ASSERT(it.value(), _SSR_RDERROR);
            it.value()->pike_draw_command(renderManager, commands, pipelineIndex, passIndex);
        }
    }

    auto pike_transfer_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRTransferCommandChunkPtr commands) noexcept
    {
        for (auto it = mBatchMap.begin(); it != mBatchMap.end(); ++it)
        {
            _SSR_ASSERT(it.value(), _SSR_RDERROR);
            it.value()->pike_transfer_command(renderManager, commands);
        }
    }
private:
    BatchMap     mBatchMap{};
    SSRMutex     mLock{};
};


#define _SSR_RENDER_BATCH_OBJECT(_SSR_DEFINER)\
    _SSR_DEFINER(Default, Default)\
    _SSR_DEFINER(Default, Batch)\
    _SSR_DEFINER(Default, Consolidating)\
    _SSR_DEFINER(Instance, Default)\
    _SSR_DEFINER(Instance, Batch)\
    _SSR_DEFINER(Instance, Consolidating)\
    _SSR_DEFINER(Indirect, Default)\
    _SSR_DEFINER(Indirect, Batch)\
    _SSR_DEFINER(Indirect, Consolidating)\


#define _SSR_RENDER_BATCH_TYPE_DEF(Mode, DrawMode)\
    using RenderBatchMode##Mode##Draw##DrawMode = SSRRenderBatch<BatchMode::e##Mode, BatchDrawMode::e##DrawMode>;\


#define _SSR_RENDER_BATCH_ENUM_DEF(Mode, DrawMode)\
    eMode##Mode##Draw##DrawMode,

#define _SSR_RENDER_BATCH_VARIANT_DEF(Mode, DrawMode)\
    RenderBatchMode##Mode##Draw##DrawMode,


_SSR_RENDER_BATCH_OBJECT(_SSR_RENDER_BATCH_TYPE_DEF);

enum class E_RenderBatchPolicy : SENUM8
{
    _SSR_RENDER_BATCH_OBJECT(_SSR_RENDER_BATCH_ENUM_DEF)
    eCount
};

using SSRRenderBlockUnknown = SSRRenderBatch<BatchMode::eUnknown, BatchDrawMode::eUnknown>;

_SSR_VARIANT_DEF(RenderBatch,
                 _SSR_RENDER_BATCH_OBJECT(_SSR_RENDER_BATCH_VARIANT_DEF)
                  SSRRenderBlockUnknown)
_SSR_END

#include "inl/SSSRRenderBatchToolbox.inl"

_SSR_STUDIO_BEGIN

template<BatchMode Mode, BatchDrawMode DrawMode>
inline auto SSRRenderBatch<Mode, DrawMode>::BatchBlock::make_renderable(SSRPrimitiveManagerInRef primitiveManager,
                                                                          SSRRenderManagerRef renderManager) noexcept
{
    RenderablePackage package{ BatchInfo(),
                               mModelingEngine,
                               primitiveManager,
                               mRenderableManager,
                               renderManager,
                               mRenderEntity,
                               mRenderableEntities,
                               mPrimitiveEntities};
    _SSR::SSRFlowMotionlessWrapper(package) | CF_Render_Block_Build_Renderable;
}

template<BatchMode Mode, BatchDrawMode DrawMode>
inline auto SSRRenderBatch<Mode, DrawMode>::BatchBlock::prepare_render_info(SSRRenderManagerRef renderManager) noexcept
{
    RenderResPackage package{ BatchInfo(),
                              mBatchId,
                              mPrimitiveEntities,
                              renderManager,
                              mRenderableManager,
                              mRenderEntity,
                              mRenderableEntities};

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Render_Block_Prepare_Render_Res;
}

template<BatchMode Mode, BatchDrawMode DrawMode>
inline auto SSRRenderBatch<Mode, DrawMode>::BatchBlock::prepare_render_pipeline(SSRRenderManagerRef renderManager) noexcept
{
    RenderResPackage package{ BatchInfo(),
                              mBatchId,
                              mPrimitiveEntities,
                              renderManager,
                              mRenderableManager,
                              mRenderEntity,
                              mRenderableEntities};

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Render_Block_Prepare_Render_Pipeline;
}

template<BatchMode Mode, BatchDrawMode DrawMode>
inline auto SSRRenderBatch<Mode, DrawMode>::BatchBlock::build_render_command(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept
{
    RenderCommandPackage package{ BatchInfo(),
                                  mBatchId,
                                  mRenderableEntities,
                                  mRenderableManager,
                                  mPrimitiveEntities,
                                  primitiveManager,
                                  mRenderEntity,
                                  renderManager };

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Render_Block_Build_Render_Command;
}

template<BatchMode Mode, BatchDrawMode DrawMode>
inline auto SSRRenderBatch<Mode, DrawMode>::BatchBlock::build_transfer_command(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept
{
    RenderCommandPackage package{ BatchInfo(),
                                  mBatchId,
                                  mRenderableEntities,
                                  mRenderableManager,
                                  mPrimitiveEntities,
                                  primitiveManager,
                                  mRenderEntity,
                                  renderManager };

    _SSR::SSRFlowMotionlessWrapper(package) | CF_Render_Block_Build_Transfer_Command;
}

template<BatchMode Mode, BatchDrawMode DrawMode>
inline auto SSRRenderBatch<Mode, DrawMode>::BatchBlock::pike_draw_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRDrawCommandChunkPtr commands, SUINT32 pipelineIndex, SUINT32 passIndex) noexcept
{
    auto index = pipelineIndex * renderManager.get_render_pipelines().size() + passIndex;

    auto&& renderPassInfos = renderManager.get_render_pass_infos(mRenderEntity);

    _SSR_ASSERT(index < renderPassInfos.size(), _SSR_RDERROR);

    auto&& drawCommands = renderPassInfos[index].mSubDrawCommand;

    auto& command = drawCommands[renderManager.get_current_frame_index()];
    _SSR_ASSERT(command != nullptr, _SSR_RDERROR);

    commands->append(command);
}

template<BatchMode Mode, BatchDrawMode DrawMode>
inline auto SSRRenderBatch<Mode, DrawMode>::BatchBlock::pike_transfer_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRTransferCommandChunkPtr commands) noexcept
{
    auto& rm = const_cast<SSRRenderManagerRef>(renderManager);

    auto& subTransferCommands = rm.get_render_transfer_command_info(mRenderEntity);
    auto& transferCommand = subTransferCommands.mSubDrawCommands[renderManager.get_current_frame_index()];

    if(subTransferCommands.mNeedTransfer && transferCommand)
    {
        commands->append(transferCommand);
        subTransferCommands.mNeedTransfer = false;
    }
}
_SSR_END


#endif

