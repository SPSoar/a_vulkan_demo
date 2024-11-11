#include "scene/SSRSceneGraphRoot.h"
#include "manager/SSRRenderManager.h"
#include "scene/SSRSceneOcTree.h"

_SSR_STUDIO_BEGIN

SSRSceneGraphRootStatic::~SSRSceneGraphRootStatic() 
{
    _SSR_DELETE_OBJECT_PTR(mOcTree);
}

auto SSRSceneGraphRootStatic::init() noexcept -> void
{
    if (!mOcTree) 
    {
        mOcTree = new SSRSceneOcTree(mPrimitiveManager);
    }

    mOcTree->init();
}

auto SSRSceneGraphRootStatic::clear(SSRRenderManagerRef renderManager) noexcept -> void
{
    mOcTree->clear_scene_node(renderManager);
}

auto SSRSceneGraphRootStatic::add_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{
    mOcTree->add_enity(entity);
}

auto SSRSceneGraphRootStatic::modify_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{
    const auto& primState = mPrimitiveManager.get_entity_state(entity);
    if (SSRPrimitiveManagerIn::E_PrimitiveStateFlags(primState).has_tranform_changed())
    {
        mOcTree->modify_enity(entity, true);
    }
    else
    {
        mOcTree->modify_enity(entity, false);
    }
}

auto SSRSceneGraphRootStatic::delete_entity(SSRPrimitiveManager::Entity entity) noexcept ->void
{
    mOcTree->remove_enity(entity);
}

auto SSRSceneGraphRootStatic::collect_scene_node(SSRRenderManagerRef renderManager) const noexcept -> void
{
    auto nodeList = mOcTree->select_octree_node(renderManager.get_current_camera());
    for (auto&& node : nodeList.result())
    {
        _SSR_ASSERT(node, _SSR_RDERROR);
        if (auto sceneNode = node->scene_node())
        {
            renderManager.add_scene_node(sceneNode);
        }
    }
}

_SSR_END

