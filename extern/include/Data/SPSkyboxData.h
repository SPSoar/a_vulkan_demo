#ifndef S_SPSKYBOXDATA_H
#define S_SPSKYBOXDATA_H


#include "SPEditData.h"

namespace spidx
{
	namespace skybox
	{
		static const sp_string HDRPath = "hdrPath_s_";
		static const sp_string Intensity = "intensity_s_";
		static const sp_string ThumbnailUrl = "thumbnailUrl_s_";
		static const sp_string Rotation = "rotation_s_";

	}

}

enum class SkyboxBinaryType : sp_uint_8
{
	None = 0,
	HDR
};

class SkyboxBinaryInfo : public SPObject
{
public:
	SkyboxBinaryInfo(const sp_uuid& id, const sp_string& name)
		:SPObject(id, "name")
		, skybox_type_(SkyboxBinaryType::None)
		, data_(nullptr)
		, len_(0)
	{}
	SkyboxBinaryInfo(const SkyboxBinaryInfo& other) = delete;
	SkyboxBinaryInfo& operator=(const SkyboxBinaryInfo& other) = delete;
	~SkyboxBinaryInfo()
	{
		delete[] data_;
	}
public:
	sp_uint_64 get_data_length()
	{
		return len_;
	}
	sp_uchar* get_data_ptr()
	{
		return data_;
	}
public:
	SkyboxBinaryType skybox_type_;
	sp_uint_64 len_;
	sp_uchar* data_;
};

using SkyboxBinaryInfoMap = std::unordered_map<sp_string, SkyboxBinaryInfo>;

class SPSkyboxData : public SPEditData, public SPBaseData<DataInfo>
{
	friend class SPDataParser<SPSkyboxData, DataType::SkyboxType>;
	friend class SPSceneFileController;
	friend class SPHDRBinaryParser;
public:
	SPSkyboxData() = delete;
	SPSkyboxData(const sp_uuid& id, const sp_string& name, const sp_uuid& pid);
	virtual ~SPSkyboxData();
	static SkyboxBinaryType Get_Skybox_Binary_Type_From_Str(const sp_string& str);
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
protected:
	SkyboxBinaryInfo skybox_binary_info_;
};

using SPSkyboxDataPtr = SPSkyboxData*;

#endif