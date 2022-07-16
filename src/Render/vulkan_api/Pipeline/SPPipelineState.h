#ifndef S_SPPIPELINESTATE_H_
#define S_SPPIPELINESTATE_H_

#include "../vulkan_common.h"

//该文件中的都是状态类，使用默认构造等

struct VertexInputState
{
	std::vector<VkVertexInputBindingDescription> bindings_;
	std::vector<VkVertexInputAttributeDescription> attributes_;
};

struct InputAssemblyState
{
	VkPrimitiveTopology topology_{ VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST };
	VkBool32 primitive_restart_enable_{ VK_FALSE };
};

struct RasterizationState
{
	VkBool32 depth_clamp_enable_{ VK_FALSE };

	VkBool32 rasterizer_discard_enable_{ VK_FALSE };

	VkPolygonMode polygon_mode_{ VK_POLYGON_MODE_FILL };

	VkCullModeFlags cull_mode_{ VK_CULL_MODE_BACK_BIT };

	VkFrontFace front_face_{ VK_FRONT_FACE_COUNTER_CLOCKWISE };

	VkBool32 depth_bias_enable_{ VK_FALSE };
};

struct ViewportState
{
	uint32_t viewport_count_{ 1 };
	uint32_t scissor_count_{ 1 };
};

struct MultisampleState
{
	VkSampleCountFlagBits rasterization_samples_{ VK_SAMPLE_COUNT_1_BIT };

	VkBool32 sample_shading_enable_{ VK_FALSE };

	float min_sample_shading_{ 0.0f };

	VkSampleMask sample_mask_{ 0 };

	VkBool32 alpha_to_coverage_enable_{ VK_FALSE };

	VkBool32 alpha_to_one_enable_{ VK_FALSE };
};

struct StencilOpState
{
	VkStencilOp fail_op_{ VK_STENCIL_OP_REPLACE };

	VkStencilOp pass_op_{ VK_STENCIL_OP_REPLACE };

	VkStencilOp depth_fail_op_{ VK_STENCIL_OP_REPLACE };

	VkCompareOp compare_op_{ VK_COMPARE_OP_NEVER };
};

struct DepthStencilState
{
	VkBool32 depth_test_enable_{ VK_TRUE };

	VkBool32 depth_write_enable_{ VK_TRUE };

	// Note: Using Reversed depth-buffer for increased precision, so Greater depth values are kept
	VkCompareOp depth_compare_op_{ VK_COMPARE_OP_GREATER };

	VkBool32 depth_bounds_test_enable_{ VK_FALSE };

	VkBool32 stencil_test_enable_{ VK_FALSE };

	StencilOpState front_{};

	StencilOpState back_{};
};


struct ColorBlendAttachmentState
{
	VkBool32 blend_enable_{ VK_FALSE };

	VkBlendFactor src_color_blend_factor_{ VK_BLEND_FACTOR_ONE };

	VkBlendFactor dst_color_blend_factor_{ VK_BLEND_FACTOR_ZERO };

	VkBlendOp color_blend_op_{ VK_BLEND_OP_ADD };

	VkBlendFactor src_alpha_blend_factor_{ VK_BLEND_FACTOR_ONE };

	VkBlendFactor dst_alpha_blend_factor_{ VK_BLEND_FACTOR_ZERO };

	VkBlendOp alpha_blend_op_{ VK_BLEND_OP_ADD };

	VkColorComponentFlags color_write_mask_{ VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT };
};

struct ColorBlendState
{
	VkBool32 logic_op_enable_{ VK_FALSE };

	VkLogicOp logic_op_{ VK_LOGIC_OP_CLEAR };

	std::vector<ColorBlendAttachmentState> attachments_;
};

/// Helper class to create specialization constants for a Vulkan pipeline. The state tracks a pipeline globally, and not per shader. Two shaders using the same constant_id will have the same data.
class SpecializationConstantState
{
public:
	void reset();

	bool is_dirty() const;

	void clear_dirty();

	template <class T>
	void set_constant(uint32_t constant_id, const T& data);

	void set_constant(uint32_t constant_id, const std::vector<uint8_t>& data);

	void set_specialization_constant_state(const std::map<uint32_t, std::vector<uint8_t>>& state);

	const std::map<uint32_t, std::vector<uint8_t>>& get_specialization_constant_state() const;

private:
	bool dirty_{ false };
	// Map tracking state of the Specialization Constants
	std::map<uint32_t, std::vector<uint8_t>> specialization_constant_state_;
};

class SPPipelineLayout;
class SPRenderPass;
class SPPipelineState
{
public:
	void reset();
	uint32_t get_subpass_index() const;

	void clear_dirty();
private:
	uint32_t subpass_index_{ 0U };
	bool dirty_{ false };
	SpecializationConstantState specialization_constant_state_{};
	SPPipelineLayout *pipeline_layout_{ nullptr };
	const SPRenderPass* render_pass_{ nullptr };
	VertexInputState vertex_input_state_{};
	InputAssemblyState input_assembly_state_{};
	RasterizationState rasterization_state_{};
	MultisampleState multisample_state_{};
	DepthStencilState depth_stencil_state_{};
	ColorBlendState color_blend_state_{};

	ViewportState viewport_state_{};
};

#endif

template<class T>
inline void SpecializationConstantState::set_constant(uint32_t constant_id, const T& data)
{
}
