#include "SPTextureSampler.h"
#include "../Interface/SPStudioManager.h"
#include "SPMaterial.h"
SPTextureSampler& SPTextureSampler::id(const spstr& id) noexcept
{
	this->mId = id;
	return *this;
}

SPTextureSampler& SPTextureSampler::index(const spint& index) noexcept
{
	this->mIndex = index;
	return *this;
}

SPTextureSampler& SPTextureSampler::width(const spint& width) noexcept
{
	this->mWidth = width;
	return *this;
}

SPTextureSampler& SPTextureSampler::height(const spint& height) noexcept
{
	this->mHeight = height;
	return *this;
}

SPTextureSampler& SPTextureSampler::channel(const spint& channel) noexcept
{
	this->mChannel = channel;
	return *this;
}

SPTextureSampler& SPTextureSampler::size(const spint& size) noexcept
{
	this->mSize = size;
	return *this;
}
SPTextureSampler& SPTextureSampler::data(spuchar*& data) noexcept
{
	this->mData = data;
	return *this;
}

SPTextureSampler::Result SPTextureSampler::create(SPStudioManager& manager, SPMaterial& material)
{
	manager.createTextureSampler(*this);
	material.addTextureSampler(*this);
	return Result::Success;
}
