#include "scene/SSRSceneGraphRoot.h"
#include "manager/SSRRenderManager.h"

_SSR_STUDIO_BEGIN

SSRSceneGraphRootFlash::~SSRSceneGraphRootFlash()
{
}

auto SSRSceneGraphRootFlash::init() noexcept -> void
{

}

auto SSRSceneGraphRootFlash::clear(SSRRenderManagerRef renderManager) noexcept -> void
{
    mSceneNode.clear(renderManager);
}

auto SSRSceneGraphRootFlash::add_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{
    mSceneNode.add_entity(entity);
}

auto SSRSceneGraphRootFlash::modify_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{

}

auto SSRSceneGraphRootFlash::delete_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{
    mSceneNode.delete_entity(entity);
}

auto SSRSceneGraphRootFlash::collect_scene_node(SSRRenderManagerRef renderManager) const noexcept -> void
{
    //if (renderManager.is_frustum_changed() && !mSceneNode.is_block_empty())
    //{
    //    renderManager.add_scene_block(&mSceneNode);
    //}
}

_SSR_END

