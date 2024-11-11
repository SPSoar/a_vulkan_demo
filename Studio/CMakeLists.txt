set(_SSR_LIB_LINKING SHARED)

_SSR_CHECK_LIB_LINKING()

if(_SSR_RESULT_LIB_LINK_DYNAMIC)
    #add_definitions(-DDEBUG)
else()
    message(FATAL_ERROR "not support dynamic lib")
endif()

set(_SSR_LIB_NAME Studio)

set(_SSR_LIB_FOLDER "SSR")

set(__SSR_COMMON_INC_PATH inc/common)
set(__SSR_COMMON_INC
    ${__SSR_COMMON_INC_PATH}/SSRStudioHeadIn.h
    ${__SSR_COMMON_INC_PATH}/SSRStudioMacro.h
    ${__SSR_COMMON_INC_PATH}/SSRStudioCommonIn.h
)

set(__SSR_BACKEND_SRC_PATH src/common)
set(__SSR_BACKEND_SRC
)

set(__SSR_CORE_INC_PATH inc/core)
set(__SSR_CORE_INC
    ${__SSR_CORE_INC_PATH}/SSRIndexBuffer.h
    ${__SSR_CORE_INC_PATH}/SSRInstanceBuffer.h
    ${__SSR_CORE_INC_PATH}/SSRIntersectFunctor.h
    ${__SSR_CORE_INC_PATH}/SSRLightUniform.h
    ${__SSR_CORE_INC_PATH}/SSRPerMaterialUniform.h
    ${__SSR_CORE_INC_PATH}/SSRPerRenderableSSbo.h
    ${__SSR_CORE_INC_PATH}/SSRPerViewUniform.h
    ${__SSR_CORE_INC_PATH}/SSRSamplerInterfaceBlock.h
    ${__SSR_CORE_INC_PATH}/SSRShadowUniform.h
    ${__SSR_CORE_INC_PATH}/SSRSkinningBuffer.h
    ${__SSR_CORE_INC_PATH}/SSRTexture.h
    ${__SSR_CORE_INC_PATH}/SSRTextureGroup.h
    ${__SSR_CORE_INC_PATH}/SSRUniformBuffer.h
    ${__SSR_CORE_INC_PATH}/SSRUniformCpuBuffer.h
    ${__SSR_CORE_INC_PATH}/SSRUniformInterfaceBlock.h
    ${__SSR_CORE_INC_PATH}/SSRVertexBuffer.h
)

set(__SSR_CORE_SRC_PATH src/core)
set(__SSR_CORE_SRC
    ${__SSR_CORE_SRC_PATH}/SSRIndexBuffer.cpp
    ${__SSR_CORE_SRC_PATH}/SSRInstanceBuffer.cpp
    ${__SSR_CORE_SRC_PATH}/SSRIntersectFunctor.cpp
    ${__SSR_CORE_SRC_PATH}/SSRLightUniform.cpp
    ${__SSR_CORE_SRC_PATH}/SSRPerMaterialUniform.cpp
    ${__SSR_CORE_SRC_PATH}/SSRPerRenderableSSbo.cpp
    ${__SSR_CORE_SRC_PATH}/SSRPerViewUniform.cpp
    ${__SSR_CORE_SRC_PATH}/SSRSamplerInterfaceBlock.cpp
    ${__SSR_CORE_SRC_PATH}/SSRShadowUniform.cpp
    ${__SSR_CORE_SRC_PATH}/SSRSkinningBuffer.cpp
    ${__SSR_CORE_SRC_PATH}/SSRTexture.cpp
    ${__SSR_CORE_SRC_PATH}/SSRTextureGroup.cpp
    ${__SSR_CORE_SRC_PATH}/SSRUniformBuffer.cpp
    ${__SSR_CORE_SRC_PATH}/SSRUniformCpuBuffer.cpp
    ${__SSR_CORE_SRC_PATH}/SSRUniformInterfaceBlock.cpp
    ${__SSR_CORE_SRC_PATH}/SSRVertexBuffer.cpp
)

set(__SSR_CORE_INL_PATH src/core/inl)
set(__SSR_CORE_INL

)

