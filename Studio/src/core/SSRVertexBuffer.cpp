#include "core/SSRVertexBuffer.h"
#include "studio/SSRStudioIn.h"

_SSR_STUDIO_BEGIN

auto SSRVertexBuffer::begin() noexcept -> SSRVertexBuffer&
{
    mInputVertexInfo.mInstanced = false;
    return *this;
}

auto SSRVertexBuffer::init_attribute_count(SUINT32 attributeCount) noexcept -> SSRVertexBuffer&
{
    mInputVertexInfo.mAttributes.resize(attributeCount);
    mBufferDescriptor.resize(attributeCount);
    mBindBufferInfo.mInfo.resize(attributeCount);
    return *this;
}
auto SSRVertexBuffer::init_vertex_count(SUINT32 vertexCount) noexcept -> SSRVertexBuffer&
{
    mVertexCount = vertexCount;
    return *this;
}
auto SSRVertexBuffer::init_attribute(E_VertexAttribute attribute,
                                       SUINT32 attributeIndex,
                                       E_AttributeType attributeType, 
                                       SUINT32 byteOffset, 
                                       SUINT32 byteStride) noexcept -> SSRVertexBuffer& 
{
    mInputVertexInfo.mAttributes[attributeIndex].mByteOffset = byteOffset;
    mInputVertexInfo.mAttributes[attributeIndex].mByteStride = byteStride;
    mInputVertexInfo.mAttributes[attributeIndex].mFormat = attributeType;
    //mVertexAttribute = attribute;
    //mAttributeType   = attributeType;
    //mInputVertexInfo.mBufferIndex = attributeIndex;
    //mInputVertexInfo.mByteOffset  = byteOffset;
    //mInputVertexInfo.mByteStride  = byteStride;
    return *this;
}

auto SSRVertexBuffer::setup() noexcept -> SSRVertexBuffer&
{
    return *this;
}

auto SSRVertexBuffer::setup_attribute_at(SUINT32 attributeIndex, const SSRBufferDescriptor& vertexInfo, SUINT32 bufferOffset) noexcept -> SSRVertexBuffer&
{
    mBufferDescriptor[attributeIndex].mBuffer = vertexInfo.mBuffer;
    mBufferDescriptor[attributeIndex].mSize   = vertexInfo.mSize;
    mBindBufferInfo.mInfo[attributeIndex].mOffset = bufferOffset;
    return *this;
}

auto SSRVertexBuffer::end() noexcept -> void
{
}

auto SSRVertexBuffer::get_vertex_count() const noexcept -> SUINT32
{
    return mVertexCount;
}
auto SSRVertexBuffer::get_input_vertex_info() const noexcept -> const _SSR_CORE::RI_InputVertexInfo&
{
    return mInputVertexInfo;
}

auto SSRVertexBuffer::get_vertex_size() const noexcept -> SCINT
{
    SCINT size{ 0 };
    for (auto&& desc : mBufferDescriptor)
    {
        size += desc.mSize;
    }
    _SSR_ASSERT(size != 0, _SSR_RDERROR);
    return size;
}

auto SSRVertexBuffer::copy_data(SUINT8* target, SCINT& offset) const noexcept -> void
{
    for (auto&& desc : mBufferDescriptor)
    {
        void* data = target + offset;

        memcpy(data, desc.mBuffer, desc.mSize);

        offset += desc.mSize;
    }
}

auto SSRVertexBuffer::get_bind_buffer_info() noexcept -> _SSR_CORE::RI_BindBufferInfo&
{
    return mBindBufferInfo;
}



_SSR_END