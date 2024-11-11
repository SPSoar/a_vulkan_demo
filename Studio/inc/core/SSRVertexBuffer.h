#pragma once
#ifndef SSR_SSRVERTEXBUFFER_H_
#define SSR_SSRVERTEXBUFFER_H_

#include "../common/SSRStudioHeadIn.h"


_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRVertexBuffer);

class SSRVertexBuffer
{
public:
    using E_AttributeType = _SSR_CORE::RE_Format;
public:
    SSRVertexBuffer() noexcept = default;
    ~SSRVertexBuffer() noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRVertexBuffer);
    _SSR_CLASS_DEFAULT_MOVE(SSRVertexBuffer);
public:
    auto begin() noexcept -> SSRVertexBuffer&;
    auto init_attribute_count(SUINT32 attributeCount) noexcept -> SSRVertexBuffer&;
    auto init_vertex_count(SUINT32 vertexCount) noexcept -> SSRVertexBuffer&;
    auto init_attribute(E_VertexAttribute attribute,
                         SUINT32 attributeIndex, 
                         E_AttributeType attributeType,
                         SUINT32 byteOffset = 0,
                         SUINT32 byteStride = 0) noexcept -> SSRVertexBuffer&;
    auto setup() noexcept -> SSRVertexBuffer&;
    auto setup_attribute_at(SUINT32 attributeIndex, const SSRBufferDescriptor& vertexInfo, SUINT32 bufferOffset = 0) noexcept -> SSRVertexBuffer&;
    auto end() noexcept -> void;
public:
    auto get_input_vertex_info() const noexcept -> const _SSR_CORE::RI_InputVertexInfo&;
    auto get_vertex_count() const noexcept -> SUINT32;
    auto get_vertex_size() const noexcept -> SCINT;
    auto copy_data(SUINT8* target, SCINT& offset) const noexcept -> void;
    auto get_bind_buffer_info() noexcept -> _SSR_CORE::RI_BindBufferInfo&;
protected:
    _SSR_CORE::RI_InputVertexInfo  mInputVertexInfo{};
    SSRVector<SSRBufferDescriptor> mBufferDescriptor{};
    _SSR_CORE::RI_BindBufferInfo   mBindBufferInfo{};
    SUINT32                        mVertexCount{ 0 };

};


_SSR_END

#endif

