#ifndef S_SPRENDERFRAME_H_
#define S_SPRENDERFRAME_H_

#include "../vulkan_common.h"

#include "../Command/SPFencePool.h"
#include "../Command/SPSemaphorePool.h"
#include "SPRenderTarget.h"
#include "../Descriptor/SPDescriptorPool.h"
#include "../Descriptor/SPDescriptorSet.h"
#include "../Buffer/SPBufferPool.h"
class SPDevice;

class SPRenderFrame
{
public:

	static constexpr uint32_t BUFFER_POOL_BLOCK_SIZE_ = 256;

	const std::unordered_map<VkBufferUsageFlags, uint32_t> Supported_Usage_Map_ = {
		{VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 1},
		{VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, 2},     
		{VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,  1},
		{VK_BUFFER_USAGE_INDEX_BUFFER_BIT,   1} 
	};

	SPRenderFrame(SPDevice& device, std::unique_ptr<SPRenderTarget>&& render_target, size_t thread_count = 1);

	SPRenderFrame(const SPRenderFrame&) = delete;
	SPRenderFrame(SPRenderFrame&&) = delete;
	SPRenderFrame& operator=(const SPRenderFrame&) = delete;
	SPRenderFrame& operator=(SPRenderFrame&&) = delete;
public:
	SPRenderTarget& get_render_target();
	void update_render_target(std::unique_ptr<SPRenderTarget>&& render_target);
private:
	SPDevice& device_;
	SPFencePool fence_pool_;
	SPSemaphorePool semaphore_pool_;
	std::unique_ptr<SPRenderTarget> swapchain_render_target_;
	uint64_t thread_count_;

	std::map<VkBufferUsageFlags, std::vector<std::pair<SPBufferPool, SPBufferBlock*>>> buffer_pools_;

	std::vector<std::unique_ptr<std::unordered_map<std::size_t, SPDescriptorPool>>> descriptor_pools;
	std::vector<std::unique_ptr<std::unordered_map<std::size_t, SPDescriptorSet>>> descriptor_sets;
};

#endif