#include "manager/SSRPrimitiveManagerIn.h"
#include "studio/SSRMaterialInstanceIn.h"

_SSR_STUDIO_BEGIN

namespace 
{
inline constexpr auto CF_E_GeometryType_To_E_EntityType =
    [](const _SSR_DATA::SSRDGeometry::GeomType geometryType) -> E_EntityType
    {
        switch (geometryType)
        {
        case _SSR_DATA::SSRDGeometry::GeomType::ePoint2D:  return E_EntityType::eWireframe;
        case _SSR_DATA::SSRDGeometry::GeomType::ePoint3D:  return E_EntityType::eWireframe;
        case _SSR_DATA::SSRDGeometry::GeomType::eLine2D:   return E_EntityType::ePatch;
        case _SSR_DATA::SSRDGeometry::GeomType::eLine3D:   return E_EntityType::ePatch;
        case _SSR_DATA::SSRDGeometry::GeomType::eBox2D:    return E_EntityType::eSurface;
        case _SSR_DATA::SSRDGeometry::GeomType::eBox3D:    return E_EntityType::eSurface;
        case _SSR_DATA::SSRDGeometry::GeomType::eCircle2D: return E_EntityType::eWireframe;
        case _SSR_DATA::SSRDGeometry::GeomType::eCircle3D: return E_EntityType::eWireframe;
        case _SSR_DATA::SSRDGeometry::GeomType::eCount:    return E_EntityType::eSurface;
        case _SSR_DATA::SSRDGeometry::GeomType::eMesh2D:   return E_EntityType::eSurface;
        case _SSR_DATA::SSRDGeometry::GeomType::eMesh3D:   return E_EntityType::eSurface;
        default: _SSR_STD::unreachable();
        }
    };


inline constexpr auto CF_Get_Geometry_BoundingBox =
    [](const _SSR_DATA::SSRDGeometry::Geometry& geom, const SMAT4X4& transfomMat) -> SSRBoundingBox
    {
        SSRBoundingBox box{ };
        using Point2D = _SSR_DATA::SSRDGeometry::Point<_SSR_DATA::E_Dimension::e2D>;
        using Point3D = _SSR_DATA::SSRDGeometry::Point<_SSR_DATA::E_Dimension::e3D>;

        using Line2D = _SSR_DATA::SSRDGeometry::Line<_SSR_DATA::E_Dimension::e2D>;
        using Line3D = _SSR_DATA::SSRDGeometry::Line<_SSR_DATA::E_Dimension::e3D>;

        using Box2D = _SSR_DATA::SSRDGeometry::Box<_SSR_DATA::E_Dimension::e2D>;
        using Box3D = _SSR_DATA::SSRDGeometry::Box<_SSR_DATA::E_Dimension::e3D>;

        using Circle2D = _SSR_DATA::SSRDGeometry::Circle<_SSR_DATA::E_Dimension::e2D>;
        using Circle3D = _SSR_DATA::SSRDGeometry::Circle<_SSR_DATA::E_Dimension::e3D>;

        _SSR_STD::visit(_SSR::SSROverload{
                [&](const Point2D& point) {
  
                },
                [&](const Point3D& point) {
                    const auto& position = point.get_data<Point3D::Key::ePosition>();
                    box = SSRBoundingBox(position, position);
                },
                [&](const Line2D& line) {

                },
                [&](const Line3D& line) {
                    const auto& startPoint = line.get_data<Line3D::Key::eStartPoint>();
                    const auto& endPoint   = line.get_data<Line3D::Key::eEndPoint>();
                    box = SSRBoundingBox(startPoint, endPoint);
                },
                [&](const Box2D& cube) {
                    //const auto& centre = circle.get_centre();
                    //const auto& normal = circle.get_normal();
                    //const auto& radius = circle.get_radius();

                },
                [&](const Box3D& cube) {

                    const auto& centre   = cube.get_data<Box3D::Key::eCentre>();
                    const auto& halfEdge = cube.get_data<Box3D::Key::eHalfEdge>();
                    box = SSRBoundingBox(centre - halfEdge, centre + halfEdge);
                },
                [&](const Circle2D& circle) {

                },
                [&](const Circle3D& circle) {

                },
            }, geom
        );
        return box;
    };
}

_SSR_UNIQUE_IMPL_IN(SSRPrimitiveManager)

_SSR_PRIMITIVE_MANAGER_PUB_MEMBER_FUNC_IMPL(SSRPrimitiveManager)

