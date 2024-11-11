#pragma once
#ifndef SSR_SSRSAMPLERGROUP_H_
#define SSR_SSRSAMPLERGROUP_H_

#include "../common/SSRStudioHeadIn.h"
#include "core/SSRTexture.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRTextureGroup);

class SSRTextureGroup
{
public:
    SSRTextureGroup() noexcept = delete;
    explicit SSRTextureGroup(const SCINT size) noexcept
        : mTextures{ size }
    {
        mTextures.shrink_to_fit();
    }
    ~SSRTextureGroup() noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRTextureGroup);
    _SSR_CLASS_DELETE_MOVE(SSRTextureGroup);
public:
    auto prepare() noexcept -> void;
    auto build(_SSR_CORE::SSRSubTransferCommandPtr transferCommand) noexcept -> void;

    auto get_size() const noexcept -> SCINT
    { 
        return cast<SCINT>(mTextures.size());
    }

    void set_texture(SCINT index, const SSRTextureSPtr& textures) noexcept
    {
        _SSR_ASSERT(index < mTextures.size(), _SSR_RDERROR);
        mTextures[index] = textures;
        mDirty = true;
    }

    auto delete_texture(SCINT index) noexcept
    {
        set_texture(index, {});
    }

    auto texture(SCINT index) noexcept -> SSRTexture&
    {
        _SSR_ASSERT(index < mTextures.size(), _SSR_RDERROR);
        return *mTextures[index];
    }

private:
    SSRFitVector<SSRTextureSPtr> mTextures{};
    mutable bool                 mDirty{ false };
};

_SSR_END

#endif

