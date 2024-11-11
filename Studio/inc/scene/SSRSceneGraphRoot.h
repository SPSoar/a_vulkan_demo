#pragma once
#ifndef SSR_SSRSCENEGRAPHROOT_H_
#define SSR_SSRSCENEGRAPHROOT_H_

#include "../common/SSRStudioHeadIn.h"

#include "SSRSceneGraphNode.h"

#define _SSR_SCENE_GRAPH_ROOT_CLASS_DEF(Graph)\
    public:\
        _SSR_NODISCARD\
        static auto Create(SSRPrimitiveManagerInRef primitiveManager) -> SSRSceneGraphRoot##Graph\
        {\
            return SSRSceneGraphRoot##Graph{ primitiveManager };\
        }\
    public:\
        SSRSceneGraphRoot##Graph() noexcept = delete;\
        explicit SSRSceneGraphRoot##Graph(SSRPrimitiveManagerInRef primitiveManager) noexcept\
            : mPrimitiveManager{ primitiveManager }\
        {\
        }\
        ~SSRSceneGraphRoot##Graph() noexcept;\
        _SSR_CLASS_DELETE_COPY(SSRSceneGraphRoot##Graph);\
        SSRSceneGraphRoot##Graph& operator=(SSRSceneGraphRoot##Graph&& other) noexcept = delete;\
        SSRSceneGraphRoot##Graph(SSRSceneGraphRoot##Graph&& other) noexcept\
            : mPrimitiveManager{ other.mPrimitiveManager }\
        {\
        }\
    public:\
        auto init() noexcept -> void;\
        auto clear(SSRRenderManagerRef renderManager) noexcept -> void;\
        auto add_entity(SSRPrimitiveManager::Entity entity) noexcept ->void;\
        auto modify_entity(SSRPrimitiveManager::Entity entity) noexcept ->void;\
        auto delete_entity(SSRPrimitiveManager::Entity entity) noexcept ->void;\
        auto collect_scene_node(SSRRenderManagerRef renderManager) const noexcept -> void;\
    protected:\
        SSRPrimitiveManagerInRef mPrimitiveManager;\

_SSR_STUDIO_BEGIN


template<typename GraphNode, typename Entity = SSRPrimitiveManager::Entity>
concept CC_IsGraphNode = requires(GraphNode graphNode, Entity entity)
{
    { graphNode.init() }                 -> _SSR_STD::same_as<void>;
    { graphNode.add_entity(entity) }     -> _SSR_STD::same_as<void>;
    { graphNode.modify_entity(entity) }  -> _SSR_STD::same_as<void>;
    { graphNode.delete_entity(entity) }  -> _SSR_STD::same_as<void>;
};

_SSR_PR_DEF(SSRSceneOcTree);
_SSR_PR_DEF(SSRRenderManager);

class SSRSceneGraphRoot
{

};

class SSRSceneGraphRootStatic final : public SSRSceneGraphRoot
{
    _SSR_SCENE_GRAPH_ROOT_CLASS_DEF(Static)
protected:
    SSRSceneOcTreePtr mOcTree{ nullptr };
};

class SSRSceneGraphRootDynamic final : public SSRSceneGraphRoot
{
    _SSR_SCENE_GRAPH_ROOT_CLASS_DEF(Dynamic)
public:
    auto animate(const SFLOAT32 time) noexcept
    {

    }
protected:
    SSRSceneGraphNode mSceneNode{ mPrimitiveManager };
};

class SSRSceneGraphRootFlash final : public SSRSceneGraphRoot
{
    _SSR_SCENE_GRAPH_ROOT_CLASS_DEF(Flash)
protected:
    SSRSceneGraphNode mSceneNode{ mPrimitiveManager };
};

_SSR_END

#endif

