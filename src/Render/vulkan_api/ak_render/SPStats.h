#ifndef S_SPSTATS_H_
#define S_SPSTATS_H_

#include "../vulkan_common.h"

class SPRenderContext;
class SPStats
{
public:
	SPStats(SPRenderContext& render_context, uint64_t buffer_size = 16);
	~SPStats();
private:
	SPRenderContext& render_context_;
	uint64_t buffer_size_;
	/// Promise to stop the worker thread
	std::unique_ptr<std::promise<void>> stop_worker_;
	/// Worker thread for continuous sampling
	std::thread worker_thread_;
};

#endif