#pragma once
#ifndef SSR_SSRUNIFORMCPUBUFFER_H_
#define SSR_SSRUNIFORMCPUBUFFER_H_

#include "../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRUniformCpuBuffer);

template<class T>
concept CC_IsUniformBufferSupportedType = _SSR_STD::is_same_v<SFLOAT32, T> ||
                                          _SSR_STD::is_same_v<SINT32,   T> ||
                                          _SSR_STD::is_same_v<SUINT32,  T> ||
                                          _SSR_STD::is_same_v<SIVEC2,   T> ||
                                          _SSR_STD::is_same_v<SIVEC3,   T> ||
                                          _SSR_STD::is_same_v<SIVEC4,   T> ||
                                          _SSR_STD::is_same_v<SUIVEC2,  T> ||
                                          _SSR_STD::is_same_v<SUIVEC3,  T> ||
                                          _SSR_STD::is_same_v<SUIVEC4,  T> ||
                                          _SSR_STD::is_same_v<SVEC2,    T> ||
                                          _SSR_STD::is_same_v<SVEC3,    T> ||
                                          _SSR_STD::is_same_v<SVEC4,    T> ||
                                          _SSR_STD::is_same_v<SMAT3X4,  T> ||
                                          _SSR_STD::is_same_v<SMAT4X4,  T>;

class SSRUniformCpuBuffer
{ 

    static constexpr SUINT32 SCV_Storage_Size = 96;
public:
    SSRUniformCpuBuffer() noexcept = delete;
    explicit SSRUniformCpuBuffer(SCINT size) noexcept;
    ~SSRUniformCpuBuffer() noexcept;
    _SSR_CLASS_DELETE_COPY(SSRUniformCpuBuffer);
    _SSR_CLASS_DELETE_MOVE(SSRUniformCpuBuffer);
    _SSR_CLASS_MEMBER_GET_IS_FUNC(bool, IsDirty, dirty,)
public:
    auto get_buffer() const noexcept -> void*
    {
        return mBuffer;
    }
    auto get_size() const noexcept -> SCINT
    {
        return cast<SCINT>(mSize);
    }
    auto set_uniforms(const SSRUniformCpuBuffer& rhs) noexcept -> SSRUniformCpuBuffer&;

    //template<CC_IsUniformBufferSupportedType T>
    //    requires _SSR_STD::negation_v<_SSR_STD::is_same<T, SMAT3X4>>
    //_SSR_ALWAYS_INLINE
    //auto set_uniform_array(SCINT offset, const T* _SSR_RESTRICT begin, SCINT count) noexcept -> void
    //{
    //    set_uniform_array_untyped<sizeof(T)>(offset, begin, count);
    //}

    //template<CC_IsUniformBufferSupportedType T>
    //    requires _SSR_STD::is_same_v<T, SMAT3X4>
    //_SSR_ALWAYS_INLINE
    //auto set_uniform_array(SCINT offset, const T* _SSR_RESTRICT begin, SCINT count) noexcept -> void
    //{
    //    //set_uniform_array_untyped<T::bsize()>(offset, reinterpret_cast<const SVEC4*>(begin), count * 3);
    //}

    template<CC_IsUniformBufferSupportedType T>
        requires is_vec<T>::value
    _SSR_ALWAYS_INLINE
    static inline auto set_uniform(void* addr, SCINT offset, const T& v) noexcept -> void
    {
        set_uniform_untyped<T::bsize()>(addr, 0ul, v.data());
    }

    template<CC_IsUniformBufferSupportedType T>
        requires is_mat<T>::value
    _SSR_ALWAYS_INLINE
    static inline auto set_uniform(void* addr, SCINT offset, const T& v) noexcept -> void
    {
        set_uniform_untyped<T::bsize()>(addr, 0ul, v.data());
    }

    template<CC_IsUniformBufferSupportedType T>
        requires _SSR_STD::negation_v<_SSR_STD::is_arithmetic<T>>
    _SSR_ALWAYS_INLINE
    inline auto set_uniform(SCINT offset, const T& v) noexcept -> void
    {
        set_uniform(invalidate_uniforms(offset, T::bsize()), 0, v);
    }

    template<CC_IsUniformBufferSupportedType T>
        requires _SSR_STD::is_arithmetic_v<T>
    _SSR_ALWAYS_INLINE
    inline auto set_uniform(SCINT offset, const T& v) noexcept -> void
    {
        set_uniform_untyped<sizeof(T)>(offset, &v);
    }

    template<CC_IsUniformBufferSupportedType T>
        requires _SSR_STD::negation_v<_SSR_STD::is_same<T, SMAT3X4>>
    auto get_uniform(SCINT offset) const noexcept -> T
    {
        return *reinterpret_cast<const T*>(static_cast<const char*>(mBuffer) + offset);
    }

    template<CC_IsUniformBufferSupportedType T>
        requires _SSR_STD::is_same_v<T, SMAT3X4>
    auto get_uniform(SCINT offset) const noexcept -> SMAT3X4
    {
        const SVEC4* p = reinterpret_cast<const SVEC4*>(static_cast<char const*>(mBuffer) + offset);
        return SMAT3X4{ p[0][0], p[0][1], p[0][2], 0.0,
                        p[1][0], p[1][1], p[1][2], 0.0,
                        p[2][0], p[2][1], p[2][2], 0.0};
    }     
protected:

    auto invalidate() noexcept -> void*
    {
        return invalidate_uniforms(0, mSize);
    }

    auto invalidate_uniforms(SCINT offset, SCINT size) noexcept -> void*
    {
        _SSR_ASSERT(offset + size <= mSize, _SSR_RDERROR);
        mIsDirty = true;
        return static_cast<char*>(mBuffer) + offset;
    }

    auto clean() const noexcept -> void
    {
        mIsDirty = false;
    }       

    //template<SCINT Size>
    //auto set_uniform_array_untyped(SCINT offset, const void* _SSR_RESTRICT begin, SCINT count) noexcept -> void
    //{
    //    constexpr SCINT stride = (Size + 0xFu) & ~0xFu;

    //    SCINT arraySize = stride * count - stride + Size;

    //    void* _SSR_RESTRICT p = invalidate_uniforms(offset, arraySize);
    //    for (SCINT i = 0; i < count; i++)
    //    {
    //        set_uniform_untyped<Size>(p, 0ul, static_cast<const char*>(begin) + i * Size);
    //        p = SF_Add(p, stride);
    //    }
    //}

    template<SCINT Size>
    auto set_uniform_untyped(SCINT offset, const void* _SSR_RESTRICT v) noexcept -> void
    {
        set_uniform_untyped<Size>(invalidate_uniforms(offset, Size), 0ul, v);
    }

    template<SCINT Size>
        requires (Size == 4 || Size == 8 || Size == 12 || Size == 16 || Size == 48 || Size == 64)
    _SSR_ALWAYS_INLINE
    static auto set_uniform_untyped(void* addr, SCINT offset, const void* v) noexcept -> void
    {
        ::memcpy(static_cast<char*>(addr) + offset, v, Size);
    }
private:
    inline auto is_local_storage() const noexcept -> bool
    { 
        return mBuffer == mStorage; 
    }
protected:
    SCHAR        mStorage[SCV_Storage_Size]{};
    void*        mBuffer{ nullptr };
    SUINT32      mSize{ 0 };
    mutable bool mIsDirty{ false };
};

_SSR_END

#endif