auto SSRPrimitiveManager::Create() -> SSRPrimitiveManagerUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRPrimitiveManager);
}

auto SSRPrimitiveManagerIn::destroy(Entity e) noexcept -> void
{   
    _SSR_LIKELY_IF (e) 
    {
        in_destroy_component(e);
        mManager.free(e);
    }
}

auto SSRPrimitiveManagerIn::get_primitive_category(const Entity e) const noexcept -> SUINT16
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    const State& state = mManager[e].mState;
    return state.category;
}

auto SSRPrimitiveManagerIn::get_entity_type(const Entity e) const noexcept -> E_EntityType
{
    const auto& geom = get_geometry(e);
    return CF_E_GeometryType_To_E_EntityType(_SSR_DATA::SSRDGeometry::get_geometry_type_by_variant_index(geom->index()));
}

auto SSRPrimitiveManagerIn::get_high_light_state(const Entity e) const noexcept -> const E_HighLightStateFlags&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    const E_HighLightStateFlags& state = mManager[e].mHighLightState;
    return state;
}

auto SSRPrimitiveManagerIn::clear_entity_state(const Entity e) noexcept -> void
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    State& state = mManager[e].mState;
    state.state = 0;
}

auto SSRPrimitiveManagerIn::set_entity_removed(const Entity e) noexcept -> void
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    State& state = mManager[e].mState;
    state.state |= cast<uint16_t>(E_PrimitiveStateFlags::eRemoved);
}

auto SSRPrimitiveManagerIn::get_entity_state(const Entity e) const noexcept -> SUINT16
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    const State& state = mManager[e].mState;
    return state.state;
}
auto SSRPrimitiveManagerIn::in_destroy_component(Entity e) noexcept -> void
{
    auto& material = get_material_instance(e);
    material = nullptr;

}

auto SSRPrimitiveManagerIn::get_geometry(const Entity e) const noexcept -> GeometryCPtr
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    const GeometryCPtr& geometry = mManager[e].mGeometry;
    return geometry;
}

auto SSRPrimitiveManagerIn::get_material_instance(const Entity e) const noexcept -> SSRMaterialInstanceInCSRef
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    const SSRMaterialInstanceInCSPtr& materialInstance = mManager[e].mMaterialInstance;
    return materialInstance;
}
auto SSRPrimitiveManagerIn::get_material_instance(const Entity e) noexcept -> SSRMaterialInstanceInSRef
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    SSRMaterialInstanceInSPtr& materialInstance = mManager[e].mMaterialInstance;
    return materialInstance;
}

auto SSRPrimitiveManagerIn::get_boundingbox(const Entity e) const noexcept -> const SSRBoundingBox&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    const SSRBoundingBox& boundingbox = mManager[e].mBox;
    return boundingbox;
}

auto SSRPrimitiveManagerIn::get_octree_item(const Entity e) const noexcept -> SSRSceneOcTreeItemPtr
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    SSRSceneOcTreeItemPtr item = mManager[e].mOcTreeItem;
    return item;
}


auto SSRPrimitiveManagerIn::in_set_material_instance(Entity e, const SSRMaterialInstanceSPtr& materialInstance) noexcept -> void
{
    _SSR_LIKELY_IF (e) 
    {
        _SSR_ASSERT(materialInstance, _SSR_RDERROR);

        auto materialInstanceIn = upcast<SSRMaterialInstanceInSPtr>(materialInstance);
        auto& materialInstanceOld = get_material_instance(e);
        if (materialInstanceOld)
        {
            if ((materialInstanceOld->get_shading_type() == E_Shading::eTranslucent &&
                 materialInstanceIn->get_shading_type() != E_Shading::eTranslucent) ||
                (materialInstanceOld->get_shading_type() != E_Shading::eTranslucent &&
                    materialInstanceIn->get_shading_type() == E_Shading::eTranslucent))
            {
                State& state = mManager[mCurrentEntity].mState;
                state.state |= cast<uint16_t>(E_PrimitiveStateFlags::eBlockChanged);
            }
        }
        materialInstanceOld = materialInstanceIn;
    }
}

