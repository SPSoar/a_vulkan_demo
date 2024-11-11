#pragma once
#ifndef SSR_SSRPERVIEWUNIFORM_H_
#define SSR_SSRPERVIEWUNIFORM_H_

#include "../common/SSRStudioHeadIn.h"
#include "core/SSRUniformInterfaceBlock.h"
#include "core/SSRUniformCpuBuffer.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRViewIn);
_SSR_PR_DEF(SSRPerViewUniform);

class SSRPerViewUniform
{
    friend class SSRViewIn;
public:
    enum class Uid : SENUM8
    {
        eViewMatrix,
        eProjectionMatrix,
        eProjectionXViewMatrix
    };
protected:
    SSRPerViewUniform() noexcept;
    ~SSRPerViewUniform() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRPerViewUniform);
    _SSR_CLASS_DELETE_MOVE(SSRPerViewUniform);
    _SSR_CLASS_MEMBER_GET_FUNC(SSRUniformCpuBufferPtr, UniformCpuBuffer, uniform_cpu_buffer,)
    _SSR_CLASS_MEMBER_GET_FUNC(SSRUniformInterfaceBlock, UniformInterfaceBlock, uniform_interface_block,)
private:
    SSRUniformCpuBufferPtr   mUniformCpuBuffer{ nullptr };
    SSRUniformInterfaceBlock mUniformInterfaceBlock{"PerViewUniformInterfaceBlock"};
};

_SSR_END

#endif

