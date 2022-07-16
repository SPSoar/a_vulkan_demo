#include "SPComponentTool.h"

SPSLight& SPComponentTool::add_directional_light(SPSScene& scene, const glm::quat& rotation, const SPSLightProperties& props, SPSNode* parent_node)
{
	return add_light(scene, SPSLightType::Directional, {}, rotation, props, parent_node);
}

SPSLight& SPComponentTool::add_light(SPSScene& scene, SPSLightType type, const glm::vec3& position, const glm::quat& rotation, const SPSLightProperties& props, SPSNode* parent_node)
{
	auto light_ptr = std::make_unique<SPSLight>("light");
	auto node = std::make_unique<SPSNode>(-1, "light node");
	
	if (parent_node)
	{
		node->set_parent(*parent_node);
	}
	
	light_ptr->set_node(*node);
	light_ptr->set_light_type(type);
	light_ptr->set_properties(props);
	
	auto& t = node->get_transform();
	t.set_translation(position);
	t.set_rotation(rotation);
	
	// Storing the light component because the unique_ptr will be moved to the scene
	auto& light = *light_ptr;
	
	node->set_component(light);
	scene.add_child(*node);
	scene.add_component(std::move(light_ptr));
	scene.add_node(std::move(node));
	
	return light;
}
