#pragma once
#ifndef SSR_SSRSCENEGRAPH_H_
#define SSR_SSRSCENEGRAPH_H_

#include "../common/SSRStudioHeadIn.h"

#include "SSRSceneGraphRoot.h"

_SSR_STUDIO_BEGIN


template<CC_IsGraphNode... Graph>
class SSRSceneGraph
{
public:
    SSRSceneGraph() noexcept = delete;
    explicit SSRSceneGraph(SSRPrimitiveManagerRef primitiveManager) noexcept 
        : mPrimitiveManager{ upcast<SSRPrimitiveManagerInRef>(primitiveManager) }
        , mGraphs{ Get_Graphs(mPrimitiveManager)}
    {
    
    }
    ~SSRSceneGraph() noexcept = default;

    _SSR_CLASS_DELETE_COPY(SSRSceneGraph);
    _SSR_CLASS_DELETE_MOVE(SSRSceneGraph);
protected:
    using GraphListType = _SSR_STD::tuple<Graph ...>;

    _SSR_NODISCARD
    static auto Get_Graphs(SSRPrimitiveManagerInRef primitiveManager) -> GraphListType
    {
        return _SSR_STD::tuple(Graph::Create(primitiveManager)...);
    }

    static constexpr const SUINT8 SCCV_GraphCount = sizeof...(Graph);

    template<SCINT Index>
        requires (Index < SCCV_GraphCount)
    auto get_graph() noexcept -> _SSR_STD::tuple_element_t<cast<size_t>(Index), GraphListType>&
    {
        return _SSR_STD::get<cast<size_t>(Index)>(mGraphs);
    }
    template<SCINT Index>
        requires (Index < SCCV_GraphCount)
    auto get_graph() const noexcept -> const _SSR_STD::tuple_element_t<cast<size_t>(Index), GraphListType>&
    {
        return _SSR_STD::get<cast<size_t>(Index)>(mGraphs);
    } 

    template<size_t GraphCount>
        requires (GraphCount >= 1)
    auto add_entity_static(SSRPrimitiveManager::Entity entity) noexcept 
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootStatic>) 
        {
            get_graph<0>().add_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 1)
    auto add_entity_dynamic(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<0>().add_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount > 1)
    auto add_entity_dynamic(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<0>().add_entity(entity);
        }
        else if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(1), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<1>().add_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 1)
    auto add_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<0>().add_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 2)
    auto add_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(1), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<1>().add_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 3)
    auto add_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(2), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<2>().add_entity(entity);
        }
        else 
        {
            _SSR_ASSERT(0, _SSR_RDERROR);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount >= 1)
    auto modify_entity_static(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootStatic>)
        {
            get_graph<0>().modify_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 1)
    auto modify_entity_dynamic(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<0>().modify_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount > 1)
    auto modify_entity_dynamic(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<0>().modify_entity(entity);
        }
        else if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(1), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<1>().modify_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 1)
    auto modify_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<0>().modify_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 2)
    auto modify_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(1), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<1>().modify_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 3)
    auto modify_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(2), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<2>().modify_entity(entity);
        }
        else
        {
            _SSR_ASSERT(0, _SSR_RDERROR);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount >= 1)
    auto delete_entity_static(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootStatic>)
        {
            get_graph<0>().delete_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 1)
    auto delete_entity_dynamic(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<0>().delete_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount > 1)
    auto delete_entity_dynamic(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<0>().delete_entity(entity);
        }
        else if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(1), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<1>().delete_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 1)
    auto delete_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<0>().delete_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 2)
    auto delete_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(1), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<1>().delete_entity(entity);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 3)
    auto delete_entity_flash(SSRPrimitiveManager::Entity entity) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(2), GraphListType>, SSRSceneGraphRootFlash>)
        {
            get_graph<2>().delete_entity(entity);
        }
        else
        {
            _SSR_ASSERT(0, _SSR_RDERROR);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount == 1)
    auto animate_dynamic(const SFLOAT32 time) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<0>().animate(time);
        }
    }

    template<size_t GraphCount>
        requires (GraphCount > 1)
    auto animate_dynamic(const SFLOAT32 time) noexcept
    {
        if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(0), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<0>().animate(time);
        }
        else if constexpr (_SSR_STD::is_same_v<_SSR_STD::tuple_element_t<cast<size_t>(1), GraphListType>, SSRSceneGraphRootDynamic>)
        {
            get_graph<1>().animate(time);
        }
    }

