#ifndef S_SPCAMERADATA_H
#define S_SPCAMERADATA_H

#include "SPEditData.h"

namespace spidx
{
	namespace camera 
	{
		static const sp_string Position = "position_s_";
		static const sp_string LookAt = "lookAt_s_";
		static const sp_string Up = "up_s_";
		static const sp_string Projection = "projection_s_";
		static const sp_string FovType = "fovType_s_";
		static const sp_string Fov = "fov_s_";
		static const sp_string Exposure = "exposure_s_";
		static const sp_string Near = "near_s_";
		static const sp_string Far = "Far_s_";
		static const sp_string ShowGround = "showGround_s_";
		static const sp_string ShadowSwitch = "shadowSwitch_s_";
		static const sp_string BackgroundType = "backgroundType_s_";
		static const sp_string BackgroundColor0 = "backgroundColor0_s_";
		static const sp_string BackgroundColor1 = "backgroundColor1_s_";
	}
	
}

struct BuilderDetails;

class SPCameraData : public SPEditData , public SPBaseData<DataInfo>
{
	friend class SPDataParser<SPCameraData, DataType::CameraType>;
public:	
	enum class Projection : int {
		PERSPECTIVE,
		ORTHO
	};

	enum class Fov : int {
		VERTICAL,
		HORIZONTAL
	};
public:
	SPCameraData() = delete;
	SPCameraData(const sp_uuid& id, const sp_string& name, const sp_uuid& pid);
	virtual ~SPCameraData();

public:
	virtual void pick_observers(std::multiset<SPEditor*, SPEditorComp> observers) override;
	virtual void add_observer(SPEditor* observer) override;
	virtual void remove_observer(SPEditor* observer) override;
	virtual void notify_observer_for_read_data(CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_write_data(const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_save_data(const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_edit_data(const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_read_modified_data(const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_command(const SPCommand& command, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;
	virtual void notify_observer_for_command_modified(const SPCommand& command, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) override;

public:
	void init();
};

using SPCameraDataPtr = SPCameraData*;

#endif