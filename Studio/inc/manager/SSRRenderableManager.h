#pragma once
#ifndef SSR_SSRRENDERABLEMANAGER_H_
#define SSR_SSRRENDERABLEMANAGER_H_

#include "../common/SSRStudioHeadIn.h"
#include "core/SSRVertexBuffer.h"
#include "core/SSRIndexBuffer.h"

_SSR_STUDIO_BEGIN

_SSR_PR_DEF(SSRRenderableManager);

class SSRRenderableManager 
{
    friend class SSRStudioIn;

    _SSR_PLACEMENT_ARRAY_DEF(RenderableManager,
                             SSRVertexBuffer, VertexBuffer,
                             SSRIndexBuffer, IndexBuffer,
                            _SSR_CORE::RE_PrimitiveType, PrimitiveType);
public:
    using Entity = SSRRenderableManagerArray::Seal;
protected:
    SSRRenderableManager() noexcept = default;
    ~SSRRenderableManager()noexcept = default;
public:
    _SSR_CLASS_DELETE_COPY(SSRRenderableManager);
    _SSR_CLASS_DELETE_MOVE(SSRRenderableManager);
public:
    auto make_renderable() noexcept -> Entity;
    auto make_renderables(SUINT32 size) noexcept -> SSRVector<Entity>;

    auto destroy(Entity e) noexcept -> void;

    auto get_primitive_type(Entity e) noexcept -> _SSR_CORE::RE_PrimitiveType&;
    auto get_primitive_type(Entity e) const noexcept -> const _SSR_CORE::RE_PrimitiveType&;

    auto get_index_buffer(Entity e) noexcept -> SSRIndexBuffer&;
    auto get_index_buffer(Entity e) const noexcept -> const SSRIndexBuffer&;

    auto get_vertex_buffer(Entity e) noexcept -> SSRVertexBuffer&;
    auto get_vertex_buffer(Entity e) const noexcept -> const SSRVertexBuffer&;
protected:
    auto create_component(Entity e) noexcept-> void;
    auto destroy_component(Entity e) noexcept-> void;
private:
    SSRRenderableManagerArray mManager{};
};


_SSR_END

#endif