auto SSRPrimitiveManagerIn::in_pick(Entity e) -> void
{
    //SSRBoundingBox& boundingbox = mManager[e].mBox;
    //boundingbox.init();

    //GeometryPtr& geometry = mManager[e].mGeometry;
    //geometry = nullptr;

    //E_HighLightStateFlags& hlState = mManager[e].mHighLightState;
    //hlState.reset();

    //SSRSlice<SSRMaterialInstanceInPtr>& materialInstance = mManager[e].mMaterialInstance;
    //materialInstance.clear();

    //SSRSceneOcTreeItemPtr& itemIn = mManager[e].mOcTreeItem;
    //itemIn = nullptr;

    //SMAT4X4& transform = mManager[mCurrentEntity].mTransform;
    //transform.repair();

    //Visibility& visibility = mManager[mCurrentEntity].mVisibility;
    //visibility = {};

    //State& state = mManager[mCurrentEntity].mState;
    //state = {};

    mCurrentEntity = e;
}

auto SSRPrimitiveManagerIn::in_discard() -> void
{
    mCurrentEntity.reset();
}


auto SSRPrimitiveManagerIn::make_primitive() noexcept -> SSRPrimitiveManager&
{
    _SSR_ASSERT(!mCurrentEntity, _SSR_RDERROR);
  
    in_pick(mManager.create());

    return *this;
}

auto SSRPrimitiveManagerIn::init() noexcept -> SSRPrimitiveManager&
{
    _SSR_ASSERT(mCurrentEntity, _SSR_RDERROR);
    
    return *this;
}

//auto SSRPrimitiveManagerIn::setup_culling(bool enable) noexcept -> SSRPrimitiveManager&
//{
//    Visibility& visibility = mManager[mCurrentEntity].mVisibility;
//    visibility.culling = enable;
//    return *this;
//}
//auto SSRPrimitiveManagerIn::setup_receive_shadows(bool enable) noexcept -> SSRPrimitiveManager&
//{
//    Visibility& visibility = mManager[mCurrentEntity].mVisibility;
//    visibility.receiveShadows = enable;
//    return *this;
//}
//auto SSRPrimitiveManagerIn::setup_cast_shadows(bool enable) noexcept -> SSRPrimitiveManager&
//{
//    Visibility& visibility = mManager[mCurrentEntity].mVisibility;
//    visibility.castShadows = enable;
//    return *this;
//}
auto SSRPrimitiveManagerIn::setup_primitive_category(SUINT16 category) noexcept -> SSRPrimitiveManager&
{
    State& state = mManager[mCurrentEntity].mState;
    state.category = category;
    return *this;
}


auto SSRPrimitiveManagerIn::setup_material_instance(const SSRMaterialInstanceSPtr& materialInstance) noexcept -> SSRPrimitiveManager&
{
    in_set_material_instance(mCurrentEntity, materialInstance);
    return *this;
}

auto SSRPrimitiveManagerIn::setup_geometry(_SSR_DATA::SSRDGeometry::Geometry* geometry) noexcept -> SSRPrimitiveManager&
{
    mManager[mCurrentEntity].mGeometry = geometry;

    return *this;
}
auto SSRPrimitiveManagerIn::setup_transform(const SMAT4X4& transfomMat) noexcept -> SSRPrimitiveManager&
{
    SMAT4X4& transform = mManager[mCurrentEntity].mTransform;
    transform = transfomMat;

    in_setup_boundingbox(CF_Get_Geometry_BoundingBox(*get_geometry(mCurrentEntity), transfomMat));

    State& state = mManager[mCurrentEntity].mState;
    state.state |= cast<uint16_t>(E_PrimitiveStateFlags::eTranformChanged);
    return *this;
}
auto SSRPrimitiveManagerIn::commit(SSRStudioPtr studio) noexcept -> SSRPrimitiveManager::Entity
{
    State& state = mManager[mCurrentEntity].mState;
    state.state |= cast<uint16_t>(E_PrimitiveStateFlags::eAdded);

    SSRPrimitiveManager::Entity entity = mCurrentEntity;
    in_discard();
    return entity;
}

auto SSRPrimitiveManagerIn::in_setup_boundingbox(const SSRBoundingBox& box) noexcept -> void
{
    SSRBoundingBox& boxIn = mManager[mCurrentEntity].mBox;
    boxIn = box;
}


auto SSRPrimitiveManagerIn::set_octree_item(Entity e, SSRSceneOcTreeItemPtr item) noexcept -> void
{
    SSRSceneOcTreeItemPtr& itemIn = mManager[e].mOcTreeItem;
    itemIn = item;
}

_SSR_END

