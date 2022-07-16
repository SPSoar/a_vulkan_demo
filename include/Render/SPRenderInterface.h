#ifndef S_SPRENDERINTERFACE_H_
#define S_SPRENDERINTERFACE_H_

#include "render_common.h"

class SPRenderControlCenter;
class RENDEREXPORT SPRenderInterface : public SPSingleton<SPRenderInterface>
{
public:
	SP_SINGLETON_PUBLIC_DEF(SPRenderInterface)
	SPEditor* get_render_controller();
	void render_frame(SPEditor* render_ctr, SPListener* swap_ctr, const sp_uint_32& width, const sp_uint_32& height);
	SPListener* create_swapchain(SPEditor* render_ctr, HWND win_id);
protected:
private:
	SPRenderControlCenter* rcc_ = nullptr;
};


#endif
