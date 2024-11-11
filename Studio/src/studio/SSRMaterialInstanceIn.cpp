#include "studio/SSRMaterialInstanceIn.h"

_SSR_STUDIO_BEGIN

_SSR_SHARED_IMPL_IN(SSRMaterialInstance)

_SSR_MATERIAL_INSTANCE_PUB_MEMBER_FUNC_IMPL(SSRMaterialInstance)

SSRMaterialInstanceIn::~SSRMaterialInstanceIn() noexcept
{
    mPerMaterialUniform.destroy_uniform_buffer(mStudio);
}

auto SSRMaterialInstanceIn::update_material_info() noexcept -> void
{
    using Uid = SSRPerMaterialUniform::Uid;

    auto&& uniformBuffer = mPerMaterialUniform.get_uniform_cpu_buffer();
    auto&& uniformInterfaceBlock = mPerMaterialUniform.get_uniform_interface_block();

    _SSR_STUDIO_UBO_SET(uniformInterfaceBlock, uniformBuffer, Uid::eBaseColor, SVEC4(1.f, 0.f, 1.f, 1.f));
}

auto SSRMaterialInstanceIn::prepare() noexcept -> void
{
    _SSR_ASSERT(mMaterial, _SSR_RDERROR);

    mMaterial->prepare();

    mPerMaterialUniform.prepare_uniform_buffer(mStudio);

    // prepare 主线程完成，不需要定序
    mBuildFinished.clear(_SSR_STD::memory_order_relaxed);

}

auto SSRMaterialInstanceIn::build(_SSR_CORE::SSRSubTransferCommandPtr tansferCommand) noexcept -> void
{
    _SSR_ASSERT(mMaterial, _SSR_RDERROR);

    mMaterial->build(tansferCommand);

    // prepare 多线程，只有第一次进入时 buil 
    if (mBuildFinished.test_and_set(_SSR_STD::memory_order_seq_cst))
    {
        return;
    }

    update_material_info();

    auto&& uniformBuffer = mPerMaterialUniform.get_uniform_cpu_buffer();

    mPerMaterialUniform.update_uniform_buffer(uniformBuffer->get_buffer(), uniformBuffer->get_size());
}

_SSR_END