public:
    auto init() noexcept -> void 
    {
        _SSR_STD::apply(
            [this](auto &... graph) {
                (graph.init(), ...);
            },
            mGraphs
        );
    }
    
    auto clear(SSRRenderManagerRef renderManager) noexcept -> void
    {
        _SSR_STD::apply(
            [this, &renderManager](auto &... graph) {
                (graph.clear(renderManager), ...);
            },
            mGraphs
        );
    }

    auto add_entity(SSRPrimitiveManager::Entity entity) noexcept ->void 
    {
        auto&& category = mPrimitiveManager.get_primitive_category(entity);
        switch (cast<E_PrimitiveCategory>(category))
        {
        case E_PrimitiveCategory::eStatic:
        {
            add_entity_static<SCCV_GraphCount>(entity);
            break;
        }
        case E_PrimitiveCategory::eDynamic:
        {
            add_entity_dynamic<SCCV_GraphCount>(entity);
            break;
        }
        case E_PrimitiveCategory::eFlash:
        {
            add_entity_flash<SCCV_GraphCount>(entity);
            break;
        }
        default:
            _SSR_ASSERT(0, _SSR_RDERROR);
            _SSR_STD::unreachable();
        }
    }

    auto modify_entity(SSRPrimitiveManager::Entity entity) noexcept ->void 
    {
        auto&& category = mPrimitiveManager.get_primitive_category(entity);
        switch (cast<E_PrimitiveCategory>(category))
        {
        case E_PrimitiveCategory::eStatic:
        {
            modify_entity_static<SCCV_GraphCount>(entity);
            break;
        }
        case E_PrimitiveCategory::eDynamic:
        {
            modify_entity_dynamic<SCCV_GraphCount>(entity);
            break;
        }
        case E_PrimitiveCategory::eFlash:
        {
            modify_entity_flash<SCCV_GraphCount>(entity);
            break;
        }
        default:
            _SSR_ASSERT(0, _SSR_RDERROR);
            _SSR_STD::unreachable();
        }
    }

    auto delete_entity(SSRPrimitiveManager::Entity entity) noexcept ->void 
    {
        auto&& category = mPrimitiveManager.get_primitive_category(entity);
        switch (cast<E_PrimitiveCategory>(category))
        {
        case E_PrimitiveCategory::eStatic:
        {
            delete_entity_static<SCCV_GraphCount>(entity);
            break;
        }
        case E_PrimitiveCategory::eDynamic:
        {
            delete_entity_dynamic<SCCV_GraphCount>(entity);
            break;
        }
        case E_PrimitiveCategory::eFlash:
        {
            delete_entity_flash<SCCV_GraphCount>(entity);
            break;
        }
        default:
            _SSR_ASSERT(0, _SSR_RDERROR);
            _SSR_STD::unreachable();
        }
    }

    auto animate(const SFLOAT32 time) noexcept -> void 
    {
        animate_dynamic<SCCV_GraphCount>(time);
    }

    auto collect_scene_node(SSRRenderManagerRef renderManager) const noexcept -> void
    {
        _SSR_STD::apply(
            [&](auto &... graph) {
                (graph.collect_scene_node(renderManager), ...);
            },
            mGraphs
        );
    }
private:
    SSRPrimitiveManagerInRef mPrimitiveManager;
    GraphListType            mGraphs{};
};

