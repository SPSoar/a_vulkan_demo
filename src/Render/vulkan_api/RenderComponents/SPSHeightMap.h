#ifndef S_SPSHEIGHTMAP_H_
#define S_SPSHEIGHTMAP_H_

#include "SPSTransform.h"

class SPSHeightMap
{
public:
	SPSHeightMap(const std::string& filename, const uint32_t patchsize);
	~SPSHeightMap();
	float get_height(const uint32_t x, const uint32_t y);
private:
	uint16_t* data_;
	uint32_t dim_;
	uint32_t scale_;
};

#endif