#include "SPPipelineState.h"

void SPPipelineState::reset()
{
	clear_dirty();

	pipeline_layout_ = nullptr;
	render_pass_ = nullptr;
	specialization_constant_state_.reset();
	vertex_input_state_ = {};
	input_assembly_state_ = {};
	rasterization_state_ = {};
	multisample_state_ = {};
	depth_stencil_state_ = {};
	color_blend_state_ = {};
	subpass_index_ = { 0U };
	viewport_state_ = {};
}

uint32_t SPPipelineState::get_subpass_index() const
{
	return subpass_index_;
}

void SPPipelineState::clear_dirty()
{
	dirty_ = false;
	specialization_constant_state_.clear_dirty();
}

void SpecializationConstantState::reset()
{
	if (dirty_)
	{
		specialization_constant_state_.clear();
	}

	dirty_ = false;
}

bool SpecializationConstantState::is_dirty() const
{
	return dirty_;
}

void SpecializationConstantState::clear_dirty()
{
	dirty_ = false;
}

void SpecializationConstantState::set_constant(uint32_t constant_id, const std::vector<uint8_t>& value)
{
	auto data = specialization_constant_state_.find(constant_id);

	if (data != specialization_constant_state_.end() && data->second == value)
	{
		return;
	}

	dirty_ = true;

	specialization_constant_state_[constant_id] = value;
}

void SpecializationConstantState::set_specialization_constant_state(const std::map<uint32_t, std::vector<uint8_t>>& state)
{
	specialization_constant_state_ = state;
}

const std::map<uint32_t, std::vector<uint8_t>>& SpecializationConstantState::get_specialization_constant_state() const
{
	return specialization_constant_state_;
}
