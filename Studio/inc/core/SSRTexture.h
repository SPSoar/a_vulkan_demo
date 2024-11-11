#pragma once
#ifndef SSR_SSRTEXTURE_H_
#define SSR_SSRTEXTURE_H_

#include "../common/SSRStudioHeadIn.h"
#include "manager/SSRBufferTracker.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRStudioIn);
_SSR_SHARED_DEF(SSRTexture);

class SSRTexture 
{
public:
    SSRTexture() noexcept= delete;
    explicit SSRTexture(SSRStudioInRef studio)  noexcept;
    ~SSRTexture() noexcept;
    _SSR_CLASS_DELETE_COPY(SSRTexture);
    _SSR_CLASS_DELETE_MOVE(SSRTexture);
    static auto Create(SSRStudioInRef studio) -> SSRTextureSPtr;
public:
    auto init() noexcept -> SSRTexture&;
    auto init_name(const char* name) noexcept -> SSRTexture&;
    auto setup() noexcept -> SSRTexture&;
    auto setup_raw_image(_SSR::SSRRawImage* rawImage) noexcept -> SSRTexture&;
    auto commit() noexcept -> void;

    auto prepare() noexcept -> void;
    auto build(_SSR_CORE::SSRSubTransferCommandPtr transferCommand) noexcept -> void;

    auto image() const noexcept 
    {
        return mImage;
    }
protected:
    SSRStudioInRef          mStudio;
    _SSR_STD::atomic_flag   mBuildFinished ATOMIC_FLAG_INIT;
    _SSR::SSRRawImage*      mRawImage{ nullptr };
    _SSR_CORE::SSRImagePtr  mImage{ nullptr };
    SSRBufferTracker::Id    mId{ ~0U };
};
_SSR_END

#endif

