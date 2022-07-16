#ifndef S_SPSPBRMATERIAL_H_
#define S_SPSPBRMATERIAL_H_

#include "SPComponent.h"

enum class SPSAlphaMode : uint8_t
{
	/// Alpha value is ignored
	Opaque = 0,
	/// Either full opaque or fully transparent
	Mask,
	/// Output is combined with the background
	Blend
};


class SPSTexture;
class SPSPBRMaterial : public SPComponent
{
public:
	SPSPBRMaterial(const std::string& name);
	virtual ~SPSPBRMaterial() = default;
	SPSPBRMaterial(SPSPBRMaterial&& other) = default;
	virtual std::type_index get_type() override;
public:
	std::unordered_map<std::string, SPSTexture*> textures_;
	glm::vec4 base_color_factor_{ 0.0f, 0.0f, 0.0f, 0.0f };
	float metallic_factor_{ 0.0f };
	float roughness_factor_{ 0.0f };

	glm::vec3 emissive_{ 0.0f, 0.0f, 0.0f };
	bool double_sided_{ false };
	float alpha_cutoff_{ 0.5f };
	SPSAlphaMode alpha_mode_{ SPSAlphaMode::Opaque };
};

#endif