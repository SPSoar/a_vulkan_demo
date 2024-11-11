#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRMATERIALINSTANCE_H_
#define SSR_SSRMATERIALINSTANCE_H_

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRStudio);
_SSR_SHARED_DEF(SSRMaterialInstance);

#define _SSR_MATERIAL_INSTANCE_PUB_MEMBER_FUNC_DEF()\
    //auto get_token(SSRToken::E_TokenType tokenType) const noexcept -> SSRToken&;

#define _SSR_MATERIAL_INSTANCE_PUB_MEMBER_FUNC_IMPL(Class)\
    //_SSR_CLASS_BASE_FUNC_OUT_CONST_NOEXCEPT(Class, SSRToken&, get_token, SSRToken::E_TokenType, tokenType)

#define _SSR_MATERIAL_INSTANCE_IN_PUB_MEMBER_FUNC_IMPL(Class)\
    //_SSR_CLASS_DERIVE_FUNC_OUT_CONST_NOEXCEPT(Class, SSRToken&, get_token, SSRToken::E_TokenType, tokenType)

class _SSR_STUDIO_EXPORT SSRMaterialInstance
{

protected:
    SSRMaterialInstance() noexcept = default;
    ~SSRMaterialInstance()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRMaterialInstance);
    _SSR_CLASS_DELETE_MOVE(SSRMaterialInstance);
    _SSR_MATERIAL_INSTANCE_PUB_MEMBER_FUNC_DEF()
};


_SSR_END

#endif

