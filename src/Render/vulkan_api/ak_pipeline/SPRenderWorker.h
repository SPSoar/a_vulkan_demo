#ifndef S_SPRENDERWORKER_H_
#define S_SPRENDERWORKER_H_

#include "../vulkan_common.h"

#include <random>

#define PARTICLES_PER_ATTRACTOR 4 * 1024

class SPRenderContext;
class SPEngine;
class SPScene;
class SPBuffer;
struct STexture;
class SPSSubMesh;

enum RenderPassCreateFlags
{
	ColorAttachmentLoad = 0x00000001
};

class SPRenderWorker
{
	friend class SPRenderContext;
public:
	SPRenderWorker() = delete;
	SPRenderWorker(SPRenderContext& render_context_, SPScene& scene, SPEngine& engine);
	~SPRenderWorker();
	SPRenderWorker(SPRenderWorker const& v) = delete;
	SPRenderWorker(SPRenderWorker&& v) = delete;
	SPRenderWorker& operator=(SPRenderWorker& v) = delete;
	SPRenderWorker& operator=(SPRenderWorker&& v) = delete;
public:
	void init();
	void resize(const sp_uint_32& width, const sp_uint_32& height);
	uint16_t get_with();
	uint16_t get_height();
	void build_command_buffers();
	void update_uniform_buffers(sp_decimal_d d);
	void update_render_pass_flags(uint32_t flags = 0);
private:
	void draw_model(std::unique_ptr<SPSSubMesh>& model, VkCommandBuffer command_buffer);
	VkPipelineShaderStageCreateInfo load_shader(const std::string& file, VkShaderStageFlagBits stage);
	VkDescriptorBufferInfo create_descriptor(SPBuffer& buffer, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
	uint64_t get_buffer_device_address(VkBuffer buffer);
public:
	void create_command_buffers();
	void destroy_command_buffers();
	void create_synchronization_primitives();
	void setup_depth_stencil();
	void setup_render_pass();
	void setup_framebuffer();

	//------------------------------
	void update_swapchain_image_usage_flags(std::set<VkImageUsageFlagBits> image_usage_flags);
	void create_storage_image();

	void create_ray_tracing_pipeline();
	void create_shader_binding_tables();

	void create_descriptor_sets();
private:
	SPEngine& engine_;
	SPRenderContext& render_context_;
	SPScene& scene_;
	//---------------------------
	std::vector<VkCommandBuffer> draw_cmd_buffers_;
	std::vector<VkFence> wait_fences_;
	VkRenderPass render_pass_;
	std::vector<VkFramebuffer> framebuffers_;
	VkDescriptorPool descriptor_pool_ = VK_NULL_HANDLE;
	//------------------------------
	struct
	{
		VkImage        image_;
		VkDeviceMemory mem_;
		VkImageView    view_;
	} depth_stencil_;
	VkPhysicalDeviceRayTracingPipelinePropertiesKHR  ray_tracing_pipeline_properties_{};
	VkPhysicalDeviceAccelerationStructureFeaturesKHR acceleration_structure_features_{};

	struct StorageImage
	{
		VkDeviceMemory memory_;
		VkImage        image_ = VK_NULL_HANDLE;
		VkImageView    view_;
		VkFormat       format_;
		uint32_t       width_;
		uint32_t       height_;
	} storage_image_;


	VkPipeline            pipeline_;
	VkPipelineLayout      pipeline_layout_;
	VkDescriptorSet       descriptor_set_;
	VkDescriptorSetLayout descriptor_set_layout_;

	std::vector<VkRayTracingShaderGroupCreateInfoKHR> shader_groups_{};

	std::unique_ptr<SPBuffer> raygen_shader_binding_table_;
	std::unique_ptr<SPBuffer> miss_shader_binding_table_;
	std::unique_ptr<SPBuffer> hit_shader_binding_table_;
};

#endif