#include "core/SSRPerViewUniform.h"
#include "studio/SSRStudioIn.h"

_SSR_STUDIO_BEGIN

SSRPerViewUniform::SSRPerViewUniform() noexcept
{
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eViewMatrix),           1, SSRUniformInterfaceBlock::Type::eMat4X4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eProjectionMatrix),     1, SSRUniformInterfaceBlock::Type::eMat4X4);
    mUniformInterfaceBlock.add_uniform(_SSR_EXPAND_STR(Uid::eProjectionXViewMatrix),1, SSRUniformInterfaceBlock::Type::eMat4X4);

    mUniformInterfaceBlock.build_uniform_interface_block();

    _SSR_CREATE_OBJECT_PTR(mUniformCpuBuffer, SSRUniformCpuBuffer, mUniformInterfaceBlock.size());
}

SSRPerViewUniform::~SSRPerViewUniform() noexcept
{
    _SSR_DELETE_OBJECT_PTR(mUniformCpuBuffer);
}

_SSR_END

