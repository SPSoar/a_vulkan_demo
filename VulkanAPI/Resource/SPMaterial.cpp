#include "SPMaterial.h"

void SPMaterial::addTextureSampler(SPTextureSampler& textureSampler)
{
	textures[textureSampler.mIndex] = textureSampler;
}
