#include "scene/SSRRenderBlock.h"
#include "manager/SSRRenderManager.h"

_SSR_STUDIO_BEGIN

SSRRenderBlock::SSRRenderBlock() noexcept 
    : _SSR_ATOM_VAL{ _SSR_STD::in_place_index<cast<size_t>(E_RenderBatchPolicy::eModeDefaultDrawDefault)> }
{

}


SSRRenderBlock::~SSRRenderBlock() noexcept
{
}

auto SSRRenderBlock::clear(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, clear, renderManager);
    mDirty = true;
}

auto SSRRenderBlock::add_entity(SSRRenderableManagerRef renderableManager, SSRMaterialInstanceSPtr materialInstance, SSRRenderBlock::PrimitiveEntity entity) noexcept ->void
{
    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, batch, renderableManager, materialInstance, entity);
    mDirty = true;
}

auto SSRRenderBlock::build_blocks(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept -> void
{
    if (!mDirty || _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, empty))
    {
        return;
    }
    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, make_renderable, primitiveManager, renderManager);
}

auto SSRRenderBlock::prepare_render_info(SSRRenderManagerRef renderManager) noexcept -> void
{
    if ((!mDirty && !renderManager.get_state().has_window_changed()) || _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, empty))
    {
        return;
    }

    if (!renderManager.get_state().has_window_changed()) 
    {
        _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, prepare_render_info, renderManager);
    }

    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, prepare_render_pipeline, renderManager);
}

auto SSRRenderBlock::build_render_command(SSRPrimitiveManagerInRef primitiveManager, SSRRenderManagerRef renderManager) noexcept -> void
{
    if ((!mDirty && !renderManager.get_state().has_window_changed()) || _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, empty))
    {
        return;
    }

    if (!renderManager.get_state().has_window_changed())
    {
        _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, build_transfer_command, primitiveManager, renderManager);
    }

    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, build_render_command, primitiveManager, renderManager);
    mDirty = false;
}

auto SSRRenderBlock::pike_draw_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRDrawCommandChunkPtr commands, SUINT32 pipelineIndex, SUINT32 passIndex) noexcept -> void
{
    if (_SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, empty))
    {
        return;
    }

    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, pike_draw_command, renderManager, commands, pipelineIndex, passIndex);
}

auto SSRRenderBlock::pike_transfer_command(SSRRenderManagerCRef renderManager, _SSR_CORE::SSRTransferCommandChunkPtr commands) noexcept -> void
{
    if (_SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, empty))
    {
        return;
    }

    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, pike_transfer_command, renderManager, commands);
}


_SSR_END

