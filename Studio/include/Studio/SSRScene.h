#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRSCENE_H_
#define SSR_SSRSCENE_H_

_SSR_STUDIO_BEGIN


_SSR_PR_DEF(SSRScene);

#define _SSR_SCENE_GROUP_PUB_MEMBER_FUNC_DEF()\
    auto init() noexcept -> void;\
    auto add_entity(Entity entity) noexcept ->void;\
    auto modify_entity(Entity entity) noexcept ->void;\
    auto delete_entity(Entity entity) noexcept ->void;\

#define _SSR_SCENE_GROUP_IN_SCENE_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, add_entity, Entity, entity)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, modify_entity, Entity, entity)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, delete_entity, Entity, entity)\

#define _SSR_SCENE_PUB_MEMBER_FUNC_DEF()\
    _SSR_SCENE_GROUP_PUB_MEMBER_FUNC_DEF()\
    auto set_skybox(SSRSkyboxPtr skybox) noexcept ->void;\


#define _SSR_SCENE_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_SCENE_GROUP_IN_SCENE_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_skybox, SSRSkyboxPtr, skybox)\

class _SSR_STUDIO_EXPORT SSRScene
{
public:
    using Entity = SSRPrimitiveManager::Entity;
protected:
    SSRScene() noexcept = default;
    ~SSRScene()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRScene);
    _SSR_CLASS_DELETE_MOVE(SSRScene);
    _SSR_SCENE_PUB_MEMBER_FUNC_DEF()
};


_SSR_END

#endif

