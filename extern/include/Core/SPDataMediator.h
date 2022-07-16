#ifndef S_SPDATAMEDIATOR_H
#define S_SPDATAMEDIATOR_H

#include <Data/data_common.h>

class COREEXPORT SPDataMediator : public SPSingleton<SPDataMediator>
{

public:
	SP_SINGLETON_PUBLIC_DEF(SPDataMediator)
public:
	void add_data_editor(SPEditor* editor);
	void reset_data_editor();
	void read_data(const DataType& dataType, const sp_uuid& pid, const sp_uuid& id, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
	void read_material_layer_data(const sp_uuid& pid, const sp_uuid& id, MaterialPositionType type, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
	void read_data_array(const DataType& dataType, const sp_uuid& pid, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
	void write_data(const DataType& dataType, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
	void save_data(const DataType& dataType, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
	void edit_data(const DataType& dataType, const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
	void command_for_data(const SPCommand& command, SPVariant var = SPVariant(), CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
	void save_all_data();

protected:
	std::multiset<SPEditor*, SPEditorComp> date_editors_;
	SPSceneDataPtr scene_data_;
};

#endif