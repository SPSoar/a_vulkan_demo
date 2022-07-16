#include "SPRenderFrame.h"


SPRenderFrame::SPRenderFrame(SPDevice& device, std::unique_ptr<SPRenderTarget>&& render_target, size_t thread_count)
	: device_{ device }
    , fence_pool_{ device }
    , semaphore_pool_{ device }
    , swapchain_render_target_{ std::move(render_target) }
    , thread_count_{ thread_count }
{
	for (auto& usage_it : Supported_Usage_Map_)
	{
		std::vector<std::pair<SPBufferPool, SPBufferBlock*>> usage_buffer_pools;
		for (size_t i = 0; i < thread_count; ++i)
		{
			usage_buffer_pools.push_back(std::make_pair(SPBufferPool{ device, BUFFER_POOL_BLOCK_SIZE_ * 1024 * usage_it.second, usage_it.first }, nullptr));
		}

		auto res_ins_it = buffer_pools_.emplace(usage_it.first, std::move(usage_buffer_pools));

		if (!res_ins_it.second)
		{
			throw std::runtime_error("Failed to insert buffer pool");
		}
	}

	for (size_t i = 0; i < thread_count; ++i)
	{
		descriptor_pools.push_back(std::make_unique<std::unordered_map<std::size_t, SPDescriptorPool>>());
		descriptor_sets.push_back(std::make_unique<std::unordered_map<std::size_t, SPDescriptorSet>>());
	}
}

SPRenderTarget& SPRenderFrame::get_render_target()
{
    return *swapchain_render_target_;
}

void SPRenderFrame::update_render_target(std::unique_ptr<SPRenderTarget>&& render_target)
{
    swapchain_render_target_ = std::move(render_target);
}
