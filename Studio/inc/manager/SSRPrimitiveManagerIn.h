#pragma once
#ifndef SSR_SSRPRIMITIVEMANAGERIN_H_
#define SSR_SSRPRIMITIVEMANAGERIN_H_

#include "../common/SSRStudioHeadIn.h"


_SSR_STUDIO_BEGIN


_SSR_PR_DEF(SSRPrimitiveManagerIn);
_SSR_PR_DEF(SSRSceneOcTreeItem);

_SSR_SHARED_DEF(SSRMaterialInstanceIn);

class SSRPrimitiveManagerIn : public SSRPrimitiveManager
{
    _SSR_DELETER_FRIEND_DEF(SSRPrimitiveManager)

#define  _SSR_FLAGS_COMBINATION_HIGHLIGHT_STATE_DEF(_SSR_DEFINER)\
    _SSR_DEFINER(All, all, Hover, Select, Error)\
    _SSR_DEFINER(HoverAndSelect, hover_and_select, Hover, Select)\

    _SSR_FLAGS_DEF(E_HighLightState,
                   _SSR_FLAGS_COMBINATION_HIGHLIGHT_STATE_DEF(_SSR_FLAGS_COMBINATION_DEF),
                   Hover, hover,
                   Select, select,
                   Error, error)

    struct Visibility 
    {
        uint8_t priority : 3;
        uint8_t channel  : 2;
        bool    castShadows    : 1;
        bool    receiveShadows : 1;
        bool    culling  : 1;
    };
    struct State
    {
        uint16_t category : 16;
        uint16_t state    : 16;
    };

    struct InstancesInfo 
    {
        union 
        {
            uint64_t             padding; // ensures the pointer is 64 bits on all archs
        };
        //SSRBufferObjectHandle handle{};
        uint16_t              count{};
    };

    _SSR_PLACEMENT_ARRAY_DEF(PrimitiveManager, 
                             SSRBoundingBox, Box, 
                             SMAT4X4, Transform, 
                             Visibility, Visibility, 
                             E_HighLightStateFlags, HighLightState,
                             State, State,
                             SSRMaterialInstanceInSPtr, MaterialInstance,
                             SSRSceneOcTreeItemPtr, OcTreeItem,
                             _SSR_DATA::SSRDGeometry::Geometry*, Geometry);

    using Entity = SSRPrimitiveManagerArray::Seal;
public:
#define  _SSR_FLAGS_COMBINATION_PRIMITIVE_STATE_DEF(_SSR_DEFINER)\
    //_SSR_DEFINER(All, all, Static, Dynamic, Flash)\

    _SSR_FLAGS_DEF(E_PrimitiveState,
                   _SSR_FLAGS_COMBINATION_PRIMITIVE_STATE_DEF(_SSR_FLAGS_COMBINATION_DEF),
                   Added, added,
                   Removed, removed,
                   Visible, visible,
                   Clip, clip,
                   BlockChanged, block_changed,
                   HighLightChanged, high_light_changed,
                   TranformChanged, tranform_changed,
                   MaterialChanged, material_changed)
    using Geometry = _SSR_DATA::SSRDGeometry::Geometry;
    _SSR_PR_DEF_S(Geometry);
protected:
    SSRPrimitiveManagerIn() noexcept = default;
    ~SSRPrimitiveManagerIn()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRPrimitiveManagerIn);
    _SSR_CLASS_DELETE_MOVE(SSRPrimitiveManagerIn);
    _SSR_PRIMITIVE_MANAGER_PUB_MEMBER_FUNC_DEF()
public:
    auto destroy(Entity e) noexcept -> void;
    auto set_octree_item(Entity e, SSRSceneOcTreeItemPtr item) noexcept -> void;
    auto clear_entity_state(Entity e) noexcept -> void;
    auto set_entity_removed(Entity e) noexcept -> void;
public:
    auto get_primitive_category(Entity e) const noexcept -> SUINT16;
    auto get_high_light_state(Entity e) const noexcept -> const E_HighLightStateFlags&;
    auto get_entity_state(Entity e) const noexcept -> SUINT16;
    auto get_entity_type(Entity e) const noexcept -> E_EntityType;
    auto get_geometry(Entity e) const noexcept -> GeometryCPtr;
    auto get_boundingbox(Entity e) const noexcept -> const SSRBoundingBox&;
    auto get_octree_item(Entity e) const noexcept -> SSRSceneOcTreeItemPtr;
    auto get_material_instance(Entity e) const noexcept ->  SSRMaterialInstanceInCSRef;
    auto get_material_instance(Entity e) noexcept ->  SSRMaterialInstanceInSRef;
protected:
    auto in_destroy_component(Entity ce) noexcept-> void;
    auto in_set_material_instance(Entity e, const SSRMaterialInstanceSPtr& materialInstance) noexcept -> void;
    auto in_setup_boundingbox(const SSRBoundingBox& box) noexcept -> void;

private:
    auto in_pick(Entity e) -> void;
    auto in_discard() -> void;

private:
    SSRPrimitiveManagerArray  mManager{};
    Entity                    mCurrentEntity{};
};


_SSR_END

#endif

