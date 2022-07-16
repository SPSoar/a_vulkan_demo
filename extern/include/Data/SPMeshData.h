#ifndef S_SPMODELDATA_H
#define S_SPMODELDATA_H


#include "SPEditData.h"

namespace spidx
{
	namespace mesh
	{
		static const sp_string MeshPath = "meshPath_s_";
		static const sp_string Offset   = "offset_s_";
		static const sp_string Rotation = "rotation_s_";
		static const sp_string Scale    = "scale_s_";
	}

}

enum class MeshBinaryType : sp_uint_8
{
	None = 0,
	Obj,
	Fbx
};

class MeshBinaryInfo : public SPObject
{
public:
	MeshBinaryInfo(const sp_uuid& id, const sp_string& name)
		:SPObject(id, "name")
		, mesh_binary_type_(MeshBinaryType::None)
		, data_(nullptr)
		, len_(0)
	{}
	MeshBinaryInfo(const MeshBinaryInfo& other) = delete;
	MeshBinaryInfo& operator=(const MeshBinaryInfo& other) = delete;
	~MeshBinaryInfo()
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
	MeshBinaryType mesh_binary_type_;
	sp_uint_64 len_;
	sp_uchar* data_;
};

using MeshInfoMap = std::unordered_map<sp_string, MeshBinaryInfo>;

class SPMeshData : public SPEditData, public SPBaseData<DataInfo>
{
	friend class SPDataParser<SPMeshData, DataType::MeshType>;
	friend class SPSceneFileController;
	friend class SPMeshBinaryParser;
public:
	SPMeshData() = delete;
	SPMeshData(const sp_uuid& id, const sp_string& name, const sp_uuid& pid);
	virtual ~SPMeshData();
	static MeshBinaryType Get_Mesh_Binary_Type_From_Str(const sp_string& str);
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
	MeshBinaryInfo mesh_binary_info;
};

using SPMeshDataPtr = SPMeshData*;

#endif