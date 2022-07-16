#ifndef S_SPVULKANDRAWINGSKILL_H_
#define S_SPVULKANDRAWINGSKILL_H_
#include "vulkan_common.h"
#include "SPVulkanWindowsDriver.h"
#include "SPVulkanArtist.h"
class  SPVulkanDrawingSkill
{
public:
	SPVulkanDrawingSkill(SPVulkanWindowsDriver& driver, SPVulkanArtist& artist);
	virtual ~SPVulkanDrawingSkill();
	void init();
	void startDraw();
	void draw();
	void endDraw();
protected:

private:

	SPVulkanWindowsDriver& s_manager;
	SPVulkanArtist& s_artist;
public:

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;
	std::vector<VkFence> imagesInFlight;

	size_t currentFrame = 0;
	spbool framebufferResized = false;

};	

#endif
