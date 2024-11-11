#include "manager/SSRRenderableManager.h"

_SSR_STUDIO_BEGIN

auto SSRRenderableManager::make_renderable() noexcept -> Entity
{
    auto e = mManager.create();
    create_component(e);
    return e;
}

auto SSRRenderableManager::make_renderables(SUINT32 size) noexcept -> SSRVector<Entity>
{
    _SSR_ASSERT(0, _SSR_RDERROR);
    return {};
}

auto SSRRenderableManager::destroy(Entity e) noexcept -> void
{
    _SSR_LIKELY_IF(e)
    {
        destroy_component(e);
        mManager.free(e);
    }
}

auto SSRRenderableManager::get_primitive_type(const Entity e) noexcept -> _SSR_CORE::RE_PrimitiveType&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mManager[e].mPrimitiveType;
}

auto SSRRenderableManager::get_primitive_type(const Entity e) const noexcept -> const _SSR_CORE::RE_PrimitiveType&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mManager[e].mPrimitiveType;
}

auto SSRRenderableManager::get_index_buffer(const Entity e) noexcept -> SSRIndexBuffer&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mManager[e].mIndexBuffer;
}

auto SSRRenderableManager::get_index_buffer(const Entity e) const noexcept -> const SSRIndexBuffer&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mManager[e].mIndexBuffer;
}

auto SSRRenderableManager::get_vertex_buffer(const Entity e) noexcept -> SSRVertexBuffer&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mManager[e].mVertexBuffer;
}

auto SSRRenderableManager::get_vertex_buffer(const Entity e) const noexcept -> const SSRVertexBuffer&
{
    _SSR_ASSERT(e, _SSR_RDERROR);
    return mManager[e].mVertexBuffer;
}

auto SSRRenderableManager::create_component(Entity e) noexcept -> void
{
    _SSR_CORE::RE_PrimitiveType& primitiveType = mManager[e].mPrimitiveType;
    primitiveType = _SSR_CORE::RE_PrimitiveType::eUndefined;

    SSRIndexBuffer& indexBuffer = mManager[e].mIndexBuffer;
    indexBuffer = {};

    SSRVertexBuffer& vertexBuffer = mManager[e].mVertexBuffer;
    vertexBuffer = {};
}

auto SSRRenderableManager::destroy_component(Entity e) noexcept -> void
{
    //_SSR_CORE::RE_PrimitiveType& primitiveType = mManager[e].mPrimitiveType;
    //primitiveType = _SSR_CORE::RE_PrimitiveType::eUndefined;

    //SSRIndexBuffer& indexBuffer = mManager[e].mIndexBuffer;
    //indexBuffer = {};

    //SSRVertexBuffer& vertexBuffer = mManager[e].mVertexBuffer;
    //vertexBuffer = {};
}

_SSR_END