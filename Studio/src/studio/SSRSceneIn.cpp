#include "studio/SSRSceneIn.h"
#include "studio/SSRCameraIn.h"
#include "studio/SSRSkyboxIn.h"
#include "manager/SSRRenderManager.h"

_SSR_STUDIO_BEGIN

_SSR_UPCAST_IN(SSRScene)

_SSR_SCENE_PUB_MEMBER_FUNC_IMPL(SSRScene)

inline constexpr auto CF_CreateSceneGraph = [](SSRPrimitiveManagerRef primitiveManager, const E_ScenePolicy scenePolicy)
    -> SceneGraphNew*
{
        switch (scenePolicy)
        {
        case E_ScenePolicy::eSceneGraphStatic:
            return new SceneGraphNew(_SSR_STD::in_place_index<cast<size_t>(E_ScenePolicy::eSceneGraphStatic)>, primitiveManager);
        case E_ScenePolicy::eSceneGraphStaticDynamic:
            return new SceneGraphNew(_SSR_STD::in_place_index<cast<size_t>(E_ScenePolicy::eSceneGraphStaticDynamic)>, primitiveManager);
        case E_ScenePolicy::eSceneGraphStaticDynamicFlash:
            return new SceneGraphNew(_SSR_STD::in_place_index<cast<size_t>(E_ScenePolicy::eSceneGraphStaticDynamicFlash)>, primitiveManager);
        case E_ScenePolicy::eSceneGraphStaticFlash:
            return new SceneGraphNew(_SSR_STD::in_place_index<cast<size_t>(E_ScenePolicy::eSceneGraphStaticFlash)>, primitiveManager);
        case E_ScenePolicy::eSceneGraphDynamic:
            return new SceneGraphNew(_SSR_STD::in_place_index<cast<size_t>(E_ScenePolicy::eSceneGraphDynamic)>, primitiveManager);
        case E_ScenePolicy::eSceneGraphDynamicFlash:
            return new SceneGraphNew(_SSR_STD::in_place_index<cast<size_t>(E_ScenePolicy::eSceneGraphDynamicFlash)>, primitiveManager);
        case E_ScenePolicy::eSceneGraphFlash:
            return new SceneGraphNew(_SSR_STD::in_place_index<cast<size_t>(E_ScenePolicy::eSceneGraphFlash)>, primitiveManager);
        case E_ScenePolicy::eCount:
            _SSR_ASSERT(0, _SSR_RDERROR);
            _SSR_STD::unreachable();
        }
        return nullptr;
};

SSRSceneIn::SSRSceneIn(SSRPrimitiveManagerRef primitiveManager, const E_ScenePolicy scenePolicy) noexcept
    : mSceneGraph{ CF_CreateSceneGraph(primitiveManager, scenePolicy) }
    , _SSR_ATOM_VAL{ *mSceneGraph }
    , mScenePolicy{ scenePolicy }
{
    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, init, );
}

SSRSceneIn::~SSRSceneIn() noexcept
{
    _SSR_DELETE_OBJECT_PTR(mSceneGraph);
}
auto SSRSceneIn::set_skybox(SSRSkyboxPtr skybox) noexcept ->void
{
    mSkybox = upcast<SSRSkyboxInPtr>(skybox);
}
auto SSRSceneIn::add_entity(Entity entity) noexcept ->void
{
    _SSR_STD::scoped_lock<SSRMutex> lock(mAddMutex);
    _SSR_ASSERT(!has(mModifyEntities, entity), _SSR_RDERROR);
    _SSR_ASSERT(!has(mDeleteEntities, entity), _SSR_RDERROR);
    mAddEntities.insert(entity);
}
auto SSRSceneIn::modify_entity(Entity entity) noexcept ->void
{
    _SSR_STD::scoped_lock<SSRMutex> lock(mModifyMutex);
    _SSR_ASSERT(!has(mAddEntities, entity), _SSR_RDERROR);
    _SSR_ASSERT(!has(mDeleteEntities, entity), _SSR_RDERROR);
    mModifyEntities.insert(entity);
}
auto SSRSceneIn::delete_entity(Entity entity) noexcept ->void
{
    _SSR_STD::scoped_lock<SSRMutex> lock(mDeleteMutex);
    _SSR_ASSERT(!has(mAddEntities, entity), _SSR_RDERROR);
    _SSR_ASSERT(!has(mModifyEntities, entity), _SSR_RDERROR);
    mDeleteEntities.insert(entity);
}

auto SSRSceneIn::prepare_scene_graph(SSRStudioInCPtr studio) noexcept -> void
{
    //可以开三个线程写
    for (const auto& e : mAddEntities)
    {
        _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, add_entity, e);
    }

    for (const auto& e: mModifyEntities)
    {
        _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, modify_entity, e);
    }

    for (const auto& e: mDeleteEntities)
    {
        _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, delete_entity, e);
    }


    //_SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, animate, 1);

    mAddEntities.clear();
    mModifyEntities.clear();
    mDeleteEntities.clear();
}
auto SSRSceneIn::clear_scene(SSRRenderManagerRef renderManager) noexcept -> void
{
    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, clear, renderManager);
}
auto SSRSceneIn::collect_scene_node(SSRRenderManagerRef renderManager) const noexcept -> void
{
    renderManager.clear_all_scene_node();
    renderManager.clear_dynamic_scene_node();

    _SSR_VARIANT_FUNC_REF(_SSR_ATOM_VAL, collect_scene_node, renderManager);

    renderManager.add_light_info(&mLightUniform);

    if (mSkybox) 
    {
        renderManager.add_background_color(mSkybox->get_color());
    }
}

_SSR_END

