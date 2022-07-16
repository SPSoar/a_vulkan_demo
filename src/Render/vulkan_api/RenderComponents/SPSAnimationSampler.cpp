#include "SPSAnimationSampler.h"

SPSAnimation::SPSAnimation(const std::string& name)
	: SPSScript{ name }
{
}

SPSAnimation::SPSAnimation(const SPSAnimation& other)
	: channels_{ other.channels_ }
{
}

void SPSAnimation::update_times(float new_start_time, float new_end_time)
{
	if (new_start_time < start_time_)
	{
		start_time_ = new_start_time;
	}
	if (new_end_time > end_time_)
	{
		end_time_ = new_end_time;
	}
}

void SPSAnimation::add_channel(SPSNode& node, const SPSAnimationTarget& target, const SPSAnimationSampler& sampler)
{
	channels_.push_back({ node, target, sampler });
}
