#ifndef S_SPENGINEBASE_H_
#define S_SPENGINEBASE_H_
#include "common.h"
#include <utility>

class  SPBase
{
public:
	SPBase() {}
	virtual ~SPBase() {}
protected:
public:
	spstr id = "0";
};


template <typename T>
class SPSkillBase {
public:
    template<typename ... ARGS>
    explicit SPSkillBase(ARGS&& ...) noexcept;
    SPSkillBase() noexcept;
    ~SPSkillBase() noexcept;
    SPSkillBase(SPSkillBase const& skill) noexcept;
    SPSkillBase(SPSkillBase&& skill) noexcept : mImpl(skill.mImpl) { skill.mImpl = nullptr; }
    SPSkillBase& operator = (SPSkillBase const& skill) noexcept;
    SPSkillBase& operator = (SPSkillBase&& skill) noexcept; 
protected:
    T* mImpl;
    inline T* operator->() noexcept { return mImpl; }
    inline T const* operator->() const noexcept { return mImpl; }
};


template<typename T>
inline SPSkillBase<T>::SPSkillBase() noexcept
    : mImpl(new T)
{
}

template<typename T>
template<typename ...ARGS>
inline SPSkillBase<T>::SPSkillBase(ARGS && ... args) noexcept
    : mImpl(new T(std::forward<ARGS>(args)...))
{
}

template<typename T>
inline SPSkillBase<T>::~SPSkillBase() noexcept
{
    delete mImpl;
}

template<typename T>
inline SPSkillBase<T>::SPSkillBase(SPSkillBase const& skill) noexcept
    : mImpl(new T(*skill.mImpl))
{
}


template<typename T>
SPSkillBase<T>& SPSkillBase<T>::operator=(SPSkillBase<T> const& skill) noexcept
{
    *mImpl = *skill.mImpl;
    return *this;
}

template<typename T>
SPSkillBase<T>& SPSkillBase<T>::operator =(SPSkillBase<T>&& skill) noexcept
{
    auto temp = mImpl;
    mImpl = skill.mImpl;
    skill.mImpl = temp;
    return *this;
}

#endif
