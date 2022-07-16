#ifndef S_SPCOMMANDBUFFER_H_
#define S_SPCOMMANDBUFFER_H_

#include "../vulkan_common.h"

#include "../Pipeline/SPPipelineState.h"
#include "../Resource/SPResourceBindingState.h"

class SPRenderPass;
class SPFramebuffer;
class SPCommandPool;
class SPDevice;
class SPDescriptorSetLayout;
class SPBuffer;
class SPImage;

class SPCommandBuffer
{
public:
	enum class ResetMode
	{
		ResetPool,
		ResetIndividually,
		AlwaysAllocate,
	};

	enum class State
	{
		Invalid,
		Initial,
		Recording,
		Executable,
	};

	struct RenderPassBinding
	{
		const SPRenderPass* render_pass_;
		const SPFramebuffer* framebuffer_;
	};

	SPCommandBuffer(SPCommandPool& command_pool, VkCommandBufferLevel level);
	SPCommandBuffer(const SPCommandBuffer&) = delete;
	SPCommandBuffer(SPCommandBuffer&& other);
	~SPCommandBuffer();
	SPCommandBuffer& operator=(const SPCommandBuffer&) = delete;
	SPCommandBuffer& operator=(SPCommandBuffer&&) = delete;
public:

	SPDevice& get_device();
	const VkCommandBuffer& get_handle() const;
	VkResult begin(VkCommandBufferUsageFlags flags, SPCommandBuffer* primary_cmd_buf = nullptr);
	VkResult begin(VkCommandBufferUsageFlags flags, const SPRenderPass* render_pass, const SPFramebuffer* framebuffer, uint32_t subpass_index);
	VkResult end();
	const RenderPassBinding& get_current_render_pass() const;
	const uint32_t get_current_subpass_index() const;
	bool is_recording() const;
	VkResult reset(ResetMode reset_mode);
public:
	void image_memory_barrier(const SPImageView& image_view, const ImageMemoryBarrier& memory_barrier) const;
	void buffer_memory_barrier(const SPBuffer& buffer, VkDeviceSize offset, VkDeviceSize size, const BufferMemoryBarrier& memory_barrier);

	void copy_buffer_to_image(const SPBuffer& buffer, const SPImage& image, const std::vector<VkBufferImageCopy>& regions);
	void copy_buffer(const SPBuffer& src_buffer, const SPBuffer& dst_buffer, VkDeviceSize size);
protected:

protected:
	SPCommandPool& command_pool_;
	uint32_t max_push_constants_size_;
	const VkCommandBufferLevel level_;
	VkCommandBuffer handle_;

	State state_;
	bool update_after_bind_;

	//未出现再构造里的参数
	RenderPassBinding current_render_pass_;
	SPPipelineState pipeline_state_;
	SPResourceBindingState resource_binding_state_;
	std::unordered_map<uint32_t, SPDescriptorSetLayout*> descriptor_set_layout_binding_state_;
	std::vector<uint8_t> stored_push_constants_;

};

#endif