#ifndef S_SPLightDATA_H
#define S_SPLightDATA_H

#include "SPEditData.h"

namespace spidx
{
	namespace light
	{
		static const sp_string Position = "position_s_";
		static const sp_string Direction = "direction_s_";
		static const sp_string Color = "color_s_";
		static const sp_string Intensity = "intensity_s_";
		static const sp_string HalfHeight = "halfHeight_s_";
		static const sp_string HalfWidth = "halfWidth_s_";
		static const sp_string LightType = "lightType_s_";
		static const sp_string Visible = "visible_s_";

	}

}
class SPLightData : public SPEditData, public SPBaseData<DataInfo>
{
	friend class SPDataParser<SPLightData, DataType::LightType>;
public:
	SPLightData() = delete;
	SPLightData(const sp_uuid& id, const sp_string& name, const sp_uuid& pid);
	virtual ~SPLightData();

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

using SPLightDataPtr = SPLightData*;
#endif