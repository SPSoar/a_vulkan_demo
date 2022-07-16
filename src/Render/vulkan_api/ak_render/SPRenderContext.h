#ifndef S_SPRENDERCONTEXT_H_
#define S_SPRENDERCONTEXT_H_

#include "../vulkan_common.h"
#include "../RenderContext/SPRenderTarget.h"
#include "../RenderContext/SPRenderFrame.h"


enum class Vsync
{
	OFF,
	ON,
	Default
};

struct SPSwapchainBuffer
{
	VkImage     image_;
	VkImageView view_;
};

class SPEngine;
class SPQueue;
class SPSwapchain;
class SPRenderWorker;

class SPRenderContext
{
public:
	static VkFormat DEFAULT_VK_FORMAT;

	SPRenderContext(SPEngine& engine, uint32_t window_width, uint32_t window_height);

	SPRenderContext(const SPRenderContext&) = delete;
	SPRenderContext(SPRenderContext&&) = delete;
	virtual ~SPRenderContext() = default;
	SPRenderContext& operator=(const SPRenderContext&) = delete;
	SPRenderContext& operator=(SPRenderContext&&) = delete;

	void init();
public:
	void set_present_mode_priority(const std::vector<VkPresentModeKHR>& present_mode_priority_list);
	void set_surface_format_priority(const std::vector<VkSurfaceFormatKHR>& surface_format_priority_list);
	void request_image_format(const VkFormat format);
	void prepare(size_t thread_count = 1, SPRenderTarget::CreateFunc create_render_size_ttarget_func = SPRenderTarget::DEFAULT_CREATE_FUNC_);
	void prepare_queue();
	void create_swapchain_buffers();
	bool has_swapchain();
	SPSwapchain& get_swapchain();
	std::vector<std::unique_ptr<SPRenderFrame>>& get_render_frames();
	VkExtent2D const& get_surface_extent() const;
	VkFormat get_format() const;
	SPDevice& get_device();
	bool handle_surface_changes(SPRenderWorker& render_worker, bool force_update);
	void update_swapchain(const VkExtent2D& extent, const VkSurfaceTransformFlagBitsKHR transform);
	void update_swapchain(const std::set<VkImageUsageFlagBits>& image_usage_flags);
	void recreate();
	void request_present_mode(const VkPresentModeKHR present_mode);
public:
	void draw(SPRenderWorker& render_worker, const sp_uint_32& width, const sp_uint_32& height);
	void resize(SPRenderWorker& render_worker, const sp_uint_32& width, const sp_uint_32& height);
	void handle_surface_changes(SPRenderWorker& render_worker);
	std::vector<SPSwapchainBuffer> & get_swapchain_buffers();
	VkFormat& get_depth_format();


private:
	void prepare_frame(SPRenderWorker& render_worker, const sp_uint_32& width, const sp_uint_32& height);
	void submit_frame(SPRenderWorker& render_worker, const sp_uint_32& width, const sp_uint_32& height);
	void queue_begin_label(VkQueue queue, const char* label_name, std::vector<float> color);
	void queue_insert_label(VkQueue queue, const char* label_name, std::vector<float> color);
	void queue_end_label(VkQueue queue);
protected:
	SPEngine& engine_;
	const SPQueue& queue_;
protected:
	VkExtent2D surface_extent_;
	std::unique_ptr<SPSwapchain> swapchain_;
	std::vector<VkPresentModeKHR> present_mode_priority_list_;
	std::vector<VkSurfaceFormatKHR> surface_format_priority_list_;
	std::vector<std::unique_ptr<SPRenderFrame>> frames_;
	size_t thread_count_;
	bool prepared_;
	SPRenderTarget::CreateFunc create_render_target_func_;
	VkSurfaceTransformFlagBitsKHR pre_transform_{ VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR };

	VkFormat depth_format_;
	struct
	{
		// Swap chain image presentation
		VkSemaphore acquired_image_ready_;
		// Command buffer submission and execution
		VkSemaphore render_complete_;
	} semaphores_;
	VkPipelineStageFlags submit_pipeline_stages_ = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkSubmitInfo submit_info_;
	VkQueue vk_queue_;

	std::vector<SPSwapchainBuffer> swapchain_buffers_;
	uint32_t current_buffer_ = 0;

	bool debug_utils_supported_ = false;
};

#endif