#pragma once
#ifndef SSR_SSRBUFFERTRACKER_H_
#define SSR_SSRBUFFERTRACKER_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRBufferTracker
{
    static constexpr SCINT SSR_DEFAULT_LARGE_HEAP_BLOCK_SIZE = 256ull * 1024 * 1024;
public:
    using Id = SUINT32;
    struct BufferInfo
    {
        SUINT8*  mStart{ nullptr };
        SCINT    mTop{ 0 };
        bool valid(const SCINT size) noexcept 
        {
            return SSR_DEFAULT_LARGE_HEAP_BLOCK_SIZE >= size + mTop;
        }
    };
    struct BufferCopyInfo
    {
        _SSR_CORE::RI_BufferCopyInfo mBufferCopyInfo{};
        SUINT8*                      mTarget{ nullptr };
    };

    struct ImageCopyInfo
    {
        _SSR_CORE::RI_ImageCopyInfo mImageCopyInfo{};
        SUINT8*                     mTarget{ nullptr };
    };
public:
    SSRBufferTracker() noexcept = default;
    ~SSRBufferTracker()noexcept;
    _SSR_CLASS_DELETE_COPY(SSRBufferTracker);
    _SSR_CLASS_DELETE_MOVE(SSRBufferTracker);
public:
    auto init(SSRStudioRef& studio) noexcept -> void;
    auto uninit(SSRStudioRef& studio) noexcept -> void;
    auto require_buffer(SCINT size) noexcept -> Id;
    auto obtain_buffer(Id id) noexcept -> BufferCopyInfo&;
    auto require_image(SCINT size) noexcept -> Id;
    auto obtain_image(Id id) noexcept -> ImageCopyInfo&;
    auto begin(SSRStudioRef& studio)  noexcept -> void;
    auto end(SSRStudioRef& studio)  noexcept -> void;
protected:
    _SSR_CORE::SSRBufferPtr     mStagingBuffer{ nullptr };
    BufferInfo                  mStagingBufferInfo{};
    SSRMap<Id, BufferCopyInfo>  mRequireBufferBlocks{};
    SSRMap<Id, ImageCopyInfo>   mRequireImageBlocks{};
};


_SSR_END

#endif