using SceneGraphStatic              = SSRSceneGraph<SSRSceneGraphRootStatic>;
using SceneGraphStaticDynamic       = SSRSceneGraph<SSRSceneGraphRootStatic, SSRSceneGraphRootDynamic>;
using SceneGraphStaticDynamicFlash  = SSRSceneGraph<SSRSceneGraphRootStatic, SSRSceneGraphRootDynamic, SSRSceneGraphRootFlash>;
using SceneGraphStaticFlash         = SSRSceneGraph<SSRSceneGraphRootStatic, SSRSceneGraphRootFlash>;
using SceneGraphDynamic             = SSRSceneGraph<SSRSceneGraphRootDynamic>;
using SceneGraphDynamicFlash        = SSRSceneGraph<SSRSceneGraphRootDynamic, SSRSceneGraphRootFlash>;
using SceneGraphFlash               = SSRSceneGraph<SSRSceneGraphRootFlash>;


_SSR_SASSERT(CC_IsGraphNode<SceneGraphStatic>, _SSR_RDERROR);
_SSR_SASSERT(CC_IsGraphNode<SceneGraphStaticDynamic>, _SSR_RDERROR);
_SSR_SASSERT(CC_IsGraphNode<SceneGraphStaticDynamicFlash>, _SSR_RDERROR);
_SSR_SASSERT(CC_IsGraphNode<SceneGraphStaticFlash>, _SSR_RDERROR);
_SSR_SASSERT(CC_IsGraphNode<SceneGraphDynamic>, _SSR_RDERROR);
_SSR_SASSERT(CC_IsGraphNode<SceneGraphDynamicFlash>, _SSR_RDERROR);
_SSR_SASSERT(CC_IsGraphNode<SceneGraphFlash>, _SSR_RDERROR);


_SSR_VARIANT_DEF(SceneGraphNew,
                 SceneGraphStatic,
                 SceneGraphStaticDynamic,
                 SceneGraphStaticDynamicFlash,
                 SceneGraphStaticFlash,
                 SceneGraphDynamic,
                 SceneGraphDynamicFlash,
                 SceneGraphFlash)

_SSR_SASSERT(_SSR_STD::is_same_v<SceneGraphStatic,             _SSR_STD::variant_alternative_t<cast<SENUM8>(E_ScenePolicy::eSceneGraphStatic), SceneGraphNew>>);
_SSR_SASSERT(_SSR_STD::is_same_v<SceneGraphStaticDynamic,      _SSR_STD::variant_alternative_t<cast<SENUM8>(E_ScenePolicy::eSceneGraphStaticDynamic), SceneGraphNew>>);
_SSR_SASSERT(_SSR_STD::is_same_v<SceneGraphStaticDynamicFlash, _SSR_STD::variant_alternative_t<cast<SENUM8>(E_ScenePolicy::eSceneGraphStaticDynamicFlash), SceneGraphNew>>);
_SSR_SASSERT(_SSR_STD::is_same_v<SceneGraphStaticFlash,        _SSR_STD::variant_alternative_t<cast<SENUM8>(E_ScenePolicy::eSceneGraphStaticFlash), SceneGraphNew>>);
_SSR_SASSERT(_SSR_STD::is_same_v<SceneGraphDynamic,            _SSR_STD::variant_alternative_t<cast<SENUM8>(E_ScenePolicy::eSceneGraphDynamic), SceneGraphNew>>);
_SSR_SASSERT(_SSR_STD::is_same_v<SceneGraphDynamicFlash,       _SSR_STD::variant_alternative_t<cast<SENUM8>(E_ScenePolicy::eSceneGraphDynamicFlash), SceneGraphNew>>);
_SSR_SASSERT(_SSR_STD::is_same_v<SceneGraphFlash,              _SSR_STD::variant_alternative_t<cast<SENUM8>(E_ScenePolicy::eSceneGraphFlash), SceneGraphNew>>);

_SSR_SASSERT(_SSR_STD::variant_size_v<SceneGraphNew> == cast<SENUM8>(E_ScenePolicy::eCount), _SSR_RDERROR);

_SSR_END

#endif

