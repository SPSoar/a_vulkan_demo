#ifndef S_SPSANIMATIONSAMPLER_H_
#define S_SPSANIMATIONSAMPLER_H_

#include "SPSScript.h"

enum class SPSAnimationType : uint8_t
{
	Linear = 0,
	Step,
	CubicSpline
};

enum class SPSAnimationTarget : uint8_t
{
	Translation = 0,
	Rotation,
	Scale
};

struct SPSAnimationSampler
{
	SPSAnimationType type_{ SPSAnimationType::Linear };

	std::vector<float> inputs_{};

	std::vector<glm::vec4> outputs_{};
};

class SPSNode;
struct SPSAnimationChannel
{
	SPSNode& node_;

	SPSAnimationTarget target_;

	SPSAnimationSampler sampler_;
};

class SPSAnimation : public SPSScript
{
public:
	SPSAnimation(const std::string& name = "");
	SPSAnimation(const SPSAnimation&);
public:
	void update_times(float start_time, float end_time);
	void add_channel(SPSNode& node, const SPSAnimationTarget& target, const SPSAnimationSampler& sampler);
private:
	std::vector<SPSAnimationChannel> channels_;
	float current_time_{ 0.0f };
	float start_time_{ std::numeric_limits<float>::max() };
	float end_time_{ std::numeric_limits<float>::min() };
};

#endif