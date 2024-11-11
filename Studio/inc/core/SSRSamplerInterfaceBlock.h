#pragma once
#ifndef SSR_SSRSAMPLERINTERFACEBLOCK_H_
#define SSR_SSRSAMPLERINTERFACEBLOCK_H_

#include "../common/SSRStudioHeadIn.h"


_SSR_STUDIO_BEGIN

enum class E_SamplerType : SENUM8
{
    eSampler2d,
    eSampler2dArray,
    eSamplerCubemap,
    eSampleExternal,
    eSampler3d,
    eSamplerCubemapArray,
};
enum class E_Precision : SENUM8
{
    eDefault,
    eLow,
    eMedium,
    eHigh
};

enum class E_SamplerFormat : SENUM8
{
    eInt,
    eUint,
    eFloat,
    eShadow
};

class SSRSamplerInterfaceBlock
{
public:
    using Type          = E_SamplerType;
    using Format        = E_SamplerFormat;
    using Precision     = E_Precision;
    using SamplerParams = _SSR_CORE::RI_SamplerParams;

    struct SamplerInfo
    {
        SSRCString mName{};
        Type       mType{};
        uint32_t   mOffset{};
        uint8_t    mSlot{};
        Format     mFormat{};
        Precision  mPrecision{};
        bool       mMultisample{};
    };
    using SamplerInfoMap = SSRMap<SSRCString, SamplerInfo>;
public:
    SSRSamplerInterfaceBlock() = delete;
    explicit SSRSamplerInterfaceBlock(const SSRCString& name) noexcept;
    ~SSRSamplerInterfaceBlock() = default;
    _SSR_CLASS_DELETE_COPY(SSRSamplerInterfaceBlock);
    _SSR_CLASS_DELETE_MOVE(SSRSamplerInterfaceBlock);
public:
    auto add_sampler(const SSRCString& name, Type type) noexcept -> void;
    auto clear_sampler_info() noexcept-> void;
    auto offset(const SSRCString& name) noexcept -> SINT32;
    auto size() const noexcept -> SUINT32;
protected:
    SamplerInfoMap mSamplerInfoMap{};
    SSRCString     mName{"SamplerInterfaceBlock"};
   // backend::ShaderStageFlags mStageFlags{};
};
_SSR_END

#endif

