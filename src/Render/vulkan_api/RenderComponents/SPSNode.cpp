#include "SPSNode.h"



SPSNode::SPSNode(const size_t id, const std::string& name)
	: id_{ id }
	, name_{ name }
	, transform_{ *this }
{
}

void SPSNode::set_component(SPComponent& component)
{
	auto it = components_.find(component.get_type());

	if (it != components_.end())
	{
		it->second = &component;
	}
	else
	{
		components_.insert(std::make_pair(component.get_type(), &component));
	}
}

void SPSNode::set_parent(SPSNode& parent)
{
	parent_ = &parent;

	transform_.invalidate_world_matrix();
}

void SPSNode::add_child(SPSNode& child)
{
	children_.push_back(&child);
}

SPSTransform& SPSNode::get_transform()
{
	return transform_;
}

SPComponent& SPSNode::get_component(const std::type_index index)
{
	return *components_.at(index);
}
