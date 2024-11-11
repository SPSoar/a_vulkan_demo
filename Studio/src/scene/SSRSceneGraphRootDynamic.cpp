#include "scene/SSRSceneGraphRoot.h"
#include "manager/SSRRenderManager.h"

_SSR_STUDIO_BEGIN

SSRSceneGraphRootDynamic::~SSRSceneGraphRootDynamic()
{

}

auto SSRSceneGraphRootDynamic::init() noexcept -> void
{

}

auto SSRSceneGraphRootDynamic::clear(SSRRenderManagerRef renderManager) noexcept -> void
{
    mSceneNode.clear(renderManager);
}

auto SSRSceneGraphRootDynamic::add_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{
    mSceneNode.add_entity(entity);
}

auto SSRSceneGraphRootDynamic::modify_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{

}

auto SSRSceneGraphRootDynamic::delete_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{
    mSceneNode.delete_entity(entity);
}

auto SSRSceneGraphRootDynamic::collect_scene_node(SSRRenderManagerRef renderManager) const noexcept -> void
{
    if (!mSceneNode.is_block_empty())
    {
        renderManager.add_dynamic_scene_node(&mSceneNode);
    }
}

_SSR_END

