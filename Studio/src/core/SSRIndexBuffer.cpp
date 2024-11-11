#include "core/SSRIndexBuffer.h"
#include "studio/SSRStudioIn.h"

_SSR_STUDIO_BEGIN

auto SSRIndexBuffer::begin() noexcept -> SSRIndexBuffer&
{
    return *this;
}
auto SSRIndexBuffer::init_index_count(SUINT32 indexCount) noexcept -> SSRIndexBuffer&
{
    mIndexCount = indexCount;
    return *this;
}
auto SSRIndexBuffer::init_index_type(E_IndexType indexType) noexcept -> SSRIndexBuffer&
{
    mIndexType = indexType;
    return *this;
}
auto SSRIndexBuffer::setup() noexcept -> SSRIndexBuffer&
{
    mBindBufferInfo.mInfo.resize(1);
    return *this;
}

auto SSRIndexBuffer::setup_buffer(const SSRBufferDescriptor& buffer, SUINT32 byteOffset) noexcept -> SSRIndexBuffer&
{
    _SSR_UNUSED_P(byteOffset);
    mBufferDescriptor.mBuffer  = buffer.mBuffer;
    mBufferDescriptor.mSize = buffer.mSize;
    mBindBufferInfo.mInfo[0].mOffset = 0;
    return *this;
}

auto SSRIndexBuffer::end() noexcept -> void
{
}

auto SSRIndexBuffer::get_index_count() const noexcept -> SUINT32
{
    return mIndexCount;
}

auto SSRIndexBuffer::get_index_size() const noexcept -> SCINT
{
    return mBufferDescriptor.mSize;
}

auto SSRIndexBuffer::copy_data(SUINT8* target, SCINT& offset) const noexcept -> void
{
    void* data = target + offset;

    memcpy(data, mBufferDescriptor.mBuffer, mBufferDescriptor.mSize);

    offset += mBufferDescriptor.mSize;
}

auto SSRIndexBuffer::get_bind_buffer_info() noexcept -> _SSR_CORE::RI_BindBufferInfo&
{
    return mBindBufferInfo;
}

_SSR_END
