#ifndef S_SPPLUGINMANAGER_H_
#define S_SPPLUGINMANAGER_H_


#include <Base/base_common.h>

class SPPlugin;
class COREEXPORT SPPluginManager
{
private:

#pragma warning(push)
#pragma warning(disable:4251)
	static std::unordered_map<sp_string, void*> Handles_;
	static std::vector<sp_string> Plugins_Path_;
	static std::vector<SPPlugin*> Plugins_;
#pragma warning(pop)
	
	SPPluginManager(const SPPluginManager&) = delete;
	SPPluginManager& operator=(const SPPluginManager&) = delete;
	SPPluginManager() = delete;

public:
	static void Add_Plugin_Path(const sp_string& path);
	static void Load_All_Plugins();
	static void Unload_All_Plugins();
	static void Init_All_Plugins();
	static void UnInit_All_Plugins();

private:
	static sp_bool Load_Plugin(const sp_string& path);
	static void* Open_Library(const sp_string& path);
	static void* Lookup_Library(void* handle, const sp_string& name);
	static void Close_Library(void* handle);

};

#endif  