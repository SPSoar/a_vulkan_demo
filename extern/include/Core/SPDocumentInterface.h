#ifndef S_SPDOCUMENTINTERFACE_H
#define S_SPDOCUMENTINTERFACE_H

#include <Base/base_common.h>
#include <Data/data_common.h>

class SPListener;
class SPDataMediator;
class SPKeyEvent;
class SPMouseEvent;
class SPExtendEvent;

class COREEXPORT SPDocumentInterface : public SPSingleton<SPDocumentInterface>
{
public:
    SP_SINGLETON_PUBLIC_DEF(SPDocumentInterface)
 
public:

    void add_data_editor(SPEditor * editor);
    void assign_data_editor();

    void create_scene(const sp_uuid& id, const sp_string& name);
    void change_scene_name(const sp_uuid& id, const sp_string& name);
    bool delete_scene_by_id(const sp_uuid& id);
    void create_scene_element(const DataType& type, const sp_uuid& pid, const sp_uuid& id = 0, const SPVariant& val = SPVariant());
    void change_scene_element_name(const DataType& type, const sp_uuid& pid, const sp_uuid& id, const sp_string& name, const SPVariant& val = SPVariant());
    bool delete_scene_element(const DataType& type, const sp_uuid& pid, const sp_uuid& id, const SPVariant& val = SPVariant());

    void read_data(const DataType& type, const sp_uuid& pid, const sp_uuid& id);
    void read_material_layer_data(const sp_uuid& pid, const sp_uuid& id, MaterialPositionType type, CallbackFunc* callback_func = nullptr, sp_bool is_async = false);
    void read_data_array(const DataType& type, const sp_uuid& pid);
    void write_data(const DataType& type, const sp_uuid& id);
    void edit_data(const DataType& type, const sp_uuid& id, const std::string& index, const SPVariant& val);
    void edit_data(const DataType& type, const sp_uuid& pid, const sp_uuid& id, const std::string& index, const SPVariant& val);
    void save_all_data();

    SPEditor* get_render_editor();

    void add_event_listener(SPListener* listener);
    void remove_event_listener(SPListener* listener);
    void notify_key_event(SPKeyEvent& event);
    void notify_mouse_event(SPMouseEvent& event);
    void notify_extend_event(SPExtendEvent& event);


private:
    SPDataMediator& data_mediator_;
    SPEditor* render_editor_;
#pragma warning(push)
#pragma warning(disable:4251)
    std::list<SPListener*> event_listeners_;
    sp_bool notify_global_listeners;
#pragma warning(pop)
};

#endif
