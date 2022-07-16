#ifndef S_SPMATERIALDATA_H
#define S_SPMATERIALDATA_H


#include "SPEditData.h"
#include "SPMaterialLayerData.h"

enum class MaterialPositionType : sp_uint_8
{
	None = 0,
	Positive,
	Reverse
};

class SPMaterialData : public SPEditData, public SPBaseData<DataInfo>
{
	friend class SPMaterialParser;
	friend class SPSceneFileController;
public:
	SPMaterialData() = delete;
	SPMaterialData(const sp_uuid& id, const sp_string& name, const sp_uuid& pid);
	virtual ~SPMaterialData();

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
	const std::list<SPMaterialLayerDataPtr>& get_positive_layer_list();
	const std::list<SPMaterialLayerDataPtr>& get_reverse_layer_list();
	void create_material_layer(const sp_uuid& id, const sp_string& name, MaterialPositionType type);
	void delete_material_layer(const sp_uuid& id, MaterialPositionType type);
public:
	void notify_material_layer_edit_data(const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
	void notify_material_layer_read_data(const sp_uuid& id, MaterialPositionType type, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);

protected:

	std::list<SPMaterialLayerDataPtr> positive_layer_list_;
	std::list<SPMaterialLayerDataPtr> reverse_layer_list_;
};

using SPMaterialDataPtr = SPMaterialData*;

#endif