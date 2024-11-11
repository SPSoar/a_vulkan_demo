#pragma once
#ifndef SSR_SSRGEOMETRICMODELINGENGINE_H_
#define SSR_SSRGEOMETRICMODELINGENGINE_H_

#include "../../common/SSRStudioHeadIn.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRStudioIn);

class SSRGeometricModelingEngine
{
public:
    SSRGeometricModelingEngine() noexcept  = default;
    ~SSRGeometricModelingEngine() noexcept = default;
    _SSR_CLASS_DELETE_COPY(SSRGeometricModelingEngine);
    _SSR_CLASS_DELETE_MOVE(SSRGeometricModelingEngine);
public:
    auto setup() noexcept -> SSRGeometricModelingEngine&;
    auto setup_geometry(const _SSR_DATA::SSRDGeometry::Geometry* geometry) noexcept -> SSRGeometricModelingEngine&;
    auto generate() noexcept -> void;
public:

protected:
    const _SSR_DATA::SSRDGeometry::Geometry* mGeometry{ nullptr };
};

_SSR_END

#endif

