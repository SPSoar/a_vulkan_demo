#ifndef SSR_STUDIO_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHead.h\" instead"
#endif

#pragma once
#ifndef SSR_STUDIO_H_
#define SSR_STUDIO_H_

//
//_SSR_BASE_BEGIN
//    class Entity;
//    class EntityManager;
//    class JobSystem;
//_SSR_END
//
_SSR_STUDIO_BEGIN

_SSR_UNIQUE_DEF_BASE(SSRStudio, _SSR_STUDIO_EXPORT);

#define _SSR_STUDIO_PUB_MEMBER_FUNC_DEF()\
    auto get_primitive_manager() noexcept -> SSRPrimitiveManager&;\
    auto get_primitive_manager() const noexcept -> const SSRPrimitiveManager&;\
    auto open() noexcept -> void;\
    auto close() noexcept -> void;\
    auto execute() noexcept -> void;\
    auto get_token() noexcept -> _SSR_CORE::SSRTokenRef;\
    auto get_transfer_command(SUINT32 index) noexcept -> _SSR_CORE::SSRTransferCommandRef;\
    auto get_primary_draw_command(SUINT32 index) noexcept -> _SSR_CORE::SSRDrawCommandRef;\
    auto make_primary_draw_commands(SUINT32 count) noexcept -> void;\
    auto create_scene(const E_ScenePolicy scenePolicy = E_ScenePolicy::eSceneGraphStatic) noexcept -> SSRScenePtr;\
    auto destroy_scene(SSRScenePtr& scene) noexcept -> void;\

#define _SSR_STUDIO_PUB_MEMBER_FUNC_IMPL(Class)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRPrimitiveManager&, get_primitive_manager,,)\
    _SSR_CLASS_BASE_FUNC_OUT_CONST_NOEXCEPT(Class, const SSRPrimitiveManager&, get_primitive_manager,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, open,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, close,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, execute,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, _SSR_CORE::SSRTokenRef, get_token,,)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, _SSR_CORE::SSRTransferCommandRef, get_transfer_command, SUINT32, index)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, _SSR_CORE::SSRDrawCommandRef, get_primary_draw_command, SUINT32, index)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, make_primary_draw_commands, SUINT32, count)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, SSRScenePtr, create_scene, const E_ScenePolicy, scenePolicy)\
    _SSR_CLASS_BASE_FUNC_OUT_NOEXCEPT(Class, void, destroy_scene, SSRScenePtr&, scene)\

class _SSR_STUDIO_EXPORT SSRStudio
{

public:
    enum class E_StudioType : SENUM8
    {
        eUnknown = 0,
        eGui = 1,
        eRaster = 2,
        eRayTrace = 3,
        eCompute = 4
    };
    struct Config 
    {
        constexpr Config(E_StudioType type) 
            : mStudioType{ type }
        {
            
        }
        SUINT32       mMinCommandBufferSizeMB{ 1 };
        SUINT32       mCommandBufferSizeMB{ mMinCommandBufferSizeMB * 3 };
        SUINT32       mPerRenderPassArenaSizeMB{ 3 };
        SUINT32       mTokenHandleArenaSizeMB{ 0 };
        SUINT32       mPerFrameCommandsSizeMB{ 2 };
        E_StudioType  mStudioType{ E_StudioType::eUnknown };
    };

protected:
    SSRStudio() noexcept = default;
    ~SSRStudio()         = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRStudio);
    _SSR_CLASS_DELETE_MOVE(SSRStudio);
    _SSR_STUDIO_PUB_MEMBER_FUNC_DEF()

public:
    static auto Create(_SSR_CORE::SSRToken& token, const Config* config  = nullptr) -> SSRStudioUPtr;
};

_SSR_END

#endif

