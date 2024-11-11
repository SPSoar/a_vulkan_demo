#pragma once
#ifndef SSR_SSRPERMATERIALUNIFORM_H_
#define SSR_SSRPERMATERIALUNIFORM_H_

#include "../common/SSRStudioHeadIn.h"
#include "core/SSRUniformInterfaceBlock.h"
#include "core/SSRSamplerInterfaceBlock.h"
#include "core/SSRUniformCpuBuffer.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRPerMaterialUniform);
_SSR_PR_DEF(SSRMaterialInstanceIn);
_SSR_PR_DEF(SSRStudioIn);

class SSRPerMaterialUniform
{
    friend class SSRMaterialInstanceIn;
public:
    enum class Uid : SENUM8
    {
        eBaseColor,
        eBaseColor1,
        eBaseColor2,
        eBaseColor3,
        eBaseColor4,
        eBaseColor5,
        eBaseColor6,
        eBaseColor7,
        eBaseColor8,
        eBaseColor9,
        eBaseColor10,
        eBaseColor11,
        eBaseColor12,
        eBaseColor13,
        eBaseColor14,
        eBaseColor15,
    };
    enum class Sid : SENUM8
    {
        eBaseColorMap,
    };
protected:
    SSRPerMaterialUniform() noexcept;
    ~SSRPerMaterialUniform()noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRPerMaterialUniform);
    _SSR_CLASS_DELETE_MOVE(SSRPerMaterialUniform);
    _SSR_CLASS_MEMBER_GET_FUNC(SSRUniformCpuBufferPtr, UniformCpuBuffer, uniform_cpu_buffer,)
    _SSR_CLASS_MEMBER_GET_FUNC(SSRUniformInterfaceBlock, UniformInterfaceBlock, uniform_interface_block,)
public:
    auto prepare_uniform_buffer(SSRStudioInRef studio) noexcept -> void;
    auto destroy_uniform_buffer(SSRStudioInRef studio) noexcept -> void;
    auto update_uniform_buffer(const void* data, SCINT size) noexcept -> void;
    auto get_buffer() const noexcept -> _SSR_CORE::SSRBufferPtr;
private:
    _SSR_CORE::SSRBufferPtr  mUniformBuffer{};
    SSRUniformCpuBufferPtr   mUniformCpuBuffer{ nullptr };
    SSRUniformInterfaceBlock mUniformInterfaceBlock{ "PerMaterialUniformInterfaceBlock" };
    SSRSamplerInterfaceBlock mSamplerInterfaceBlock{ "PerMaterialSamplerInterfaceBlock" };

};


_SSR_END

#endif

