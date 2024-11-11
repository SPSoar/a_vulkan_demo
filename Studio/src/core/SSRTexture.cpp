#include "core/SSRTexture.h"
#include "studio/SSRStudioIn.h"

_SSR_STUDIO_BEGIN


auto SSRTexture::Create(SSRStudioInRef studio) -> SSRTextureSPtr
{
    return _SSR_MAKE_SHARED(SSRTexture, studio);
}

SSRTexture::SSRTexture(SSRStudioInRef studio) noexcept
    : mStudio{ studio }
{

}

SSRTexture::~SSRTexture() noexcept
{
    if (mImage) 
    {
        mStudio.get_token().delete_image(mImage);
        mImage = nullptr;
    }
}

auto SSRTexture::init() noexcept -> SSRTexture&
{
    return *this;
}
auto SSRTexture::init_name(const char* name) noexcept -> SSRTexture&
{
    return *this;
}
auto SSRTexture::setup() noexcept -> SSRTexture&
{
    return *this;
}
auto SSRTexture::setup_raw_image(_SSR::SSRRawImage* rawImage) noexcept -> SSRTexture&
{
    _SSR_ASSERT(rawImage, _SSR_RDERROR);
    mRawImage = rawImage;
    return *this;
}
auto SSRTexture::commit() noexcept -> void
{

}

auto SSRTexture::prepare() noexcept -> void
{
    if (mBuildFinished.test(_SSR_STD::memory_order_relaxed))
    {
        return;
    }

    mBuildFinished.clear(_SSR_STD::memory_order_relaxed);

    _SSR_ASSERT(mRawImage, _SSR_RDERROR);
    _SSR_ASSERT(!mImage, _SSR_RDERROR);

    SCINT size = mRawImage->mHeight* mRawImage->mWidth* mRawImage->mChannel * sizeof(SUINT8);

    mId = mStudio.get_render_manager().get_buffer_tracker().require_image(size);

    _SSR_CORE::RI_ImageInfo texInfo{};
    texInfo.mName = "aaaaa";
    texInfo.mImageParams.i.dimension = _SSR_CORE::RE_ImageDimension::e2D;
    texInfo.mImageParams.i.format  = _SSR_CORE::RE_ImageFormat::eRGBA8U;
    texInfo.mImageParams.i.sampleCount = _SSR_CORE::RE_SampleCount::eCount1;
    texInfo.mImageParams.i.layout = _SSR_CORE::RE_ImageLayout::eReadOnly;
    texInfo.mUsage.set_texture(true);
    texInfo.mUsage.set_transfer_dst(true);
    texInfo.mUsage.set_combined(true);
    texInfo.mWidth = mRawImage->mWidth;
    texInfo.mHeight = mRawImage->mHeight;

    mImage = mStudio.get_token().make_image(std::move(texInfo));

}

auto SSRTexture::build(_SSR_CORE::SSRSubTransferCommandPtr transferCommand) noexcept -> void
{
    if (mBuildFinished.test_and_set(_SSR_STD::memory_order_seq_cst))
    {
        return;
    }
    auto& CopyInfo = mStudio.get_render_manager().get_buffer_tracker().obtain_image(mId);

    CopyInfo.mImageCopyInfo.mCopyOffsets.resize(1);

    CopyInfo.mImageCopyInfo.mCopyOffsets[0].mOffset = 0;
    CopyInfo.mImageCopyInfo.mCopyOffsets[0].mMipmapOffset.resize(1);
    CopyInfo.mImageCopyInfo.mCopyOffsets[0].mMipmapOffset[0] = 0;
    CopyInfo.mImageCopyInfo.mImage = mImage;

    auto&& data = CopyInfo.mTarget;

    void* target = data + CopyInfo.mImageCopyInfo.mStagingOffset;

    memcpy(target, mRawImage->mData, CopyInfo.mImageCopyInfo.mBufferSize);

    transferCommand->copy_image(CopyInfo.mImageCopyInfo);
}

_SSR_END