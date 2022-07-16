#include "SPSTexture.h"


SPSTexture::SPSTexture(const std::string& name)
	: SPComponent{ name }
	, image_{ nullptr }
	, sampler_{ nullptr }
{
}

std::type_index SPSTexture::get_type()
{
	return typeid(SPSTexture);
}

void SPSTexture::set_image(SPSImage& image)
{
	image_ = &image;
}

SPSImage* SPSTexture::get_image()
{
	return image_;
}

void SPSTexture::set_sampler(SPSSampler& sampler)
{
	sampler_ = &sampler;
}

SPSSampler* SPSTexture::get_sampler()
{
	return sampler_;
}
