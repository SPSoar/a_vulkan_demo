#ifndef S_SPSNODE_H_
#define S_SPSNODE_H_

#include "SPSTransform.h"

class SPSNode
{
public:
	SPSNode(const size_t id, const std::string& name);
	virtual ~SPSNode() = default;
	SPSNode(SPSNode&& other) = default;
public:
	void set_component(SPComponent& component);
	void set_parent(SPSNode& parent);
	void add_child(SPSNode& child);
	SPSTransform& get_transform();

	template <class T>
	inline T& get_component()
	{
		return dynamic_cast<T&>(get_component(typeid(T)));
	}
	SPComponent& get_component(const std::type_index index);
	
private:
	std::unordered_map<std::type_index, SPComponent*> components_;
	size_t id_;
	std::string name_;

	SPSTransform transform_;
	SPSNode* parent_{ nullptr };
	std::vector<SPSNode*> children_;
};

#endif