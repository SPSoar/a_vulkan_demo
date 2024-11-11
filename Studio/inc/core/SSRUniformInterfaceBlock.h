#pragma once
#ifndef SSR_SSRUNIFORMINTERFACEBLOCK_H_
#define SSR_SSRUNIFORMINTERFACEBLOCK_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

class SSRUniformInterfaceBlock
{
public:
    using Type = SSRKeeper::E_Type;
    struct UinformDesc
    {
        SSRCString mName{};
        Type       mType{ Type::eNone };
        SUINT32    mSize{ 0 };
        SBOOL      mIsArray{ false };
    };
    using UinformDescs = SSRVector<UinformDesc>;
    struct UinformInfo
    {
        SSRCString  mName{};
        Type        mType{ Type::eNone };
        SUINT32     mOffset{ 0 };
        SUINT32     mStride{ 0 };
        SUINT32     mSize{ 0 };
        SUINT8      mSlot{ 0 };
        SBOOL       mIsArray{ false };
    };
    using UniformInfoMap = SSRMap<SSRCString, UinformInfo>;
public:
    SSRUniformInterfaceBlock() noexcept = delete;
    explicit SSRUniformInterfaceBlock(const SSRCString& name) noexcept;
    ~SSRUniformInterfaceBlock() noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRUniformInterfaceBlock);
    _SSR_CLASS_DELETE_MOVE(SSRUniformInterfaceBlock);
public:
    auto build_uniform_interface_block() noexcept -> void;
    auto add_uniform(const SSRCString& name, SUINT32 size, Type type) noexcept -> void;
    auto offset(const SSRCString& name) noexcept -> SINT32;
    auto index(const SSRCString& name) noexcept -> SINT32;
    auto size() const noexcept -> SUINT32;
protected:
    auto clear_uniform_info() noexcept-> void;
protected:
    SSRCString      mName{ "UniformInInterfaceBlock" };
    UniformInfoMap  mUniformInfoMap{};
    UinformDescs    mUinformDescs{};
    SUINT32         mSize{ 0 };

};


_SSR_END

#endif

