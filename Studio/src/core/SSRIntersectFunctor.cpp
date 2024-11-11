//#include "core/SSRIntersectFunctor.h"
//
//_SSR_STUDIO_BEGIN
//namespace 
//{
//    auto clip_by_plane(SVEC3& start, SVEC3& end, const const SVEC4& plane) noexcept -> bool 
//    {
//        double startDis = _SSR_MATH::dot(SVEC4(start.x, start.y, start.z, 1.f), plane);
//        double endDis   = _SSR_MATH::dot(SVEC4(end.x, end.y, end.z, 1.f), plane);
//
//        if (startDis > 0.0 && endDis > 0.0 && startDis != 0 && endDis != 0) 
//        {
//            return false;
//        }
//        else if (startDis > 0.0 && startDis != 0 && endDis <= 0)
//        {
//            end = (-start * SVEC3(endDis) + end * SVEC3(startDis)) / SVEC3(startDis - endDis);
//            return false;
//        }
//        else if (endDis > 0.0 && endDis != 0 && startDis <= 0)
//        {
//            end = (-start * SVEC3(endDis) + end * SVEC3(startDis)) / SVEC3(startDis - endDis);
//            return false;
//        }
//        else 
//        {
//            return true;
//        }
//    }
//
//    enum class ClipResult : SENUM8
//    {
//        eAll,
//        ePart,
//        eNone
//    };
//
//    template<typename PointContainer, typename FaceIndexContainer, typename NormalContainer>
//    class SSRConvexMesh3D 
//    {
//    public:
//        using Point  = typename PointContainer::ValueType;
//        using Plane  = SVEC4;
//        using Normal = typename NormalContainer::ValueType;
//        using Index  = SUINT32;
//
//        struct SPoint 
//        {
//            SPoint(const Point& p, bool isVisible = true) noexcept
//                : mPoint{ p }
//                , mVisible{ isVisible }
//            {
//            
//            }
//            Point mPoint{};
//            bool  mVisible{ true };
//        };
//
//        struct SEdge
//        {
//            SPoint(const SSRFixVector<Index, 2>& pointIndice, const SSRSet<Index>& faceIndices, bool isVisible = true) noexcept
//                : mPointIndices{ pointIndice }
//                , mFaceIndices{ faceIndices }
//                , mVisible{ isVisible }
//            {
//
//            }
//            SSRFixVector<Index, 2> mPointIndices{};
//            SSRSet<Index>          mFaceIndices{};
//            bool                   mVisible{ true };
//        };
//
//        struct SFace
//        {
//            SPoint(const SSRSet<Index>& edgeIndices, const Normal& normal, bool isVisible = true) noexcept
//                : mEdgeIndices{ edgeIndices }
//                , mNormal{ normal }
//                , mVisible{ isVisible }
//            {
//
//            }
//            SSRSet<Index>          mEdgeIndices{};
//            Normal                 mNormal{};
//            bool                   mVisible{ true };
//        };
//
//        SSRConvexMesh3D(const PointContainer& points, const FaceIndexContainer& facesIndices, const PointContainer& points, const NormalContainer& facesNormals) noexcept 
//        {
//        
//        }
//
//    protected:
//        SSRVector<SPoint> mPoints{};
//        SSRVector<SEdge>  mEdges{};
//        SSRVector<SFace>  mFaces{};
//    };
//
//    template<typename PointContainer, typename EdgeIndexContainer, typename NormalContainer, typename FaceIndexContainer>
//    auto cut_convex_by_plane() noexcept -> bool 
//    {
//        
//    }
//}
//
//auto SSRLineIntersectFunctor::box(const SSRBoundingBox& box) const noexcept -> bool
//{
//    return false;
//}
//
//auto SSRLineIntersectFunctor::line(const SVEC3& start, const SVEC3& end) const noexcept -> Result
//{
//    return Result();
//}
//
//auto SSRLineIntersectFunctor::tri(const SVEC3& a, const SVEC3& b, const SVEC3& c) const noexcept -> Result
//{
//    return Result();
//}
//
//auto SSRLineIntersectFunctor::point(const SVEC3& point) const noexcept -> Result
//{
//    return Result();
//}
//
//auto SSRLineIntersectFunctor::clone() const noexcept -> SSRLineIntersectFunctor*
//{
//    return nullptr;
//}
//
//auto SSRLineIntersectFunctor::transform_by(const SMAT4X4& mat) noexcept -> void
//{
//}
//
//auto SSRLineIntersectFunctor::clip_by_plane(const SVEC4& plane) noexcept -> bool
//{
//    return false;
//}
//
//auto SSRLineIntersectFunctor::lerance(const SVEC3& position) const noexcept -> SFLOAT64
//{
//    return SFLOAT64();
//}
//
//SSRBoxIntersectFunctor::SSRBoxIntersectFunctor(SFLOAT64 xMin, SFLOAT64 yMin, SFLOAT64 zMin, SFLOAT64 xMax, SFLOAT64 yMax, SFLOAT64 zMax) noexcept
//{
//}
//
//auto SSRBoxIntersectFunctor::box(const SSRBoundingBox& box) const noexcept -> bool
//{
//    return false;
//}
//
//auto SSRBoxIntersectFunctor::line(const SVEC3& start, const SVEC3& end) const noexcept -> Result
//{
//    return Result();
//}
//
//auto SSRBoxIntersectFunctor::tri(const SVEC3& a, const SVEC3& b, const SVEC3& c) const noexcept -> Result
//{
//    return Result();
//}
//
//auto SSRBoxIntersectFunctor::point(const SVEC3& point) const noexcept -> Result
//{
//    return Result();
//}
//
//auto SSRBoxIntersectFunctor::clone() const noexcept -> SSRBoxIntersectFunctor*
//{
//    return nullptr;
//}
//
//auto SSRBoxIntersectFunctor::transform_by(const SMAT4X4& mat) noexcept -> void
//{
//}
//
//auto SSRBoxIntersectFunctor::clip_by_plane(const SVEC4& plane) noexcept -> bool
//{
//    return false;
//}
//
//
//_SSR_END
