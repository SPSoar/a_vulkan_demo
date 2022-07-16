#ifndef S_SPFRAMEPLUGIN_H
#define S_SPFRAMEPLUGIN_H

#include <Base/base_common.h>

class SPDocumentInterface;

struct SPFrameWrapper 
{
    SPObject* left_panel_widget_;
    SPObject* right_panel_widget_;
    SPObject* bottom_panel_widget_;
};

enum class PluginGroup : sp_uint_8
{
    None = 0,
    EditorPlugin,
    SceneLibraryPlugin,
    LocalLibraryPlugin
};

enum class PluginLocation : sp_uint_8
{
    None = 0,
    Left,
    Right,
    Bottom
};


class COREEXPORT SPFramePlugin
{

public:
    SPFramePlugin(PluginGroup group, PluginLocation location) :group_(group), location_(location) {}
    virtual void on_create_main_frame_panel(SPFrameWrapper* frame_wrapper) = 0;
    virtual void on_destroy_main_frame_panel(SPFrameWrapper* frame_wrapper) = 0;
public:
    PluginGroup group_;
    PluginLocation location_;
};

class COREEXPORT SPFrameManager
{

public:
    static SPFrameManager* Get();
    SPFrameManager(){}
    virtual ~SPFrameManager() {}
	sp_bool add_frame_panel(SPFramePlugin* panel);
	void remove_frame_panel(SPFramePlugin* panel);
    void on_init(SPFrameWrapper* main_frame, const PluginGroup& group);
    void on_unit(SPFrameWrapper* main_frame);
 protected:
	std::vector<SPFramePlugin* > frames_;  
	static SPFrameManager* Frame_Manager_;
};


#endif
