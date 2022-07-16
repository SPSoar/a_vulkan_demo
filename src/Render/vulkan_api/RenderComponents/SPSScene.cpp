#include "SPSScene.h"

#include "../RenderComponents/SPSNode.h"

SPSScene::SPSScene()
	: name_{""}
	, nodes_{ }
	, root_{ nullptr }
	, components_{}
{
}

const std::vector<std::unique_ptr<SPComponent>>& SPSScene::get_components(const std::type_index& type_info) const
{
	return components_.at(type_info);
}

void SPSScene::add_component(std::unique_ptr<SPComponent>&& component)
{
	if (component)
	{
		components_[component->get_type()].push_back(std::move(component));
	}
}

void SPSScene::add_component(std::unique_ptr<SPComponent>&& component, SPSNode& node)
{
	node.set_component(*component);

	if (component)
	{
		components_[component->get_type()].push_back(std::move(component));
	}
}

void SPSScene::set_components(const std::type_index& type_info, std::vector<std::unique_ptr<SPComponent>>&& components)
{
	components_[type_info] = std::move(components);
}

bool SPSScene::has_component(const std::type_index& type_info) const
{
	auto component = components_.find(type_info);
	return (component != components_.end() && !component->second.empty());
}

void SPSScene::set_root_node(SPSNode& node)
{
	root_ = &node;
}

void SPSScene::set_nodes(std::vector<std::unique_ptr<SPSNode>>&& nodes)
{
	assert(nodes_.empty() && "Scene nodes were already set");
	nodes_ = std::move(nodes);
}

void SPSScene::add_node(std::unique_ptr<SPSNode>&& node)
{
	nodes_.emplace_back(std::move(node));
}

SPSNode& SPSScene::get_root_node()
{
	return *root_;
}

void SPSScene::add_child(SPSNode& child)
{
	root_->add_child(child);
}
