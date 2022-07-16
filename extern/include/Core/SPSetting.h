#ifndef S_SPSETTING_H
#define S_SPSETTING_H

#include <Base/base_common.h>

typedef std::unordered_map<sp_string, sp_string> SPConfig;

#define SCENEDIR    "SceneData"
#define CONFIGDIR   "ConfigData"

namespace spsetting 
{
	static const sp_string App_Exe_Path	   = "appExePath";
	static const sp_string App_Exe_Dir     = "appExeDir";
	static const sp_string App_Name		   = "appName";
	static const sp_string App_Version	   = "appVersion";
	static const sp_string App_Icon		   = "appIcon";
	static const sp_string Scene_Path      = "sceneDirPath";
	static const sp_string Config_Path	   = "configPath";
	static const sp_string Root_Path	   = "rootPath";

}


struct COREEXPORT SPSetting
{

public:
	static void Create_Dirs(void);
	static SPConfig& Config();
	SP_CLASS_DELETE_COPY(SPSetting)
	SP_CLASS_DELETE_MOVE(SPSetting)

private:
	SPSetting() = default;
	virtual ~SPSetting() = default;
	static SPConfig Config_;
};

#endif
