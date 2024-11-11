//#pragma once
//#ifndef SSR_SSRINTERSECTFUNCTOR_H_
//#define SSR_SSRINTERSECTFUNCTOR_H_
//
//#include "../common/SSRStudioHeadIn.h"
//
//_SSR_STUDIO_BEGIN
//
//struct SSRIntersectResult
//{
//    struct Result
//    {
//        bool operator <(const Result& r) const noexcept
//        {
//            return true;
//        }
//
//    };
//    static Result better(const Result& l, const Result& r)
//    {
//        return l < r ? l : r;
//    }
//};
//
//struct SSRBoxIntersectResult
//{
//    using IntersectRes = _SSR::E_IntersectMath;
//    struct Result
//    {
//        IntersectRes res;
//        bool operator <(const Result& r) const noexcept
//        {
//            return true;
//        }
//
//    };
//    static Result better(const Result& l, const Result& r)
//    {
//        return l < r ? l : r;
//    }
//};
//
//
//class SSRLineIntersectFunctor 
//{
//public:
//    using IntersectResult = SSRIntersectResult;
//    using Result = SSRIntersectResult::Result;
//
//    SSRLineIntersectFunctor() = default;
//    SSRLineIntersectFunctor(const SVEC3& from, const SVEC3& to, const SVEC3& fromMargin, const SVEC3& toMargin) noexcept
//        : mFrom{ from }
//        , mTo{ to }
//        , mFromMargin{ fromMargin }
//        , mToMargin{ toMargin }
//    {
//    
//    }
//    ~SSRLineIntersectFunctor() = default;
//
//    auto box(const SSRBoundingBox& box) const noexcept -> bool;
//    auto line(const SVEC3& start, const SVEC3& end) const noexcept -> Result;
//    auto tri(const SVEC3& a, const SVEC3& b, const SVEC3& c) const noexcept -> Result;
//    auto point(const SVEC3& point) const noexcept -> Result;
//
//    auto clone() const noexcept -> SSRLineIntersectFunctor*;
//
//    auto transform_by(const SMAT4X4& mat) noexcept -> void;
//    auto clip_by_plane(const SVEC4& plane) noexcept -> bool;
//private:
//    auto lerance(const SVEC3& position) const noexcept -> SFLOAT64;
//
//protected:
//    SVEC3 mFrom{};
//    SVEC3 mTo{};
//    SVEC3 mFromMargin{};
//    SVEC3 mToMargin{};
//};
//
//class SSRBoxIntersectFunctor
//{
//public:
//    using IntersectResult = SSRBoxIntersectResult;
//    using ResultMode = SSRBoxIntersectResult::IntersectRes;
//    using Result     = SSRBoxIntersectResult::Result;
//    using Index      = SUINT32;
//
//    SSRBoxIntersectFunctor() = default;
//    SSRBoxIntersectFunctor(SFLOAT64 xMin, SFLOAT64 yMin, SFLOAT64 zMin, SFLOAT64 xMax, SFLOAT64 yMax, SFLOAT64 zMax) noexcept;
//    ~SSRBoxIntersectFunctor() = default;
//
//    auto box(const SSRBoundingBox& box) const noexcept -> bool;
//    auto line(const SVEC3& start, const SVEC3& end) const noexcept -> Result;
//    auto tri(const SVEC3& a, const SVEC3& b, const SVEC3& c) const noexcept -> Result;
//    auto point(const SVEC3& point) const noexcept -> Result;
//
//    auto clone() const noexcept -> SSRBoxIntersectFunctor*;
//
//    auto transform_by(const SMAT4X4& mat) noexcept -> void;
//    auto clip_by_plane(const SVEC4& plane) noexcept -> bool;
//
//protected:
//    SSRVector<SVEC3>                   mPoints{};
//    SSRVector <SSRFixVector<Index, 2>> mEdgesPointIndices{};
//    SSRVector<SVEC3>                   mNormal{};
//    SSRVector<SSRVector<Index>>        mFacesPointIndices{};
//};
//
//
//
//
//_SSR_END
//
//#endif
//
