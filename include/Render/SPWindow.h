#ifndef S_SPWINDOW_H_
#define S_SPWINDOW_H_

#include "render_common.h"

class SPEngine;
class SPScene;
class SPRenderContext;
class SPRenderWorker;
class SPStats;
class SPWindow : public SPListener
{
public:
	SPWindow(SPEngine& engine, SPScene& scene, HWND win_id);
	~SPWindow();
	void init();
	void render_frame(const sp_uint_32& width, const sp_uint_32& height);

	virtual void notify_mouse_event(SPMouseEvent& event);
	virtual void notify_key_event(SPKeyEvent& event);
	virtual void notify_extend_event(SPExtendEvent& event);
private:
	void prepare_render();

	void mouse_moved(int x, int y);
	void mouse_wheel(int x);
	void mouse_double_clicked();
	bool on_mouse_event(SPMouseEvent& event);

	void update_mouse_status(MouseEventType mouse_event_type, MouseButtonType button_type, bool update_pos = true, int x = 0, int y = 0);

private:
	SPEngine& engine_;
	SPScene& scene_;
	HWND win_id_;
	SPRenderContext* render_context_;
	SPRenderWorker* render_worker_;
	std::unique_ptr<SPStats> stats_;

	sp_bool  left_button_pressed_ = false;
	sp_bool  right_button_pressed_ = false;
	sp_bool  mid_button_pressed_ = false;
	spv_vec2 last_mouse_position_;
};


#endif