set(__SSR_SCENE_INC_PATH inc/scene)
set(__SSR_SCENE_INC
    ${__SSR_SCENE_INC_PATH}/SSRRenderBatch.h
    ${__SSR_SCENE_INC_PATH}/SSRRenderBlock.h
    ${__SSR_SCENE_INC_PATH}/SSRSceneGraph.h
    ${__SSR_SCENE_INC_PATH}/SSRSceneGraphNode.h
    ${__SSR_SCENE_INC_PATH}/SSRSceneGraphRoot.h
    ${__SSR_SCENE_INC_PATH}/SSRSceneOcTree.h
)

set(__SSR_SCENE_SRC_PATH src/scene)
set(__SSR_SCENE_SRC
    ${__SSR_SCENE_SRC_PATH}/SSRRenderBlock.cpp
    ${__SSR_SCENE_SRC_PATH}/SSRSceneGraphNode.cpp
    ${__SSR_SCENE_SRC_PATH}/SSRSceneGraphRootDynamic.cpp
    ${__SSR_SCENE_SRC_PATH}/SSRSceneGraphRootFlash.cpp
    ${__SSR_SCENE_SRC_PATH}/SSRSceneGraphRootStatic.cpp
    ${__SSR_SCENE_SRC_PATH}/SSRSceneOcTree.cpp

)

set(__SSR_SCENE_INC_INL_PATH inc/scene/inl)
set(__SSR_SCENE_INC_INL
    ${__SSR_SCENE_INC_INL_PATH}/SSSRRenderBatchToolbox.inl
)

set(__SSR_SCENE_SRC_INL_PATH src/scene/inl)
set(__SSR_SCENE_SRC_INL
    ${__SSR_SCENE_SRC_INL_PATH}/SSRSceneGraphNodeToolbox.inl
)

set(__SSR_RENDERER_INC_PATH inc/renderer)
set(__SSR_RENDERER_INC
    ${__SSR_RENDERER_INC_PATH}/SSRRenderer.h
    ${__SSR_RENDERER_INC_PATH}/SSRRenderPass.h
    ${__SSR_RENDERER_INC_PATH}/SSRRenderPipeline.h
)

set(__SSR_RENDERER_INC_INL_PATH inc/renderer/inl)
set(__SSR_RENDERER_INC_INL
    ${__SSR_RENDERER_INC_INL_PATH}/SSRRendererToolbox.inl
)

set(__SSR_RENDERER_SRC_PATH src/renderer)
set(__SSR_RENDERER_SRC
    ${__SSR_RENDERER_SRC_PATH}/SSRRenderPassDebugSingleColor.cpp
    ${__SSR_RENDERER_SRC_PATH}/SSRRenderPassDebugSubpassColor.cpp
    ${__SSR_RENDERER_SRC_PATH}/SSRRenderPassDebugSubpassMix.cpp
    ${__SSR_RENDERER_SRC_PATH}/SSRRenderPassDepthOnly.cpp
    ${__SSR_RENDERER_SRC_PATH}/SSRRenderPassFxaa.cpp
    ${__SSR_RENDERER_SRC_PATH}/SSRRenderPassOitCollect.cpp
    ${__SSR_RENDERER_SRC_PATH}/SSRRenderPassOitColor.cpp
    ${__SSR_RENDERER_SRC_PATH}/SSRRenderPassOpaque.cpp
)

set(__SSR_RENDERER_SRC_INL_PATH src/renderer/inl)
set(__SSR_RENDERER_SRC_INL
    ${__SSR_RENDERER_SRC_INL_PATH}/SSRRenderPassDebugSingleColorToolbox.inl
    ${__SSR_RENDERER_SRC_INL_PATH}/SSRRenderPassDebugSubpassColorToolbox.inl
    ${__SSR_RENDERER_SRC_INL_PATH}/SSRRenderPassDebugSubpassMixToolbox.inl
    ${__SSR_RENDERER_SRC_INL_PATH}/SSRRenderPassDepthOnlyToolbox.inl
    ${__SSR_RENDERER_SRC_INL_PATH}/SSRRenderPassFxaaToolbox.inl
    ${__SSR_RENDERER_SRC_INL_PATH}/SSRRenderPassOitCollectToolbox.inl
    ${__SSR_RENDERER_SRC_INL_PATH}/SSRRenderPassOitColorToolbox.inl
    ${__SSR_RENDERER_SRC_INL_PATH}/SSRRenderPassOpaqueToolbox.inl
)

