#include "core/SSRUniformInterfaceBlock.h"

_SSR_STUDIO_BEGIN

namespace
{
    uint8_t alignment_from_type(SSRUniformInterfaceBlock::Type type, bool isArray) noexcept
    {
        if (isArray)
        {
            // in std140 arrays are aligned to float4
            return 4;
        }

        using Type = SSRUniformInterfaceBlock::Type;
        switch (type)
        {
        case Type::eFloat:
        case Type::eInt:
        case Type::eUInt:
            return 1;
        case Type::eFloat2:
        case Type::eInt2:
        case Type::eUInt2:
            return 2;
        case Type::eFloat3:
        case Type::eFloat4:
        case Type::eInt3:
        case Type::eInt4:
        case Type::eUInt3:
        case Type::eUInt4:
        case Type::eMat3X4:
        case Type::eMat4X4:
        case Type::eStruct:
            return 4;
        case Type::eNone:
        case Type::eUUID:
        case Type::eStr:
            return 0;
        }
        return 0;
    }

    uint8_t  stride_for_type(SSRUniformInterfaceBlock::Type type, uint32_t size) noexcept
    {
        using Type = SSRUniformInterfaceBlock::Type;
        switch (type)
        {
        case Type::eInt:
        case Type::eUInt:
        case Type::eFloat:
            return 1;
        case Type::eInt2:
        case Type::eUInt2:
        case Type::eFloat2:
            return 2;
        case Type::eInt3:
        case Type::eUInt3:
        case Type::eFloat3:
            return 3;
        case Type::eInt4:
        case Type::eUInt4:
        case Type::eFloat4:
            return 4;
        case Type::eMat3X4:
            return 12;
        case Type::eMat4X4:
            return 16;
        case Type::eStruct:
            return cast<SUINT8>(size);
        case Type::eNone:
        case Type::eUUID:
        case Type::eStr:
            return 0;
        }
        return 0;
    }
}

using UniformOffsetSizeType = float;

SSRUniformInterfaceBlock::SSRUniformInterfaceBlock(const SSRCString& name) noexcept
    : mName{ name }
{
}

auto SSRUniformInterfaceBlock::build_uniform_interface_block() noexcept -> void
{
    mSize = 0;
    SUINT32 alignedOffset{ 0 };
    mUniformInfoMap.clear();
    for (SUINT32 i = 0; i < mUinformDescs.size(); i++)
    {
        auto const& uniform = mUinformDescs[i];
        //alignment 每个变量占用内存
        SUINT8  alignment = alignment_from_type(uniform.mType, uniform.mIsArray);
        //stride 每个变量有几个float
        uint8_t stride = stride_for_type(uniform.mType, uniform.mSize);

        if (uniform.mIsArray)
        {
            // the stride of an array is always rounded to its alignment (which is POT)
            stride = (stride + alignment - 1) & ~(alignment - 1);
        }
        alignedOffset += (alignment - (alignedOffset % alignment)) % alignment;

        auto& info = mUniformInfoMap[uniform.mName];
        info.mIsArray = uniform.mIsArray;
        info.mName = uniform.mName;
        info.mOffset = alignedOffset * sizeof(UniformOffsetSizeType);
        info.mStride = cast<SUINT32>(stride);
        info.mSize = cast<SUINT32>(uniform.mSize);
        info.mSlot = cast<SUINT8>(i);
        info.mType = uniform.mType;

        alignedOffset += stride * _SSR_STD::max(1u, info.mSize);
    }
    mSize = sizeof(UniformOffsetSizeType) * ((alignedOffset + 3) & ~3);

    mUinformDescs.clear();

    mUinformDescs.shrink_to_fit();
}

auto SSRUniformInterfaceBlock::add_uniform(const SSRCString& name, SUINT32 size, Type type) noexcept -> void
{
    SINT32 idx = index(name);
    if (idx > -1)
    {
        mUinformDescs[idx] = { name, type, size };
    }
    else
    {
        mUinformDescs.push_back({ name, type, size });
    }
}

auto SSRUniformInterfaceBlock::clear_uniform_info() noexcept -> void
{
    mUniformInfoMap.clear();
}

auto SSRUniformInterfaceBlock::offset(const SSRCString& name) noexcept -> SINT32
{
    auto itr = mUniformInfoMap.find(name);
    if (itr != mUniformInfoMap.end())
    {
        return cast<SINT32>(itr->second.mOffset);
    }
    return -1;
}

auto SSRUniformInterfaceBlock::index(const SSRCString& name) noexcept -> SINT32
{
    for (SINT32 i = 0; i < cast<SINT32>(mUinformDescs.size()); i++)
    {
        if (mUinformDescs[i].mName == name)
        {
            return i;
        }
    }
    return -1;
}

auto SSRUniformInterfaceBlock::size() const noexcept -> SUINT32
{
    return mSize;
}


_SSR_END

