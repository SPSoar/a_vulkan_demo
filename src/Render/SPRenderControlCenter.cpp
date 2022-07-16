#include "SPRenderControlCenter.h"
#include "SPWindow.h"
#include "vulkan_api/ak_engine/SPEngine.h"
#include "vulkan_api/ak_scene/SPScene.h"

SPRenderControlCenter::SPRenderControlCenter()
	: SPEditor()
	, engine_{ nullptr }
	, swap_ctr_(nullptr)
	, scene_{ nullptr }
{
	editor_type_ = EditorType::RenderEditor;
	data_level_ = DATA_LEVEL_RENDER;

	scene_permissions_.setReadPermissions(true);
	scene_permissions_.setReadModifiedPermissions(true);
	scene_permissions_.setCommandModifiedPermissions(true);

	light_permissions_.setReadModifiedPermissions(true);
	light_permissions_.setEditPermissions(true);
}

SPRenderControlCenter::~SPRenderControlCenter()
{
	SP_DELETE_OBJECT_PTR(swap_ctr_);
	SP_DELETE_OBJECT_PTR(scene_);
	SP_DELETE_OBJECT_PTR(engine_);
}

void SPRenderControlCenter::init()
{
	SPLogger::Get()->init("render.log");

	SP_CREATE_OBJECT_PTR(engine_, SPEngine);
	SP_CREATE_OBJECT_PTR(scene_, SPScene, *engine_);

}

void SPRenderControlCenter::read_data(SPEditData* data, CallbackFunc* callback_func, sp_bool is_async)
{
	SPSceneDataPtr scene = static_cast<SPSceneDataPtr>(data);
}

void SPRenderControlCenter::write_data(SPEditData* data, const SPVariant& var, CallbackFunc* callback_func, sp_bool is_async)
{
}

void SPRenderControlCenter::save_data(SPEditData* data, const SPVariant& var, CallbackFunc* callback_func, sp_bool is_async)
{
}

void SPRenderControlCenter::edit_data(SPEditData* data, const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func, sp_bool is_async)
{
	switch (data->get_data_type())
	{
	case DataType::LightType:
	{
		SPLightDataPtr light = static_cast<SPLightDataPtr>(data);
		if (acc.data_idx_ == spidx::light::LightType) 
		{
			//scene_->ubo_.sampler_index = var.i;
			//scene_->update_uniform_buffers();
		}
	}
	default:
		break;
	}
}

void SPRenderControlCenter::read_modified_data(SPEditData* data, const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func, sp_bool is_async)
{
}

void SPRenderControlCenter::command(SPEditData* data, const SPCommand& commad, const SPVariant& var, CallbackFunc* callback_func, sp_bool is_async)
{
}

void SPRenderControlCenter::command_modified(SPEditData* data, const SPCommand& commad, const SPVariant& var, CallbackFunc* callback_func, sp_bool is_async)
{
}

SPListener* SPRenderControlCenter::create_swap_chain(HWND win_id)
{

	SP_CREATE_OBJECT_PTR(swap_ctr_, SPWindow, *engine_, *scene_, win_id);
	return swap_ctr_;
}

void SPRenderControlCenter::render_frame(SPListener* swap_ctr, const sp_uint_32& width, const sp_uint_32& height)
{
	SPWindow* controller = static_cast<SPWindow*>(swap_ctr);
	if(controller)
		controller->render_frame(width, height);
}
