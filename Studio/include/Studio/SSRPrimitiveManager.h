#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRPRIMITIVEMANAGER_H_
#define SSR_SSRPRIMITIVEMANAGER_H_

_SSR_STUDIO_BEGIN


_SSR_UNIQUE_DEF_BASE(SSRPrimitiveManager, _SSR_STUDIO_EXPORT);

#define _SSR_PRIMITIVE_MANAGER_PUB_MEMBER_FUNC_DEF()\
    auto make_primitive() noexcept -> SSRPrimitiveManager&;\
    auto init() noexcept -> SSRPrimitiveManager&;\
    auto setup_primitive_category(SUINT16 category) noexcept -> SSRPrimitiveManager&;\
    auto setup_material_instance(const SSRMaterialInstanceSPtr&  materialInstance) noexcept -> SSRPrimitiveManager&;\
    auto setup_geometry(_SSR_DATA::SSRDGeometry::Geometry* geometry) noexcept -> SSRPrimitiveManager&;\
    auto setup_transform(const SMAT4X4& transfomMat) noexcept -> SSRPrimitiveManager&;\
    auto commit(SSRStudioPtr studio) noexcept -> SSRPrimitiveManager::Entity;\

#define _SSR_PRIMITIVE_MANAGER_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRPrimitiveManager&, make_primitive,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRPrimitiveManager&, init,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRPrimitiveManager&, setup_primitive_category, SUINT16, category)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRPrimitiveManager&, setup_material_instance, const SSRMaterialInstanceSPtr& , materialInstance)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRPrimitiveManager&, setup_geometry, _SSR_DATA::SSRDGeometry::Geometry*, geometry)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRPrimitiveManager&, setup_transform, const SMAT4X4&, transfomMat)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRPrimitiveManager::Entity, commit, SSRStudioPtr, studio)\

class _SSR_STUDIO_EXPORT SSRPrimitiveManager
{
public:
    using Entity = SSRSeal<SSRPrimitiveManager>;
protected:
    SSRPrimitiveManager() noexcept = default;
    ~SSRPrimitiveManager()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRPrimitiveManager);
    _SSR_CLASS_DELETE_MOVE(SSRPrimitiveManager);
    _SSR_PRIMITIVE_MANAGER_PUB_MEMBER_FUNC_DEF()

public:
    static auto Create() -> SSRPrimitiveManagerUPtr;
};


_SSR_END

#endif

