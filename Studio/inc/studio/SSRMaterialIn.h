#pragma once
#ifndef SSR_SSRMATERIALIN_H_
#define SSR_SSRMATERIALIN_H_

#include "../common/SSRStudioHeadIn.h"

#include "core/SSRUniformInterfaceBlock.h"
#include "core/SSRSamplerInterfaceBlock.h"
#include "core/SSRTextureGroup.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRStudioIn);

class SSRMaterialIn : public SSRMaterial, public _SSR_STD::enable_shared_from_this<SSRMaterialIn>
{
    _SSR_DELETER_FRIEND_DEF(SSRMaterial)
public:
    using ShaderMap = SSRMap<MaterialVariantKey, _SSR_CORE::SSRShaderPtr>;
public:
    SSRMaterialIn() noexcept= delete;
    explicit SSRMaterialIn(SSRStudioRef studio)  noexcept;
    ~SSRMaterialIn() noexcept;
    _SSR_CLASS_DELETE_COPY(SSRMaterialIn);
    _SSR_CLASS_DELETE_MOVE(SSRMaterialIn);
    _SSR_MATERIAL_PUB_MEMBER_FUNC_DEF()
    static auto Make_Material_Key(_SSR_CORE::RE_ShaderType shaderType, SE_MaterialVariantFlags::ValueType variant)  noexcept -> MaterialVariantKey;
public:
    auto get_shader(const _SSR_CORE::RE_ShaderType shaderType, const SE_MaterialVariantFlags::ValueType variant) const noexcept -> _SSR_CORE::SSRShaderPtr
    {
        auto key = Make_Material_Key(shaderType, variant);
        if (mShaderMap.find(key) != mShaderMap.end()) 
        {
            return mShaderMap.at(key);
        }
        else 
        {
            return nullptr;
        }
    }
    auto get_studio() const noexcept -> SSRStudioInRef&
    {
        return mStudio;
    }
    auto get_shading_type() const noexcept 
    {
        return mShading;
    }
    auto prepare() noexcept -> void;
    auto build(_SSR_CORE::SSRSubTransferCommandPtr transferCommand) noexcept -> void;

    auto textureGroup() const noexcept -> SSRTextureGroupRef
    {
        return *mTextureGroup;
    }
protected:
    auto parse() noexcept -> void;
protected:
    SSRStudioInRef                     mStudio;
    _SSR_DATA::SSRDMaterial::Material* mMaterialDesc{ nullptr };
    ShaderMap                          mShaderMap{};
    E_Shading                          mShading{ E_Shading::eNone };
    bool                               mShaderCompiled{ false };
    _SSR_STD::atomic_flag              mBuildFinished ATOMIC_FLAG_INIT;
    SSRUniformInterfaceBlock           mUniformInterfaceBlock{ "PerMaterialUniformInterfaceBlock" };
    SSRSamplerInterfaceBlock           mSamplerInterfaceBlock{ "PerMaterialSamplerInterfaceBlock" };
    SSRTextureGroupPtr                 mTextureGroup{ nullptr };
};
_SSR_END

#endif

