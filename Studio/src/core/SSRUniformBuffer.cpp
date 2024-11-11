#include "core/SSRUniformBuffer.h"
#include "studio/SSRStudioIn.h"

_SSR_STUDIO_BEGIN

auto SSRUniformBuffer::init() noexcept -> SSRUniformBuffer&
{
    return *this;
}

auto SSRUniformBuffer::setup() noexcept -> SSRUniformBuffer&
{
    return *this;
}

auto SSRUniformBuffer::setup_buffer(const SSRBufferDescriptor& buffer, SUINT32 byteOffset) noexcept -> SSRUniformBuffer&
{
    _SSR_UNUSED_P(byteOffset);
    mBufferInfo.mDataSize = buffer.mSize;
    mBufferInfo.mBufferUsage = _SSR_CORE::RE_BufferUsage::eUniformBuffer;
    mBufferInfo.mMemoryUsage = _SSR_CORE::RE_MemoryUsage::eCpuToGpu;
    return *this;
}

auto SSRUniformBuffer::commit(SSRStudioPtr studio) noexcept -> void
{
    //auto studioIn = static_cast<SSRStudioIn*>(studio);

    //mBuffer = { studio, studioIn->get_token().make_buffer(_SSR_STD::move(mBufferInfo)) };
}
auto SSRUniformBuffer::upload_buffer(SSRStudioPtr studio) noexcept -> void
{

    //if(mBuffer.handle())
    //{
    //    mBuffer.handle()->upload_buffer();
    //}
}

_SSR_END