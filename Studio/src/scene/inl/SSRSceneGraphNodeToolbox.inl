#include "scene/SSRSceneGraphNode.h"
#include "manager/SSRPrimitiveManagerIn.h"

_SSR_STUDIO_BEGIN

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Check_Bolck_State)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(dirtyFlags, SSRSceneGraphNode::BuildBlock::eDirtyFlags);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveManager, SSRSceneGraphNode::BuildBlock::ePrimitiveManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(processeEntities, SSRSceneGraphNode::BuildBlock::eProcesseEntities);

    dirtyFlags.reset();
    
    // 可以多线程
    for(auto e : processeEntities)
    {
        using StateFlags = SSRPrimitiveManagerIn::E_PrimitiveStateFlags;
        StateFlags stateFlags(primitiveManager.get_entity_state(e));

        _SSR_ASSERT(stateFlags.any(), _SSR_RDERROR);

        if (dirtyFlags.has_all())
        {
            primitiveManager.clear_entity_state(e);
            continue;
        }

        const auto& highLightState = primitiveManager.get_high_light_state(e);

        auto materialIns = primitiveManager.get_material_instance(e);
        _SSR_ASSERT(materialIns, _SSR_RDERROR);

        if (stateFlags.has_removed() || stateFlags.has_added())
        {
            if (materialIns->get_shading_type() == E_Shading::eTranslucent) 
            {
                dirtyFlags.set_translucent_dirty(true);
                dirtyFlags.set_per_renderable_ssbo_dirty(true);
            }
            else 
            {
                dirtyFlags.set_opaque_dirty(true);
                dirtyFlags.set_per_renderable_ssbo_dirty(true);
            }
        }
        else if(stateFlags.has_material_changed() || stateFlags.has_block_changed())
        {
            dirtyFlags.set_translucent_dirty(true);
            dirtyFlags.set_opaque_dirty(true);
            dirtyFlags.set_per_renderable_ssbo_dirty(true);
        }
        else if (stateFlags.has_high_light_changed() ||
                 stateFlags.has_clip() ||
                 stateFlags.has_tranform_changed() ||
                 stateFlags.has_visible())
        {
            dirtyFlags.set_per_renderable_ssbo_dirty(true);
        }
        if (stateFlags.has_removed()) 
        {
            primitiveManager.destroy(e);
        }
        else 
        {
            primitiveManager.clear_entity_state(e);
        }
    }
    processeEntities.clear();
    processeEntities.reserve(SSRSceneGraphNode::eContainerSize);
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Prepare_Render_Block)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(dirtyFlags, SSRSceneGraphNode::BuildBlock::eDirtyFlags);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveManager, SSRSceneGraphNode::BuildBlock::ePrimitiveManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(entitySet, SSRSceneGraphNode::BuildBlock::eEntitySet);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderBlockGroup, SSRSceneGraphNode::BuildBlock::eRenderBlockGroup);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableManager, SSRSceneGraphNode::BuildBlock::eRenderableManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRSceneGraphNode::BuildBlock::eRenderManager);

    auto& opaqueBlock = renderBlockGroup[cast<size_t>(SSRSceneGraphNode::E_PerRenderBlockGroup::eOpaque)];
    auto& translucentBlock = renderBlockGroup[cast<size_t>(SSRSceneGraphNode::E_PerRenderBlockGroup::eTranslucent)];

    if (dirtyFlags.has_opaque_dirty())
    {
        opaqueBlock.clear(renderManager);
    }

    if (dirtyFlags.has_translucent_dirty())
    {
        translucentBlock.clear(renderManager);
    }

    //可以开线程写
    if (dirtyFlags.has_opaque_dirty() || dirtyFlags.has_translucent_dirty())
    {
        for (auto iter = entitySet.begin(); iter != entitySet.end(); ++iter)
        {
            auto&& e = iter.key();

            auto&& materialInstanceIn = primitiveManager.get_material_instance(e);
     
            if (dirtyFlags.has_opaque_dirty() && materialInstanceIn->get_shading_type() != E_Shading::eTranslucent)
            {
                opaqueBlock.add_entity(renderableManager, materialInstanceIn, e);
            }
            else if(dirtyFlags.has_translucent_dirty())
            {
                translucentBlock.add_entity(renderableManager, materialInstanceIn, e);
            }
        }
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Build_Render_Block)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(dirtyFlags, SSRSceneGraphNode::BuildBlock::eDirtyFlags);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveManager, SSRSceneGraphNode::BuildBlock::ePrimitiveManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableManager, SSRSceneGraphNode::BuildBlock::eRenderableManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRSceneGraphNode::BuildBlock::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(entitySet, SSRSceneGraphNode::BuildBlock::eEntitySet);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderBlockGroup, SSRSceneGraphNode::BuildBlock::eRenderBlockGroup);

    for (SUINT32 i = 0; i < cast<SUINT32>(SSRSceneGraphNode::E_PerRenderBlockGroup::eCount); i++)
    {
        auto& renderBlock = renderBlockGroup[i];
        renderBlock.build_blocks(primitiveManager, renderManager);
    }

_SSR_MOTIONLESS_FLOW_IFUNC_END

_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Prepare_Block_Render_Info)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderBlockGroup, SSRSceneGraphNode::PrepareBlock::eRenderBlockGroup);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRSceneGraphNode::PrepareBlock::eRenderManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderableManager, SSRSceneGraphNode::PrepareBlock::eRenderableManager);

    for (SUINT32 i = 0; i < cast<SUINT32>(SSRSceneGraphNode::E_PerRenderBlockGroup::eCount); i++)
    {
        auto& renderBlock = renderBlockGroup[i];
        renderBlock.prepare_render_info(renderManager);
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END


_SSR_MOTIONLESS_FLOW_IFUNC_BEGIN(CF_Build_Block_Render_Commands)
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderBlockGroup, SSRSceneGraphNode::BuildBlockCommand::eRenderBlockGroup);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(primitiveManager, SSRSceneGraphNode::BuildBlockCommand::ePrimitiveManager);
    _SSR_MOTIONLESS_FLOW_IFUNC_PAR(renderManager, SSRSceneGraphNode::BuildBlockCommand::eRenderManager);

    for (SUINT32 i = 0; i < cast<SUINT32>(SSRSceneGraphNode::E_PerRenderBlockGroup::eCount); i++)
    {
        auto& renderBlock = renderBlockGroup[i];
        renderBlock.build_render_command(primitiveManager, renderManager);
    }
_SSR_MOTIONLESS_FLOW_IFUNC_END


_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Build_Blocks)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Check_Bolck_State)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Prepare_Render_Block)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Build_Render_Block)
_SSR_MOTIONLESS_FLOW_END


_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Prepare_Blocks)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Prepare_Block_Render_Info)
_SSR_MOTIONLESS_FLOW_END

_SSR_MOTIONLESS_FLOW_ABEGIN(CF_Build_Render_Commands)
_SSR_MOTIONLESS_FLOW_APPEND(CF_Build_Block_Render_Commands)
_SSR_MOTIONLESS_FLOW_END


_SSR_END
