#ifndef S_SPSCENEDATA_H
#define S_SPSCENEDATA_H

#include "SPEditData.h"

#define SP_MAP_DELETE(dataMap) \
	for (auto& it : dataMap) \
		delete it.second;\
	dataMap.clear(); 

#define SP_DATA_DELETE(dataMap, id) \
	if(dataMap.find(id) != dataMap.end()) { \
		delete dataMap[id]; \
		dataMap.erase(id);  \
	}\

#define SP_DATA_DELETE_TATG(dataMap, id) \
	if(dataMap.find(id) != dataMap.end()) { \
		dataMap[id]->set_is_deleted(true); \
	}\

#define SP_DATA_SET(dataMap, class_ptr, data) \
	if(dataMap.find(data->get_id()) != dataMap.end()) { \
		delete dataMap[data->get_id()]; \
		dataMap.erase(data->get_id());  \
	}\
	dataMap[data->get_id()] = static_cast<class_ptr>(data);\

#define SP_DATA_CREATE(data_map, observers, Class, id, name, pid) \
	Class##Ptr data##Class = new Class(id, name, pid);\
	data##Class->init(); \
	data##Class->set_is_edited(true); \
	if(data_map.find(data##Class->get_id()) != data_map.end()) { \
		delete data_map[data##Class->get_id()]; \
		data_map.erase(data##Class->get_id());  \
	}\
	data##Class->pick_observers(observers);\
	data_map[data##Class->get_id()] = data##Class;\

#define SP_DATA_ADD_OBSERVER(dataMap, observer) \
	for (auto& data : dataMap) \
	{\
		data.second->add_observer(observer);\
	}\

#define SP_DATA_REMOVE_OBSERVER(dataMap, observer) \
	for (auto& data : dataMap) \
	{\
		data.second->remove_observer(observer);\
	}\

#define SP_DATA_PICK_OBSERVERS(dataMap, observerList) \
	for (auto& data : dataMap) \
	{\
		data.second->pick_observers(observerList);\
	}\

using SceneDataMap = std::unordered_map<sp_uuid, SPEditDataPtr>;

using GloabId = std::atomic_int64_t;

class SPSceneData : public SPEditData , public SPBaseData<DataInfo>
{
	friend class SPSceneFileController;
public:
	SPSceneData() = delete;
	SPSceneData(const sp_uuid& id, const sp_string& name, const sp_uuid& pid = 0);
	virtual ~SPSceneData();
public:
	void init();
	void clear_data();

	void set_data(const DataType& type, SPEditDataPtr base);
	SPEditDataPtr get_data(const DataType& type, const sp_uuid& id);
	void delete_data(const DataType& type, const sp_uuid& id);

	void set_scene_info(const sp_string& id, const SPVariant& value);
	SPVariant& get_scene_inof(const sp_string& id);
	void delete_scene_info(const sp_string& id);

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
	virtual void notify_observer_for_read_data_array(const DataType& type, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
protected:
	SceneDataMap camera_map_;
	SceneDataMap light_map_;
	SceneDataMap skybox_map_;
	SceneDataMap mesh_map_;
	SceneDataMap material_map_;
private:
};

using SPSceneDataPtr = SPSceneData*;

#endif