set(__SSR_STUDIO_INC_PATH inc/studio)
set(__SSR_STUDIO_INC
    ${__SSR_STUDIO_INC_PATH}/SSRCameraIn.h
    ${__SSR_STUDIO_INC_PATH}/SSRLightIn.h
    ${__SSR_STUDIO_INC_PATH}/SSRManipulatorIn.h
    ${__SSR_STUDIO_INC_PATH}/SSRMaterialIn.h
    ${__SSR_STUDIO_INC_PATH}/SSRMaterialInstanceIn.h
    ${__SSR_STUDIO_INC_PATH}/SSROfflineRendererIn.h
    ${__SSR_STUDIO_INC_PATH}/SSROnlineRendererIn.h
    ${__SSR_STUDIO_INC_PATH}/SSRSceneIn.h
    ${__SSR_STUDIO_INC_PATH}/SSRSkyboxIn.h
    ${__SSR_STUDIO_INC_PATH}/SSRStudioIn.h
    ${__SSR_STUDIO_INC_PATH}/SSRViewIn.h
)

set(__SSR_STUDIO_IN_SRC_PATH src/studio)
set(__SSR_STUDIO_IN_SRC
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRCameraIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRLightIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRManipulatorIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRMaterialIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRMaterialInstanceIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSROfflineRendererIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSROnlineRendererIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRSceneIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRSkyboxIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRStudioIn.cpp
    ${__SSR_STUDIO_IN_SRC_PATH}/SSRViewIn.cpp
)

set(__SSR_STUDIO_IN_INL_PATH src/studio/inl)
set(__SSR_STUDIO_IN_INL
    ${__SSR_STUDIO_IN_INL_PATH}/SSRViewInToolbox.inl
)

set(__SSR_MANAGER_INC_PATH inc/manager)
set(__SSR_MANAGER_INC
    ${__SSR_MANAGER_INC_PATH}/SSRBufferTracker.h
    ${__SSR_MANAGER_INC_PATH}/SSRCameraManagerIn.h
    ${__SSR_MANAGER_INC_PATH}/SSRLightManagerIn.h
    ${__SSR_MANAGER_INC_PATH}/SSRPrimitiveManagerIn.h
    ${__SSR_MANAGER_INC_PATH}/SSRRenderableManager.h
    ${__SSR_MANAGER_INC_PATH}/SSRRenderManager.h
    ${__SSR_MANAGER_INC_PATH}/SSRRenderTracker.h
    ${__SSR_MANAGER_INC_PATH}/SSRTextureManager.h
    ${__SSR_MANAGER_INC_PATH}/SSRTransformManagerIn.h
)

set(__SSR_MANAGER_SRC_PATH src/manager)
set(__SSR_MANAGER_SRC
    ${__SSR_MANAGER_SRC_PATH}/SSRBufferTracker.cpp
    ${__SSR_MANAGER_SRC_PATH}/SSRCameraManagerIn.cpp
    ${__SSR_MANAGER_SRC_PATH}/SSRLightManagerIn.cpp
    ${__SSR_MANAGER_SRC_PATH}/SSRPrimitiveManagerIn.cpp
    ${__SSR_MANAGER_SRC_PATH}/SSRRenderableManager.cpp
    ${__SSR_MANAGER_SRC_PATH}/SSRRenderManager.cpp
    ${__SSR_MANAGER_SRC_PATH}/SSRRenderTracker.cpp
    ${__SSR_MANAGER_SRC_PATH}/SSRTextureManager.cpp
    ${__SSR_MANAGER_SRC_PATH}/SSRTransformManagerIn.cpp
)


set(__SSR_GEOMETRY_CONSTRAINT_INC_PATH inc/geometry/constraint)
set(__SSR_GEOMETRY_CONSTRAINT_INC
    ${__SSR_GEOMETRY_CONSTRAINT_INC_PATH}/SSRGeometricConstraintEngine.h
)

set(__SSR_GEOMETRY_CONSTRAINT_SRC_PATH src/geometry/constraint)
set(__SSR_GEOMETRY_CONSTRAINT_SRC
    ${__SSR_GEOMETRY_CONSTRAINT_SRC_PATH}/SSRGeometricConstraintEngine.cpp
)

set(__SSR_GEOMETRY_MODELING_INC_PATH inc/geometry/modeling)
set(__SSR_GEOMETRY_MODELING_INC
    ${__SSR_GEOMETRY_MODELING_INC_PATH}/SSRGeometricModelingEngine.h
)

