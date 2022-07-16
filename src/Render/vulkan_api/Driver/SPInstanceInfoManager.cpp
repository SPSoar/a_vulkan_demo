#include "SPInstanceInfoManager.h"

std::unordered_map<const char*, bool> SPInstanceInfoManager::Instance_Extensions_ = {};
std::vector<const char*> SPInstanceInfoManager::Validation_Layers_ = {};



void SPInstanceInfoManager::Add_Instance_Extension(const char* extension, bool optional)
{
	Instance_Extensions_[extension] = optional;
}

void SPInstanceInfoManager::Add_Validation_Layers(const char* validation)
{
	Validation_Layers_.push_back(validation);
}

std::vector<const char*> SPInstanceInfoManager::Get_Optimal_Validation_Layers(const std::vector<VkLayerProperties>& supported_instance_layers)
{
	std::vector<std::vector<const char*>> validation_layer_priority_list =
	{
		// The preferred validation layer is "VK_LAYER_KHRONOS_validation"
		{"VK_LAYER_KHRONOS_validation"},

		// Otherwise we fallback to using the LunarG meta layer
		{"VK_LAYER_LUNARG_standard_validation"},

		// Otherwise we attempt to enable the individual layers that compose the LunarG meta layer since it doesn't exist
		{
			"VK_LAYER_GOOGLE_threading",
			"VK_LAYER_LUNARG_parameter_validation",
			"VK_LAYER_LUNARG_object_tracker",
			"VK_LAYER_LUNARG_core_validation",
			"VK_LAYER_GOOGLE_unique_objects",
		},

		// Otherwise as a last resort we fallback to attempting to enable the LunarG core layer
		{"VK_LAYER_LUNARG_core_validation"} };

	for (auto& validation_layers : validation_layer_priority_list)
	{
		if (Validate_Layers(validation_layers, supported_instance_layers))
		{
			return validation_layers;
		}

		SPWarning("Couldn't enable validation layers (see log for error) - falling back");
	}

	// Else return nothing
	return {};
}


bool SPInstanceInfoManager::Validate_Layers(const std::vector<const char*>& required,
	const std::vector<VkLayerProperties>& available)
{
	for (auto layer : required)
	{
		bool found = false;
		for (auto& available_layer : available)
		{
			if (strcmp(available_layer.layerName, layer) == 0)
			{
				found = true;
				break;
			}
		}

		if (!found)
		{
			SPError("Validation Layer " << layer << " not found");
			return false;
		}
	}

	return true;
}