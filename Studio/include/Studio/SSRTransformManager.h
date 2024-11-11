#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRTRANSFORMMANAGER_H_
#define SSR_SSRTRANSFORMMANAGER_H_

_SSR_STUDIO_BEGIN


_SSR_UNIQUE_DEF_BASE(SSRTransformManager, _SSR_STUDIO_EXPORT);

#define _SSR_TRANSFORM_MANAGER_PUB_MEMBER_FUNC_DEF()\
    //auto get_token(SSRToken::E_TokenType tokenType) const noexcept -> SSRToken&;

#define _SSR_TRANSFORM_MANAGER_PUB_MEMBER_FUNC_IMPL(Class)\
    //_SSR_CLASS_BASE_FUNC_OUT_CONST_NOEXCEPT(Class, SSRToken&, get_token, SSRToken::E_TokenType, tokenType)

#define _SSR_TRANSFORM_MANAGER_IN_PUB_MEMBER_FUNC_IMPL(Class)\
    //_SSR_CLASS_DERIVE_FUNC_OUT_CONST_NOEXCEPT(Class, SSRToken&, get_token, SSRToken::E_TokenType, tokenType)

class _SSR_STUDIO_EXPORT SSRTransformManager
{

protected:
    SSRTransformManager() noexcept = default;
    ~SSRTransformManager()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRTransformManager);
    _SSR_CLASS_DELETE_MOVE(SSRTransformManager);
    _SSR_TRANSFORM_MANAGER_PUB_MEMBER_FUNC_DEF()

public:
    static auto Create() -> SSRTransformManagerUPtr;
};


_SSR_END

#endif

