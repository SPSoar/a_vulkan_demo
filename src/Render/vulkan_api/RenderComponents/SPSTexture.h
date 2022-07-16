#ifndef S_SPSTEXTURE_H_
#define S_SPSTEXTURE_H_

#include "SPComponent.h"

class SPSImage;
class SPSSampler;

class SPSTexture : public SPComponent
{
public:
	SPSTexture(const std::string& name);
	SPSTexture(SPSTexture&& other) = default;
	virtual ~SPSTexture() = default;
	virtual std::type_index get_type() override;
public:
	void set_image(SPSImage& image);
	SPSImage* get_image();
	void set_sampler(SPSSampler& sampler);
	SPSSampler* get_sampler();
private:
	SPSImage* image_;
	SPSSampler* sampler_;
};

#endif