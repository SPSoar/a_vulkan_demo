#include "core/SSRUniformCpuBuffer.h"

_SSR_STUDIO_BEGIN


SSRUniformCpuBuffer::SSRUniformCpuBuffer(const SCINT size) noexcept
    : mBuffer{ mStorage }
    , mSize{ cast<SUINT32>(size) }
    , mIsDirty{true}
{
    _SSR_LIKELY_IF(size > sizeof(mStorage))
    {
        mBuffer = ::malloc(size);
    }
    ::memset(mBuffer, 0, size);
}
SSRUniformCpuBuffer::~SSRUniformCpuBuffer() noexcept
{
    _SSR_LIKELY_IF(mBuffer && !is_local_storage())
    {
        ::free(mBuffer);
    }
    mSize = 0;
}

auto SSRUniformCpuBuffer::set_uniforms(const SSRUniformCpuBuffer& rhs) noexcept -> SSRUniformCpuBuffer&
{
    if (this != &rhs) 
    {
        _SSR_UNLIKELY_IF(mSize != rhs.mSize)
        {
            if (mBuffer && !is_local_storage())
            {
                ::free(mBuffer);
                mSize = 0;
            }
            mBuffer = mStorage;
            mSize   = rhs.mSize;
            if (mSize > sizeof(mStorage))
            {
                mBuffer = ::malloc(mSize);
            }
        }
        memcpy(mBuffer, rhs.mBuffer, rhs.mSize);

        invalidate();
    }
    return *this;
}

_SSR_END

