#pragma once
#ifndef SSR_SSRMATERIALINSTANCEIN_H_
#define SSR_SSRMATERIALINSTANCEIN_H_

#include "../common/SSRStudioHeadIn.h"
#include "SSRMaterialIn.h"
#include "core/SSRPerMaterialUniform.h"

_SSR_STUDIO_BEGIN

_SSR_SHARED_DEF(SSRMaterialInstanceIn);
_SSR_SHARED_DEF(SSRMaterialIn);

class SSRMaterialInstanceIn : public SSRMaterialInstance
{
    _SSR_DELETER_FRIEND_DEF(SSRMaterialInstance)
    friend class SSRMaterialIn;
public:
    SSRMaterialInstanceIn() noexcept = delete;
    explicit SSRMaterialInstanceIn(const SSRMaterialInSPtr& material) noexcept
        : mStudio{ material->get_studio()}
        , mMaterial{ material }
    {
        update_material_info();
    }
    ~SSRMaterialInstanceIn() noexcept;
    _SSR_CLASS_DELETE_COPY(SSRMaterialInstanceIn);
    _SSR_CLASS_DELETE_MOVE(SSRMaterialInstanceIn);
    _SSR_MATERIAL_INSTANCE_PUB_MEMBER_FUNC_DEF()
public:
    auto get_uniform_buffer() const noexcept -> _SSR_CORE::SSRBufferPtr
    {
        return mPerMaterialUniform.get_buffer();
    }

    auto get_shading_type() const noexcept
    {
        return mMaterial->get_shading_type();
    }

    auto get_material() const noexcept
    {
        return mMaterial;
    }

    auto update_material_info() noexcept -> void;
    auto prepare() noexcept -> void;
    auto build(_SSR_CORE::SSRSubTransferCommandPtr tansferCommand) noexcept -> void;
protected:
    SSRStudioInRef        mStudio;
    SSRMaterialInSPtr     mMaterial{ nullptr };
    SSRPerMaterialUniform mPerMaterialUniform{};
    _SSR_STD::atomic_flag mBuildFinished ATOMIC_FLAG_INIT;
};
_SSR_END

#endif

