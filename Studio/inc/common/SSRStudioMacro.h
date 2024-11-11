#ifndef SSR_STUDIO_IN_INCLUDE
#error "DO NOT include the header directly, use #include \"SSRStudioHeadIn.h\" instead"
#endif

#pragma once
#ifndef SSR_SSRSTUDIOMACRO_H_
#define SSR_SSRSTUDIOMACRO_H_

#define _SSR_STUDIO_UBO_SET(InterfaceBlock, UniformBuffer, Id, Value)\
    {\
        _SSR_UNUSED_P(Id);\
        auto offset = InterfaceBlock.offset(_SSR_EXPAND_STR(Id));\
        _SSR_ASSERT(offset != -1, _SSR_RDERROR);\
        UniformBuffer->set_uniform(cast<SCINT>(offset), Value);\
    }\
    _SSR_ASSERT(uniformBuffer, _SSR_RDERROR)

#define _SSR_STUDIO_TEXTURE_SET(InterfaceBlock, TextureGroup, Id, Value)\
    {\
        auto offset = InterfaceBlock.offset(_SSR_EXPAND_STR(Id));\
        _SSR_ASSERT(offset != -1, _SSR_RDERROR);\
        TextureGroup->set_texture(cast<SCINT>(offset), Value);\
    }\
    _SSR_ASSERT(Value, _SSR_RDERROR)

#define _SSR_STUDIO_UNIFORM_BINDING_FUNC(Uniform, Slot)\
    {\
        setInfo.mBindings.push_back({});\
        auto& binding = setInfo.mBindings.back();\
        binding = _SSR_CORE::SSRBindingSetItem::Create_UniformBuffer(Slot,\
                    Uniform,\
                    _SSR_CORE::SSRBufferRange(0, Uniform->get_config().mDataSize));\
    }\

#define _SSR_STUDIO_SSBO_BINDING_FUNC(SSBO, Slot)\
    {\
        setInfo.mBindings.push_back({});\
        auto& binding = setInfo.mBindings.back();\
        binding = _SSR_CORE::SSRBindingSetItem::Create_StorageBuffer(Slot,\
                    SSBO,\
                    _SSR_CORE::SSRBufferRange(0, SSBO->get_config().mDataSize));\
    }\

#define _SSR_STUDIO_TEXTURE_BINDING_FUNC(Texture, Slot)\
    {\
        setInfo.mBindings.push_back({});\
        auto& binding = setInfo.mBindings.back();\
        binding = _SSR_CORE::SSRBindingSetItem::Create_CombinedImage(Slot,\
                    Texture);\
    }\

#define _SSR_STUDIO_INPUT_BINDING_FUNC(Texture, Slot)\
    {\
        setInfo.mBindings.push_back({});\
        auto& binding = setInfo.mBindings.back();\
        binding = _SSR_CORE::SSRBindingSetItem::Create_InputImage(Slot,\
                    Texture);\
    }\

#define _SSR_STUDIO_DEBUG_PROCESSE

#define _SSR_SCENE_ENTITY_CONTAINER_SIZE 200

#endif

