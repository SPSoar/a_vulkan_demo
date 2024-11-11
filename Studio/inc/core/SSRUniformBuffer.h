#pragma once
#ifndef SSR_SSRUNIFORMBUFFER_H_
#define SSR_SSRUNIFORMBUFFER_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRUniformBuffer);

class SSRUniformBuffer 
{
public:
    SSRUniformBuffer()  noexcept = default;
    ~SSRUniformBuffer() noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRUniformBuffer);
    _SSR_CLASS_DELETE_MOVE(SSRUniformBuffer);
public:
    auto init() noexcept -> SSRUniformBuffer&;
    auto setup() noexcept -> SSRUniformBuffer&;
    auto setup_buffer(const SSRBufferDescriptor& buffer, SUINT32 byteOffset = 0) noexcept -> SSRUniformBuffer&;
    auto commit(SSRStudioPtr studio) noexcept -> void;
    auto upload_buffer(SSRStudioPtr studio) noexcept -> void;
protected:
    _SSR_CORE::RI_BufferInfo mBufferInfo{};
};


_SSR_END

#endif

