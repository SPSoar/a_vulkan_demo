#ifndef S_SPRENDERCONTROLLER_H_
#define S_SPRENDERCONTROLLER_H_

#include "render_common.h"

class SPEngine;

class SPWindow;
class SPScene;
class SPRenderControlCenter : public SPEditor
{
	friend class SPWindow;

public:
	SPRenderControlCenter();
	virtual ~SPRenderControlCenter();
	void init();
	virtual void read_data(SPEditData* data, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void read_data_array(SceneDataMap& data_map, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override {}
	virtual void write_data(SPEditData* data, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void save_data(SPEditData* data, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void edit_data(SPEditData* data, const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void read_modified_data(SPEditData* data, const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void command(SPEditData* data, const SPCommand& commad, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void command_modified(SPEditData* data, const SPCommand& commad, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
public:
	SPListener* create_swap_chain(HWND win_id);
	void render_frame(SPListener* swap_ctr, const sp_uint_32& width, const sp_uint_32& height);

protected:
	SPEngine* engine_;
	SPWindow* swap_ctr_;
	SPScene* scene_;
	bool open = false;
};

#endif