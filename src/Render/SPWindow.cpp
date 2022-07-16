#include "SPWindow.h"
#include "SPRenderControlCenter.h"

#include "vulkan_api/ak_engine/SPEngine.h"
#include "vulkan_api/ak_render/SPRenderContext.h"
#include "vulkan_api/ak_render/SPStats.h"
#include "vulkan_api/ak_pipeline/SPRenderWorker.h"
#include "vulkan_api/ak_scene/SPScene.h"
#include "vulkan_api/RenderComponents/SPSCamera.h"
SPWindow::SPWindow(SPEngine& engine, SPScene& scene, HWND win_id)
	: engine_{ engine }
	, scene_{ scene }
	, win_id_{ win_id }
	, render_context_{nullptr}
	, render_worker_{ nullptr }
	, stats_{nullptr}
	, last_mouse_position_{spv_vec2::Zero()}
{

}

SPWindow::~SPWindow()
{
	//SP_DELETE_OBJECT_PTR(stats_);
	SP_DELETE_OBJECT_PTR(render_worker_);
	SP_DELETE_OBJECT_PTR(render_context_);
}

void SPWindow::init()
{
	if (win_id_) 
	{
		engine_.create_surface(win_id_);
		engine_.create_queue();
		engine_.create_command_pool();
		engine_.create_fence_pool();
	}

	SP_CREATE_OBJECT_PTR(render_context_, SPRenderContext, engine_, 800, 600);

	prepare_render();

	SP_CREATE_OBJECT_PTR(render_worker_, SPRenderWorker, *render_context_, scene_, engine_);

	//render_worker_->prepare_pipeline();
	render_worker_->build_command_buffers();

}

void SPWindow::prepare_render()
{
	render_context_->prepare();

	stats_ = std::make_unique<SPStats>(*render_context_);

	render_context_->prepare_queue();

	render_context_->create_swapchain_buffers();
}

void SPWindow::mouse_moved(int x, int y)
{

	spd_decimal dx = (spd_decimal)last_mouse_position_.x() - x;
	spd_decimal dy = (spd_decimal)last_mouse_position_.y() - y;

	dx /= render_context_->get_surface_extent().width;
	dy /= render_context_->get_surface_extent().height;

	last_mouse_position_.x() = x;
	last_mouse_position_.y() = y;

	scene_.camera_.rotate(spv_vec2(dx, dy), 3.0);
}

void SPWindow::mouse_wheel(int x)
{
	
	spd_decimal offset = (spd_decimal) std::clamp(x, -10, 10) / 100;

	scene_.camera_.dolly(offset, 2);

}

void SPWindow::mouse_double_clicked()
{
	scene_.create_camera(render_context_->get_surface_extent().width, render_context_->get_surface_extent().height);
}

bool SPWindow::on_mouse_event(SPMouseEvent& event)
{
	return true;
}

void SPWindow::update_mouse_status(MouseEventType mouse_event_type, MouseButtonType button_type, bool update_pos, int x, int y)
{

	if (mouse_event_type == MouseEventType::Press)
	{
		if (update_pos)
			last_mouse_position_ = spv_vec2(x, y);

		if (button_type == MouseButtonType::LeftButton)
			left_button_pressed_ = true;
		if (button_type == MouseButtonType::RightButton)
			right_button_pressed_ = true;
		if (button_type == MouseButtonType::MidButton)
			mid_button_pressed_ = true;
	}
	else if (mouse_event_type == MouseEventType::Release)
	{
		if (button_type == MouseButtonType::LeftButton)
			left_button_pressed_ = false;
		if (button_type == MouseButtonType::RightButton)
			right_button_pressed_ = false;
		if (button_type == MouseButtonType::MidButton)
			mid_button_pressed_ = false;
	}
}

void SPWindow::render_frame(const sp_uint_32& width, const sp_uint_32& height)
{
	render_context_->draw(*render_worker_, width, height);

	{
		render_worker_->update_uniform_buffers(0.1);
		//scene_.camera_.update_ = false;
	}
	
}

void SPWindow::notify_mouse_event(SPMouseEvent& event)
{
	if (on_mouse_event(event))
	{
		update_mouse_status(event.get_event_type(), event.get_button_type(), true, event.get_x(), event.get_y());

		switch (event.get_event_type())
		{
		case MouseEventType::MouseMove:
			mouse_moved(event.get_x(), event.get_y());
			break;
		case MouseEventType::Wheel:
			mouse_wheel(event.get_x());
			break;
		case MouseEventType::Press:
			break;
		case MouseEventType::Release:
			break;
		case MouseEventType::DoubleClick: 
		{
			if (event.get_button_type() == MouseButtonType::RightButton) 
			{
				mouse_double_clicked();
				break;
			}
		}
		default:
			break;
		}

	}
}

void SPWindow::notify_key_event(SPKeyEvent& event)
{

}

void SPWindow::notify_extend_event(SPExtendEvent& event)
{

}
