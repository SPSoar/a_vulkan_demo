#include "SPRenderInterface.h"
#include "SPRenderControlCenter.h"


SP_SINGLETON_IMPL(SPRenderInterface)

SPRenderInterface::SPRenderInterface()
	: rcc_(nullptr)
{
}

void SPRenderInterface::init()
{
	SP_CREATE_OBJECT_PTR(rcc_, SPRenderControlCenter);
}

void SPRenderInterface::destroy()
{
	SP_DELETE_OBJECT_PTR(rcc_);
}

SPEditor* SPRenderInterface::get_render_controller()
{
	SPAssert(rcc_, "SPRenderInterface is not init");
	return rcc_;
}

void SPRenderInterface::render_frame(SPEditor* render_ctr, SPListener* swap_ctr, const sp_uint_32& width, const sp_uint_32& height)
{
	if (render_ctr == nullptr || swap_ctr == nullptr)
		return;
	SPRenderControlCenter* controller = static_cast<SPRenderControlCenter*>(render_ctr);
	if (!controller)
		return;
	controller->render_frame(swap_ctr, width, height);
}

SPListener* SPRenderInterface::create_swapchain(SPEditor* render_ctr, HWND win_id)
{
	if (render_ctr == nullptr)
		return nullptr;
	SPRenderControlCenter* controller = static_cast<SPRenderControlCenter*>(render_ctr);
	if (!controller)
		return nullptr;

	return controller->create_swap_chain(win_id);
}
