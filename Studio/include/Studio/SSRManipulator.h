#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRMANIPULATOR_H_
#define SSR_SSRMANIPULATOR_H_

_SSR_STUDIO_BEGIN

_SSR_UNIQUE_DEF_BASE(SSRManipulator, _SSR_STUDIO_EXPORT);

#define _SSR_MANIPULATOR_PUB_MEMBER_FUNC_DEF()\
    auto get_look_at(SVEC3& eyePosition, SVEC3& targetPosition, SVEC3& upward) const noexcept -> void;\
    auto set_target_position(const SVEC3& position) noexcept -> void;\
    auto set_eye(const SVEC3& eye) noexcept -> void;\
    auto set_up(const SVEC3& up) noexcept -> void;\
    auto set_viewport(SFLOAT64 width, SFLOAT64 height) noexcept -> void;\
    auto grab_begin(SFLOAT64 x, SFLOAT64 y, SBOOL strafe) noexcept -> void;\
    auto grab_update(SFLOAT64 x, SFLOAT64 y) noexcept -> void;\
    auto scroll(SFLOAT64 x, SFLOAT64 y, SFLOAT64 scrolldelta) noexcept -> void;\
    auto grab_end() noexcept -> void;\

#define _SSR_MANIPULATOR_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_CONST_NOEXCEPT(Class, void, get_look_at, SVEC3&, eyePosition, SVEC3&, targetPosition, SVEC3&, upward)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_target_position, const SVEC3&, position)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_eye, const SVEC3&, eye)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_up, const SVEC3&, up)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, set_viewport, SFLOAT64, width, SFLOAT64, height)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, grab_begin, SFLOAT64, x, SFLOAT64, y, SBOOL, strafe)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, grab_update, SFLOAT64, x, SFLOAT64, y)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, scroll, SFLOAT64, x, SFLOAT64, y, SFLOAT64, scrolldelta)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, grab_end,,)\


class _SSR_STUDIO_EXPORT SSRManipulator
{
protected:
    SSRManipulator() noexcept = default;
    ~SSRManipulator() = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRManipulator);
    _SSR_CLASS_DELETE_MOVE(SSRManipulator);
    _SSR_MANIPULATOR_PUB_MEMBER_FUNC_DEF()

public:
    static auto Create() -> SSRManipulatorUPtr;
};


_SSR_END

#endif

