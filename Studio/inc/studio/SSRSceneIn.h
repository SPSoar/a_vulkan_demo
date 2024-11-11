#pragma once
#ifndef SSR_SSRSCENEIN_H_
#define SSR_SSRSCENEIN_H_

#include "../common/SSRStudioHeadIn.h"
#include "scene/SSRSceneGraph.h"
#include "core/SSRLightUniform.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRSceneIn);
_SSR_PR_DEF(SSRStudioIn);
_SSR_PR_DEF(SSRSkyboxIn);
_SSR_PR_DEF(SSRRenderManager);

class SSRSceneIn : public SSRScene
{
    friend class SSRStudioIn;
public:
    using Entities = SSRSet<Entity, Entity::Hasher>;
protected:
    SSRSceneIn() = delete;
    explicit SSRSceneIn(SSRPrimitiveManagerRef primitiveManager, E_ScenePolicy sceneGroupType) noexcept;
    ~SSRSceneIn() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRSceneIn);
    _SSR_CLASS_DELETE_MOVE(SSRSceneIn);
    _SSR_SCENE_PUB_MEMBER_FUNC_DEF()
public:
    auto clear_scene(SSRRenderManagerRef renderManager) noexcept -> void;
    auto prepare_scene_graph(SSRStudioInCPtr studio) noexcept -> void;
    auto collect_scene_node(SSRRenderManagerRef renderManager) const noexcept -> void;
protected:
    SceneGraphNew*           mSceneGraph;
    SceneGraphNew&           _SSR_ATOM_VAL;
    const E_ScenePolicy      mScenePolicy{};

    Entities mAddEntities{};
    Entities mModifyEntities{};
    Entities mDeleteEntities{};

    SSRMutex mAddMutex{};
    SSRMutex mModifyMutex{};
    SSRMutex mDeleteMutex{};

    SSRLightUniform mLightUniform{};
    SSRSkyboxInPtr  mSkybox{ nullptr };

};

_SSR_END

#endif

