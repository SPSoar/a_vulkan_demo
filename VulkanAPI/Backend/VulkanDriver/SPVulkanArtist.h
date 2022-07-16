#ifndef S_SPVULKANARTIST_H_
#define S_SPVULKANARTIST_H_
#include "vulkan_common.h"
#include "SPDraftVulkan.h"
#include "SPMaterialVulkan.h"

struct ArtistContext 
{
	VkFormat swapSurfaceFormat;
	VkColorSpaceKHR colorSpace;
	VkPresentModeKHR presentMode;
};

struct ColorAttachment 
{
	VkAttachmentLoadOp loadOp;
	VkAttachmentStoreOp storeOp;
	VkAttachmentLoadOp stencilLoadOp;
	VkAttachmentStoreOp stencilStoreOp;
	VkImageLayout initialLayout;
	VkImageLayout finalLayout;
};
struct DepthAttachment 
{
	VkAttachmentLoadOp loadOp;
	VkAttachmentStoreOp storeOp;
	VkAttachmentLoadOp stencilLoadOp;
	VkAttachmentStoreOp stencilStoreOp;
	VkImageLayout initialLayout;
	VkImageLayout finalLayout;
};

struct ColorAttachmentResolve 
{
	VkAttachmentLoadOp loadOp;
	VkAttachmentStoreOp storeOp;
	VkAttachmentLoadOp stencilLoadOp;
	VkAttachmentStoreOp stencilStoreOp;
	VkImageLayout initialLayout;
	VkImageLayout finalLayout;
	VkSampleCountFlagBits samples;
};

struct ColorAttachmentRef
{
	spuint attachment;
	VkImageLayout layout;
};

struct DepthAttachmentRef
{
	spuint attachment;
	VkImageLayout layout;
};

struct ColorAttachmentResolveRef
{
	spuint attachment;
	VkImageLayout layout;
};

struct Subpass
{
	VkPipelineBindPoint pipelineBindPoint;
	spuint inputAttachmentCount;
	spbool pInputAttachments;
	spuint colorAttachmentCount;
	spbool pColorAttachments;
	spbool pResolveAttachments;
	spbool pDepthStencilAttachment;
	spuint preserveAttachmentCount;
	spbool pPreserveAttachments;
};

struct Dependency
{
	spuint                srcSubpass;
	spuint                dstSubpass;
	VkPipelineStageFlags    srcStageMask;
	VkPipelineStageFlags    dstStageMask;
	VkAccessFlags           srcAccessMask;
	VkAccessFlags           dstAccessMask;
	VkDependencyFlags       dependencyFlags;
};

class SPDrawingPadVulkan;
class SPVulkanDrawingSkill;
class  SPVulkanArtist
{
public:
	SPVulkanArtist(SPDrawingPadVulkan& drawingBoard);
	virtual ~SPVulkanArtist();
	void init();
	void draft();
	void shader();
	void addDrawingSkill(SPVulkanDrawingSkill* skill);
public:
	SPDrawingPadVulkan& drawingBoard;


	ArtistContext artistContext;
	VkSurfaceFormatKHR vkSurfaceFormat;
	unsigned int width;
	unsigned int height;
	ColorAttachment colorAttachment;
	DepthAttachment depthAttachment;
	ColorAttachmentResolve colorAttachmentResolve;
	ColorAttachmentRef colorAttachmentRef;
	DepthAttachmentRef depthAttachmentRef;
	ColorAttachmentResolveRef colorAttachmentResolveRef;
	Subpass subpass;
	Dependency dependency;

	//shader---------------------------------
	SPVulkanDrawingSkill* skill;

	
};

#endif
