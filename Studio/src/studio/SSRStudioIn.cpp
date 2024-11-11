#include "studio/SSRStudioIn.h"
#include "studio/SSRSceneIn.h"

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_IMPL_IN(SSRStudio)

_SSR_STUDIO_PUB_MEMBER_FUNC_IMPL(SSRStudio)

auto SSRStudio::Create(_SSR_CORE::SSRToken& token, const Config* config) -> SSRStudioUPtr
{
    return _SSR_MAKE_UNIQUE_IN(SSRStudio, token, config);
}

SSRStudioIn::SSRStudioIn(_SSR_CORE::SSRToken& token, const Config* config) noexcept
    : mToken{ token }
    , mRenderableManager{}
    , mPrimitiveManagerUPtr{ SSRPrimitiveManager::Create() }
    , mRenderManager{ *this }
{

}

SSRStudioIn::~SSRStudioIn() noexcept
{
    for (auto&& cmd : mPrimaryTransferCommands)
    {
        mToken.delete_transfer_command(cmd);
        cmd = nullptr;
    }
    mPrimaryTransferCommands.clear();

    for (auto&& cmd : mPrimaryDrawCommands)
    {
        mToken.delete_draw_command(cmd);
        cmd = nullptr;
    }
    mPrimaryDrawCommands.clear();
}
auto SSRStudioIn::get_render_manager() noexcept -> SSRRenderManager&
{
    return mRenderManager;
}

auto SSRStudioIn::get_renderable_manager() noexcept -> SSRRenderableManager&
{
    return mRenderableManager;
}

auto SSRStudioIn::get_renderable_manager() const noexcept -> const SSRRenderableManager&
{
    return mRenderableManager;
}

auto SSRStudioIn::get_primitive_manager() noexcept -> SSRPrimitiveManager&
{
    return *mPrimitiveManagerUPtr.get();
}
auto SSRStudioIn::get_primitive_manager() const noexcept -> const SSRPrimitiveManager&
{
    return *mPrimitiveManagerUPtr.get();
}

auto SSRStudioIn::create_scene(const E_ScenePolicy scenePolicy) noexcept -> SSRScenePtr
{
    return new SSRSceneIn(upcast<SSRPrimitiveManagerInRef>(get_primitive_manager()), scenePolicy);
}

auto SSRStudioIn::destroy_scene(SSRScenePtr& scene) noexcept -> void
{
    auto scenePtr = upcast<SSRSceneInPtr>(scene);
    _SSR_ASSERT(scenePtr, _SSR_RDERROR);

    scenePtr->clear_scene(get_render_manager());
    _SSR_DELETE_OBJECT_PTR(scenePtr);
    scene = nullptr;
}
auto SSRStudioIn::open() noexcept -> void
{
}

auto SSRStudioIn::close() noexcept -> void
{
    get_token().wait_idle();
}

auto SSRStudioIn::execute() noexcept -> void
{
    //auto res = mToken.wait_idle();
   // _SSR_ASSERT(res, _SSR_RDERROR);
}
auto SSRStudioIn::get_token() noexcept -> _SSR_CORE::SSRTokenRef
{
    return mToken;
}
auto SSRStudioIn::get_transfer_command(SUINT32 index) noexcept -> _SSR_CORE::SSRTransferCommandRef
{
    _SSR_ASSERT(index < mPrimaryTransferCommands.size(), _SSR_RDERROR);
    return *mPrimaryTransferCommands[index];
}
auto SSRStudioIn::make_primary_draw_commands(SUINT32 count) noexcept -> void
{
    _SSR_ASSERT(mPrimaryDrawCommands.empty(), _SSR_RDERROR);
    mPrimaryDrawCommands.resize(count);

    for (auto& cmd : mPrimaryDrawCommands)
    {
        _SSR_CORE::RI_DrawCommandInfo info{};
        cmd = mToken.make_draw_command(_SSR_STD::move(info));
        _SSR_ASSERT(cmd != nullptr, _SSR_RDERROR);
    }

    _SSR_ASSERT(mPrimaryTransferCommands.empty(), _SSR_RDERROR);
    mPrimaryTransferCommands.resize(count);

    for (auto& cmd : mPrimaryTransferCommands)
    {
        _SSR_CORE::RI_TransferCommandInfo info{};
        cmd = mToken.make_transfer_command(_SSR_STD::move(info));
        _SSR_ASSERT(cmd != nullptr, _SSR_RDERROR);
    }
}
auto SSRStudioIn::get_primary_draw_command(SUINT32 index) noexcept -> _SSR_CORE::SSRDrawCommandRef
{
    _SSR_ASSERT(index < mPrimaryDrawCommands.size(), _SSR_RDERROR);
    return *mPrimaryDrawCommands[index];
}
_SSR_END
