#include "SPResourceBindingState.h"

void SPResourceBindingState::reset()
{
	clear_dirty();
	resource_sets_.clear();
}

void SPResourceBindingState::clear_dirty()
{
	dirty_ = false;
}

void SPResourceBindingState::clear_dirty(uint32_t set)
{
	resource_sets_[set].clear_dirty();
}

void SPResourceSet::clear_dirty()
{
	dirty_ = false;
}
