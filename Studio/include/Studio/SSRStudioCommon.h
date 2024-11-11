#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once

#ifndef SSR_SSRSTUDIOCOMMON_H_
#define SSR_SSRSTUDIOCOMMON_H_

#include <Base/SSRBase.h>
#include <Core/SSRCore.h>

_SSR_STUDIO_BEGIN

// this must be same as the type of SSROnlineRendererIn::Renderer 
enum class E_OnlineRendererMode : SENUM8
{
    eDebugSingleColor = 0,
    eDebugTwoSubpass,
    eForwardRendering,
};


enum class E_ScenePolicy : SENUM8
{
    eSceneGraphStatic = 0,
    eSceneGraphStaticDynamic,
    eSceneGraphStaticDynamicFlash,
    eSceneGraphStaticFlash,
    eSceneGraphDynamic,
    eSceneGraphDynamicFlash,
    eSceneGraphFlash,
    eCount
};
enum class E_PrimitiveCategory : SENUM8
{
    eStatic = 0,
    eDynamic,
    eFlash,
};


enum class E_EntityType : SENUM8
{
    ePoints = 0,
    eWireframe,
    eSurface,
    ePatch,
};

//------------------------------material start
enum class E_Shading : SENUM8 
{
    eNone,
    eSurface,
    eLine,
    eUnlit,
    eLit,
    eSubSurface,
    eCloth,
    eTranslucent,
    ePoint
};
//------------------------------material end

//------------------------------buffer start
/**
* Vertex attribute types
*/
enum E_VertexAttribute : SENUM8 
{

    ePosition    = 0,
    eTangents    = 1,
    eColor       = 2, 
    eUv0         = 3,
    eUv1         = 4,
    eBoneIndices = 5, 
    eBoneWeights = 6,

    eCustom0 = 8,
    eCustom1 = 9,
    eCustom2 = 10,
    eCustom3 = 11,
    eCustom4 = 12,
    eCustom5 = 13,
    eCustom6 = 14,
    eCustom7 = 15,
};


class SSRBufferDescriptor
{
public:
    SSRBufferDescriptor() noexcept  = default;
    ~SSRBufferDescriptor() noexcept = default;

    _SSR_CLASS_DELETE_COPY(SSRBufferDescriptor);
    SSRBufferDescriptor(SSRBufferDescriptor&& rhs) noexcept
        : mBuffer{ _SSR_STD::exchange(rhs.mBuffer, nullptr) }
        , mSize(_SSR_STD::exchange(rhs.mSize, 0))
    {

    }
    SSRBufferDescriptor& operator=(SSRBufferDescriptor&& rhs) noexcept
    {
        if (this != &rhs)
        {
            mBuffer = _SSR_STD::exchange(rhs.mBuffer, nullptr);
            mSize = _SSR_STD::exchange(rhs.mSize, 0);
        }
        return *this;
    }

    SSRBufferDescriptor(const void* buffer, SCINT size) noexcept
        : mBuffer(const_cast<void*>(buffer))
        , mSize{ size }
    {
    }
public:
    void* mBuffer {nullptr};
    SCINT mSize{ 0 };
};

//------------------------------buffer end

_SSR_END

using TempCode_2024 = _SSR_CORE::E_ConfigCode;

#endif

