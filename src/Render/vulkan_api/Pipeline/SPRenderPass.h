#ifndef S_SPRENDERPASS_H_
#define S_SPRENDERPASS_H_

#include "../vulkan_common.h"

class SPDevice;
struct SPAttachment;

struct SPSubpassInfo
{
	std::vector<uint32_t> input_attachments_;

	std::vector<uint32_t> output_attachments_;

	std::vector<uint32_t> color_resolve_attachments_;

	bool disable_depth_stencil_attachment_;

	uint32_t depth_stencil_resolve_attachment_;

	VkResolveModeFlagBits depth_stencil_resolve_mode_;

	std::string debug_name_;
};


class SPRenderPass
{
public:
	SPRenderPass(SPDevice& device,
		const std::vector<SPAttachment>& attachments,
		const std::vector<SPLoadStoreInfo>& load_store_infos,
		const std::vector<SPSubpassInfo>& subpasses);

	SPRenderPass(const SPRenderPass&) = delete;
	SPRenderPass(SPRenderPass&& other) noexcept;
	~SPRenderPass();
	SPRenderPass& operator=(const SPRenderPass&) = delete;
	SPRenderPass& operator=(SPRenderPass&&) = delete;
public:
	const VkRenderPass& get_handle() const;
protected:
	template <typename T_SubpassDescription, typename T_AttachmentDescription, typename T_AttachmentReference, typename T_SubpassDependency, typename T_RenderPassCreateInfo>
	void create_renderpass(const std::vector<SPAttachment>& attachments, const std::vector<SPLoadStoreInfo>& load_store_infos, const std::vector<SPSubpassInfo>& subpasses);

protected:
	SPDevice& device_;
	uint64_t subpass_count_;
	std::vector<uint32_t> color_output_count_;
	VkRenderPass handle_;
};

#endif