set(__SSR_GEOMETRY_MODELING_SRC_PATH src/geometry/modeling)
set(__SSR_GEOMETRY_MODELING_SRC
    ${__SSR_GEOMETRY_MODELING_SRC_PATH}/SSRGeometricModelingEngine.cpp
)


set(__SSR_STUDIO_INCLUDE 
    include/Studio/SSRCamera.h
    include/Studio/SSRCameraManager.h
    include/Studio/SSRLight.h
    include/Studio/SSRLightManager.h
    include/Studio/SSRManipulator.h
    include/Studio/SSRMaterial.h
    include/Studio/SSRMaterialInstance.h
    include/Studio/SSROfflineRenderer.h
    include/Studio/SSROnlineRenderer.h
    include/Studio/SSRPrimitiveManager.h
    include/Studio/SSRScene.h
    include/Studio/SSRSkybox.h
    include/Studio/SSRStudio.h
    include/Studio/SSRStudioCommon.h
    include/Studio/SSRStudioHead.h
    include/Studio/SSRTransformManager.h
    include/Studio/SSRView.h
    include/Studio/SSRViewport.h
   
)

set(__SSR_STUDIO_SRC
   
)

#if(_SSR_WINDOWS)
#  set(__SSR_PLATFORM_INC )
#  set(__SSR_PLATFORM_SRC )
#endif()

set(__SSR_SELF_INCLUDE_ALL 
    ${__SSR_STUDIO_INCLUDE}
)

set(_SSR_LIB_INCLUDE ${__SSR_SELF_INCLUDE_ALL})
set(_SSR_LIB_INC 
    ${__SSR_COMMON_INC}
    ${__SSR_CORE_INC}
    ${__SSR_SCENE_INC_INL}
    ${__SSR_SCENE_INC}
    ${__SSR_RENDERER_INC}
    ${__SSR_RENDERER_INC_INL}
    ${__SSR_STUDIO_INC}
    ${__SSR_MANAGER_INC}
    ${__SSR_GEOMETRY_CONSTRAINT_INC}
    ${__SSR_GEOMETRY_MODELING_INC}
)
set(_SSR_LIB_SRC 
    ${__SSR_COMMON_SRC}
    ${__SSR_CORE_SRC}
    ${__SSR_CORE_INL}
    ${__SSR_SCENE_SRC}
    ${__SSR_SCENE_SRC_INL}
    ${__SSR_RENDERER_SRC}
    ${__SSR_RENDERER_SRC_INL}
    ${__SSR_STUDIO_IN_SRC}
    ${__SSR_STUDIO_SRC}
    ${__SSR_STUDIO_IN_INL}
    ${__SSR_MANAGER_SRC}
    ${__SSR_GEOMETRY_CONSTRAINT_SRC}
    ${__SSR_GEOMETRY_MODELING_SRC}
)

if(_SSR_WINDOWS)
  set(__SSR_PLATFORM_COMPILE_OPTIONS )
  set(__SSR_PLATFORM_COMPILE_OPTIONS_DEBUG )
  set(__SSR_PLATFORM_COMPILE_OPTIONS_RELEASE )
  set(__SSR_PLATFORM_LINK_FLAGS 
    #/SUBSYSTEM:WINDOWS
  )
  set(__SSR_PLATFORM_LINK_FLAGS_DEBUG )
  set(__SSR_PLATFORM_LINK_FLAGS_RELEASE )
elseif(_SSR_UNIX)
  set(__SSR_PLATFORM_COMPILE_OPTIONS )
  set(__SSR_PLATFORM_COMPILE_OPTIONS_DEBUG )
  set(__SSR_PLATFORM_COMPILE_OPTIONS_RELEASE )
  set(__SSR_PLATFORM_LINK_FLAGS )
  set(__SSR_PLATFORM_LINK_FLAGS_DEBUG )
  set(__SSR_PLATFORM_LINK_FLAGS_RELEASE )
else()
  message(FATAL_ERROR "not support platform")
endif()

set(_SSR_LIB_GENERAL_COMPILE_OPTIONS ${__SSR_PLATFORM_COMPILE_OPTIONS})
set(_SSR_LIB_GENERAL_COMPILE_OPTIONS_DEBUG ${__SSR_PLATFORM_COMPILE_OPTIONS_DEBUG})
set(_SSR_LIB_GENERAL_COMPILE_OPTIONS_RELEASE ${__SSR_PLATFORM_COMPILE_OPTIONS_RELEASE})

