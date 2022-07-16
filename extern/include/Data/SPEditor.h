#ifndef S_SPEDITOR_H
#define S_SPEDITOR_H

#include "SPDataTypeDefine.h"

enum class EditorType : sp_uint_8
{
	None = 0,
	SceneEditor,
	EditorEditor,
	RenderEditor,
	LocalLibraryEditor,
	FileEditor
};

class SPEditData;
using SceneDataMap = std::unordered_map<sp_uuid, SPEditData*>;
class SPEditor 
{
public:
	SPEditor() = default;
	virtual ~SPEditor() = default;
	virtual void read_data(SPEditData* data, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void read_data_array(SceneDataMap& data_map, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void write_data(SPEditData* data, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void save_data(SPEditData* data, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void edit_data(SPEditData* data, const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void read_modified_data(SPEditData* data, const SPAccess& acc, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void command(SPEditData* data, const SPCommand& command, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
	virtual void command_modified(SPEditData* data, const SPCommand& command, const SPVariant& var, CallbackFunc* callback_func = nullptr, sp_bool is_async = false) = 0;
public:
	SP_CLASS_MEMBER(SPEditor, EditorType, editor_type)
public:
	Permissions scene_permissions_;
	Permissions camera_permissions_;
	Permissions light_permissions_;
	Permissions skybox_permissions_;
	Permissions mesh_permissions_;
	Permissions material_permissions_;
	Permissions material_layer_permissions_;
	sp_uint_8	data_level_;
};

struct  SPEditorComp {
	bool operator()(const SPEditor* el, const SPEditor* er) const {
		return el->data_level_ < er->data_level_;
	
	}
};

#endif