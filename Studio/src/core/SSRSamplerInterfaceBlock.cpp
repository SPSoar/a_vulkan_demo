#include "core/SSRSamplerInterfaceBlock.h"

_SSR_STUDIO_BEGIN

using SamplerOffsetSizeType = float;

SSRSamplerInterfaceBlock::SSRSamplerInterfaceBlock(const SSRCString& name) noexcept
    : mName{ name }
{
}

auto SSRSamplerInterfaceBlock::add_sampler(const SSRCString& name, Type type) noexcept -> void
{
    SINT32 index = offset(name);
    if (index < 0)
    {
        index = cast<SINT32>(size());
    }
    mSamplerInfoMap[name] = { name,
                              type,
                              cast<uint32_t>(index * sizeof(SamplerOffsetSizeType)),
                              cast<uint8_t>(index),
                              E_SamplerFormat::eInt,
                              Precision::eMedium,
                              false };
}

auto SSRSamplerInterfaceBlock::clear_sampler_info() noexcept -> void
{
    mSamplerInfoMap.clear();
}

auto SSRSamplerInterfaceBlock::offset(const SSRCString& name) noexcept -> SINT32
{
    auto itr = mSamplerInfoMap.find(name);
    if (itr != mSamplerInfoMap.end()) 
    {
        return itr->second.mSlot;
    }
    return -1;
}

auto SSRSamplerInterfaceBlock::size() const noexcept -> SUINT32
{
    return cast<SUINT32>(mSamplerInfoMap.size());
}


_SSR_END

