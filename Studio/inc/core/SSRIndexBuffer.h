#pragma once
#ifndef SSR_SSRINDEXBUFFER_H_
#define SSR_SSRINDEXBUFFER_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRIndexBuffer);

class SSRIndexBuffer
{
public:
    enum class E_IndexType : SENUM8
    {
        eUShort = cast<SENUM8>(_SSR_CORE::RE_Format::eR8Uint),
        eUInt   = cast<SENUM8>(_SSR_CORE::RE_Format::eR32Uint)
    };
public:
    SSRIndexBuffer()  noexcept = default;
    ~SSRIndexBuffer() noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRIndexBuffer);
    _SSR_CLASS_DEFAULT_MOVE(SSRIndexBuffer);
public:
    auto begin() noexcept -> SSRIndexBuffer&;
    auto init_index_count(SUINT32 indexCount) noexcept -> SSRIndexBuffer&;
    auto init_index_type(E_IndexType indexType) noexcept -> SSRIndexBuffer&;
    auto setup() noexcept -> SSRIndexBuffer&;
    auto setup_buffer(const SSRBufferDescriptor& buffer, SUINT32 byteOffset = 0) noexcept -> SSRIndexBuffer&;
    auto end() noexcept -> void;
public:
    auto get_index_count() const noexcept -> SUINT32;
    auto get_index_size() const noexcept -> SCINT;
    auto copy_data(SUINT8* target, SCINT& offset) const noexcept -> void;
    auto get_bind_buffer_info() noexcept -> _SSR_CORE::RI_BindBufferInfo&;
protected:
    _SSR_CORE::RI_InputIndexInfo mInputIndexInfo{};
    SSRBufferDescriptor          mBufferDescriptor{};
    _SSR_CORE::RI_BindBufferInfo mBindBufferInfo{};
    SUINT32                      mIndexCount{ 0 };
    E_IndexType                  mIndexType{ E_IndexType::eUInt };

};


_SSR_END

#endif

