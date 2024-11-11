#pragma once
#ifndef SSR_SSRSTUDIOIN_H_
#define SSR_SSRSTUDIOIN_H_

#include "../common/SSRStudioHeadIn.h"
#include "manager/SSRRenderableManager.h"
#include "manager/SSRRenderManager.h"

_SSR_STUDIO_BEGIN

template<class T>
concept CC_IsStudio = requires (T& t,
                                    _SSR_CORE::SSRToken& token,
                                    const SSRStudio::Config* config)
{
    { SSRStudio::Create(token, config) };
};

class SSRStudioIn : public SSRStudio
{
    _SSR_DELETER_FRIEND_DEF(SSRStudio)
protected:
    SSRStudioIn(_SSR_CORE::SSRToken& token, const Config* config) noexcept;
    ~SSRStudioIn() noexcept;
public:
    _SSR_CLASS_DELETE_COPY(SSRStudioIn);
    _SSR_CLASS_DELETE_MOVE(SSRStudioIn);
    _SSR_STUDIO_PUB_MEMBER_FUNC_DEF()
public:
    auto get_render_manager() noexcept -> SSRRenderManager&;
    auto get_renderable_manager() noexcept -> SSRRenderableManager&;
    auto get_renderable_manager() const noexcept -> const SSRRenderableManager&;
protected:
    _SSR_CORE::SSRTokenRef   mToken;
    SSRRenderableManager     mRenderableManager{};
    SSRPrimitiveManagerUPtr  mPrimitiveManagerUPtr{ nullptr };
    SSRRenderManager         mRenderManager{ *this };
private:
    SSRVector<_SSR_CORE::SSRDrawCommandPtr>     mPrimaryDrawCommands{};
    SSRVector<_SSR_CORE::SSRTransferCommandPtr> mPrimaryTransferCommands{};
};

static_assert(CC_IsStudio<SSRStudioIn>, "this is not a studio!");

_SSR_PR_DEF_S(SSRStudioIn);

_SSR_END

#endif

