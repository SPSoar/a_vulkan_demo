#ifndef S_SPFRAMEBUFFER_H_
#define S_SPFRAMEBUFFER_H_

//需要一个管理vulkan扩展的管理类，暂时使用容器存储
#include "../vulkan_common.h"

class SPInstanceInfoManager
{
public:
	static void Add_Instance_Extension(const char* extension, bool optional = false);
	static void Add_Validation_Layers(const char* validation);
	
	static std::vector<const char*> Get_Optimal_Validation_Layers(const std::vector<VkLayerProperties>& supported_instance_layers);
	static bool Validate_Layers(const std::vector<const char*>& required, const std::vector<VkLayerProperties>& available);
public:
	static std::unordered_map<const char*, bool> Instance_Extensions_;
	static std::vector<const char*> Validation_Layers_;
	
};

#endif