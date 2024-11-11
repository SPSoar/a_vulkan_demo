#include "core/SSRTextureGroup.h"

_SSR_STUDIO_BEGIN

auto SSRTextureGroup::prepare() noexcept -> void
{
    if (!mDirty) 
    {
        return;
    }

    for (auto&& texture : mTextures)
    {
        texture->prepare();
    }
}

auto SSRTextureGroup::build(_SSR_CORE::SSRSubTransferCommandPtr transferCommand) noexcept -> void
{
    if (!mDirty)
    {
        return;
    }

    for (auto&& texture : mTextures)
    {
        texture->build(transferCommand);
    }

    mDirty = false;
}


_SSR_END
