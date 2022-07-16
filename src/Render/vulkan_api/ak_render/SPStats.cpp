#include "SPStats.h"

SPStats::SPStats(SPRenderContext& render_context, uint64_t buffer_size)
	: render_context_(render_context)
	, buffer_size_(buffer_size)
{
	assert(buffer_size >= 2 && "Buffers size should be greater than 2");
}

SPStats::~SPStats()
{
	if (stop_worker_)
	{
		stop_worker_->set_value();
	}

	if (worker_thread_.joinable())
	{
		worker_thread_.join();
	}
}