set(_SSR_LIB_GENERAL_LINK_FLAGS ${__SSR_PLATFORM_LINK_FLAGS})
set(_SSR_LIB_GENERAL_LINK_FLAGS_DEBUG ${__SSR_PLATFORM_LINK_FLAGS_DEBUG})
set(_SSR_LIB_GENERAL_LINK_FLAGS_RELEASE ${__SSR_PLATFORM_LINK_FLAGS_RELEASE})

set(_SSR_LIB_INCLUDE_DIRECTORIES_PRIVATE 
  include/
  inc/
  ${_SSR_ROOT_PATH}/Base/include
  ${_SSR_ROOT_PATH}/Core/include
  ${_SSR_ROOT_PATH}/Geometry/include
  ${_SSR_ROOT_PATH}/Data/include
  ${_SSR_ROOT_PATH}/Shaders/include
  ${_SSR_DSF_ROOT}/include/
)
set(_SSR_LIB_LINK_LIBRARIES_PRIVATE

)
set(_SSR_LIB_LINK_LIBRARIES_PRIVATE_DEBUG 
)
set(_SSR_LIB_LINK_LIBRARIES_PRIVATE_RELEASE 
)

set(_SSR_LIB_USE_MODULES_PRIVATE 
  Base
  Core
  Data
  Shaders
)
# 编译依赖顺序
set(_SSR_LIB_DEPENDENCIES)

set(_SSR_LIB_INSTALL ON)

set(_SSR_LIB_INSTALL_HEADERS )

_SSR_SETUP_LIB()

if(_SSR_WINDOWS)
    source_group(inc\\common
        FILES 
        ${__SSR_COMMON_INC}
    )
    source_group(src\\common
        FILES 
        ${__SSR_COMMON_SRC}
    )
    source_group(inc\\core
        FILES 
        ${__SSR_CORE_INC}
    )
    source_group(src\\core
        FILES 
        ${__SSR_CORE_SRC}
    )
    source_group(src\\core\\inl
        FILES 
        ${__SSR_CORE_INL}
    )
    source_group(inc\\scene
        FILES 
        ${__SSR_SCENE_INC}
    )
    source_group(inc\\scene\\inl
        FILES 
        ${__SSR_SCENE_INC_INL}
    )    
    source_group(src\\scene
        FILES 
        ${__SSR_SCENE_SRC}
    )        
    source_group(src\\scene\\inl
        FILES 
        ${__SSR_SCENE_SRC_INL}
    )    
    source_group(inc\\renderer
        FILES 
        ${__SSR_RENDERER_INC}
    )
    source_group(inc\\renderer\\inl
        FILES 
        ${__SSR_RENDERER_INC_INL}
    )   
    source_group(src\\renderer
        FILES 
        ${__SSR_RENDERER_SRC}
    )        
    source_group(src\\renderer\\inl
        FILES 
        ${__SSR_RENDERER_SRC_INL}
    )    
    source_group(inc\\studio
        FILES 
        ${__SSR_STUDIO_INC}
    )
    source_group(src\\studio
        FILES 
        ${__SSR_STUDIO_IN_SRC}
    )
    source_group(src\\studio\\inl
        FILES 
        ${__SSR_STUDIO_IN_INL}
    )    
    source_group(inc\\manager
        FILES 
        ${__SSR_MANAGER_INC}
    )
    source_group(src\\manager
        FILES 
        ${__SSR_MANAGER_SRC}
    )
    source_group(inc\\geometry\\constraint
        FILES 
        ${__SSR_GEOMETRY_CONSTRAINT_INC}
    )
    source_group(src\\geometry\\constraint
        FILES 
        ${__SSR_GEOMETRY_CONSTRAINT_SRC}
    )
    source_group(inc\\geometry\\modeling
        FILES 
        ${__SSR_GEOMETRY_MODELING_INC}
    )
    source_group(src\\geometry\\modeling
        FILES 
        ${__SSR_GEOMETRY_MODELING_SRC}
    )
    source_group(include
        FILES 
        ${__SSR_STUDIO_INCLUDE}
    )
    source_group(src
        FILES 
        ${__SSR_STUDIO_SRC}
    )
endif()
