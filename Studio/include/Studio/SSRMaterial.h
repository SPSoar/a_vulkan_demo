#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRMATERIAL_H_
#define SSR_SSRMATERIAL_H_

_SSR_STUDIO_BEGIN

_SSR_SHARED_DEF(SSRMaterial);

#define _SSR_MATERIAL_PUB_MEMBER_FUNC_DEF()\
    auto init() noexcept -> SSRMaterial&;\
    auto init_name(const char* name) noexcept -> SSRMaterial&;\
    auto setup() noexcept -> SSRMaterial&;\
    auto setup_package(_SSR_DATA::SSRDMaterial::Material* material) noexcept -> SSRMaterial&;\
    auto commit() noexcept -> void;\
    auto make_instance() noexcept -> SSRMaterialInstanceSPtr;\

#define _SSR_MATERIAL_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRMaterial&, init,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRMaterial&, init_name, const char*, name)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRMaterial&, setup,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRMaterial&, setup_package, _SSR_DATA::SSRDMaterial::Material*, material)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, commit,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRMaterialInstanceSPtr, make_instance,,)\

class _SSR_STUDIO_EXPORT SSRMaterial
{
protected:
    SSRMaterial() noexcept = default;
    ~SSRMaterial()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRMaterial);
    _SSR_CLASS_DELETE_MOVE(SSRMaterial);
    _SSR_MATERIAL_PUB_MEMBER_FUNC_DEF()
public:
    static auto Create(SSRStudioRef studio) -> SSRMaterialSPtr;
};

_SSR_END

#endif

