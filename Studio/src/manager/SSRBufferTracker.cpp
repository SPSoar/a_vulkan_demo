#include "manager/SSRBufferTracker.h"

_SSR_STUDIO_BEGIN

namespace 
{
    SUINT32 S_Id{ 0 };
}

SSRBufferTracker::~SSRBufferTracker() noexcept
{
    _SSR_ASSERT(!mStagingBuffer, _SSR_RDERROR);
}

auto SSRBufferTracker::init(SSRStudioRef& studio) noexcept -> void
{
    _SSR_CORE::RI_BufferInfo stagingBufferInfo{};
    stagingBufferInfo.mBufferUsage = _SSR_CORE::RE_BufferUsage::eStagingBuffer;
    stagingBufferInfo.mMemoryUsage = _SSR_CORE::RE_MemoryUsage::eCpuOnly;
    stagingBufferInfo.mDataSize = SSR_DEFAULT_LARGE_HEAP_BLOCK_SIZE;
    _SSR_ASSERT(!mStagingBuffer, _SSR_RDERROR);

    mStagingBuffer = studio.get_token().make_buffer(_SSR_STD::move(stagingBufferInfo));
    _SSR_ASSERT(mStagingBuffer, _SSR_RDERROR);
}

auto SSRBufferTracker::uninit(SSRStudioRef& studio) noexcept -> void
{
    _SSR_ASSERT(mStagingBuffer, _SSR_RDERROR);
    _SSR_LIKELY_IF(mStagingBuffer)
    {
        studio.get_token().delete_buffer(mStagingBuffer);
        mStagingBuffer = nullptr;
    }

}

auto SSRBufferTracker::require_buffer(SCINT size) noexcept -> Id
{
    auto id = S_Id++;
    if (mStagingBufferInfo.valid(size))
    {
        mRequireBufferBlocks[id].mTarget = mStagingBufferInfo.mStart;
        mRequireBufferBlocks[id].mBufferCopyInfo.mStagingBuffer = mStagingBuffer;
        mRequireBufferBlocks[id].mBufferCopyInfo.mStagingOffset = mStagingBufferInfo.mTop;
        mRequireBufferBlocks[id].mBufferCopyInfo.mBufferSize = size;
        mRequireBufferBlocks[id].mTarget = mStagingBufferInfo.mStart;
        mStagingBufferInfo.mTop = mStagingBufferInfo.mTop + size;
    }
    else
    {
        _SSR_ASSERT(false, _SSR_RDERROR);
    }

    return id;
}

auto SSRBufferTracker::obtain_buffer(const Id id) noexcept -> BufferCopyInfo&
{
    _SSR_ASSERT(has(mRequireBufferBlocks, id), _SSR_RDERROR);
    return mRequireBufferBlocks.at(id);
}

auto SSRBufferTracker::require_image(SCINT size) noexcept -> Id
{
    auto id = S_Id++;
    if (mStagingBufferInfo.valid(size))
    {
        mRequireImageBlocks[id].mTarget = mStagingBufferInfo.mStart;
        mRequireImageBlocks[id].mImageCopyInfo.mStagingBuffer = mStagingBuffer;
        mRequireImageBlocks[id].mImageCopyInfo.mStagingOffset = mStagingBufferInfo.mTop;
        mRequireImageBlocks[id].mImageCopyInfo.mBufferSize = size;
        mRequireImageBlocks[id].mTarget = mStagingBufferInfo.mStart;
        mStagingBufferInfo.mTop = mStagingBufferInfo.mTop + size;
    }
    else
    {
        _SSR_ASSERT(false, _SSR_RDERROR);
    }

    return id;
}

auto SSRBufferTracker::obtain_image(const Id id) noexcept -> ImageCopyInfo&
{
    _SSR_ASSERT(has(mRequireImageBlocks, id), _SSR_RDERROR);
    return mRequireImageBlocks.at(id);
}

auto SSRBufferTracker::begin(SSRStudioRef& studio) noexcept -> void
{
    mStagingBuffer->map_buffer(mStagingBufferInfo.mStart);
    mStagingBufferInfo.mTop = 0;

    _SSR_ASSERT(mRequireBufferBlocks.empty(), _SSR_RDERROR);
    _SSR_ASSERT(mRequireImageBlocks.empty(), _SSR_RDERROR);
}

auto SSRBufferTracker::end(SSRStudioRef& studio) noexcept -> void
{
    mStagingBuffer->unmap_buffer();

    mRequireBufferBlocks.clear();
    mRequireImageBlocks.clear();
}

_SSR_END