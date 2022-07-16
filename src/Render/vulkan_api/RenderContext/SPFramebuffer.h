#ifndef S_SPINSTANCEEXTENSIONS_H_
#define S_SPINSTANCEEXTENSIONS_H_

#include "../vulkan_common.h"

class SPDevice;
class SPRenderTarget;
class SPRenderPass;
class SPFramebuffer
{
public:
	SPFramebuffer(SPDevice& device, const SPRenderTarget& render_target, const SPRenderPass& render_pass);
	SPFramebuffer(const SPFramebuffer&) = delete;
	SPFramebuffer(SPFramebuffer&& other) noexcept;
	~SPFramebuffer();
	SPFramebuffer& operator=(const SPFramebuffer&) = delete;
	SPFramebuffer& operator=(SPFramebuffer&&) = delete;
public:
	VkFramebuffer get_handle() const;
	const VkExtent2D& get_extent() const;
protected:
	SPDevice& device_;
	VkFramebuffer handle_;
	VkExtent2D extent_;
};

#endif