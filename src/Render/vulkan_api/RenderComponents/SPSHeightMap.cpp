#include "SPSHeightMap.h"

VKBP_DISABLE_WARNINGS()
#include <ktx.h>
#include <ktxvulkan.h>
VKBP_ENABLE_WARNINGS()

SPSHeightMap::SPSHeightMap(const std::string& filename, const uint32_t patchsize)
{
	ktxTexture* ktx_texture;
	ktxResult   ktx_result;
	ktx_result = ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &ktx_texture);

	assert(ktx_result == KTX_SUCCESS);

	ktx_size_t   ktx_size = ktxTexture_GetImageSize(ktx_texture, 0);
	ktx_uint8_t* ktx_image = ktxTexture_GetData(ktx_texture);

	dim_ = ktx_texture->baseWidth;
	data_ = new uint16_t[dim_ * dim_];

	memcpy(data_, ktx_image, ktx_size);

	this->scale_ = dim_ / patchsize;

	ktxTexture_Destroy(ktx_texture);
}

SPSHeightMap::~SPSHeightMap()
{
	delete[] data_;
}

float SPSHeightMap::get_height(const uint32_t x, const uint32_t y)
{
	glm::ivec2 rpos = glm::ivec2(x, y) * glm::ivec2(scale_);
	rpos.x = std::max(0, std::min(rpos.x, (int)dim_ - 1));
	rpos.y = std::max(0, std::min(rpos.y, (int)dim_ - 1));
	rpos /= glm::ivec2(scale_);
	return *(data_ + (rpos.x + rpos.y * dim_) * scale_) / 65535.0f;
}
