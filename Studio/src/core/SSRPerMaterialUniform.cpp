#include "core/SSRPerMaterialUniform.h"
#include "studio/SSRStudioIn.h"

_SSR_STUDIO_BEGIN

SSRPerMaterialUniform::SSRPerMaterialUniform() noexcept
{
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor1), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor2), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor3), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor4), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor5), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor6), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor7), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor8), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor9), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor10), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor11), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor12), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor13), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor14), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eBaseColor15), 1, SSRUniformInterfaceBlock::Type::eFloat4);
    
    mUniformInterfaceBlock.build_uniform_interface_block();

    mSamplerInterfaceBlock.add_sampler(_SSR_EXPAND_STR(Sid::eBaseColorMap), SSRSamplerInterfaceBlock::Type::eSampler2d);

    _SSR_CREATE_OBJECT_PTR(mUniformCpuBuffer, SSRUniformCpuBuffer, mUniformInterfaceBlock.size());
}

SSRPerMaterialUniform::~SSRPerMaterialUniform() noexcept
{
    _SSR_DELETE_OBJECT_PTR(mUniformCpuBuffer);
}

auto SSRPerMaterialUniform::update_uniform_buffer(const void* data, const SCINT size) noexcept -> void
{
    _SSR_ASSERT(mUniformBuffer, _SSR_RDERROR);
    _SSR_LIKELY_IF (mUniformBuffer)
    {
        mUniformBuffer->upload_buffer(data, size);
    }
}

auto SSRPerMaterialUniform::get_buffer() const noexcept -> _SSR_CORE::SSRBufferPtr
{
    return mUniformBuffer;
}

auto SSRPerMaterialUniform::prepare_uniform_buffer(SSRStudioInRef studio) noexcept -> void
{
    if (!mUniformBuffer)
    {
        _SSR_CORE::RI_BufferInfo bufferInfo{};
        bufferInfo.mDataSize = mUniformCpuBuffer->get_size();
        bufferInfo.mBufferUsage = _SSR_CORE::RE_BufferUsage::eUniformBuffer;
        bufferInfo.mMemoryUsage = _SSR_CORE::RE_MemoryUsage::eCpuToGpu;
        mUniformBuffer = studio.get_token().make_buffer(_SSR_STD::move(bufferInfo));
    }
}

auto SSRPerMaterialUniform::destroy_uniform_buffer(SSRStudioInRef studio) noexcept -> void
{
    _SSR_LIKELY_IF (mUniformBuffer) 
    {
        studio.get_token().delete_buffer(mUniformBuffer);
        mUniformBuffer = nullptr;
    }
}

_SSR_END

