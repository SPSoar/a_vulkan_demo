#ifndef S_SPSSCENE_H_
#define S_SPSSCENE_H_

#include "SPComponent.h"

#include "SPSNode.h"

class SPSScene
{
public:
	SPSScene();
	virtual ~SPSScene() = default;
	SPSScene(SPSScene&& other) = default;
public:
	template <class T>
	void set_components(std::vector<std::unique_ptr<T>>&& components)
	{
		std::vector<std::unique_ptr<SPComponent>> result(components.size());
		std::transform(components.begin(), components.end(), result.begin(),
			[](std::unique_ptr<T>& component) -> std::unique_ptr<SPComponent> {
				return std::unique_ptr<SPComponent>(std::move(component));
			});
		set_components(typeid(T), std::move(result));
	}

	template <class T>
	std::vector<T*> get_components() const
	{
		std::vector<T*> result;
		if (has_component(typeid(T)))
		{
			auto& scene_components = get_components(typeid(T));

			result.resize(scene_components.size());
			std::transform(scene_components.begin(), scene_components.end(), result.begin(),
				[](const std::unique_ptr<SPComponent>& component) -> T* {
					return dynamic_cast<T*>(component.get());
				});
		}

		return result;
	}
	const std::vector<std::unique_ptr<SPComponent>>& get_components(const std::type_index& type_info) const;

	void add_component(std::unique_ptr<SPComponent>&& component);
	void add_component(std::unique_ptr<SPComponent>&& component, SPSNode& node);
	void set_components(const std::type_index& type_info, std::vector<std::unique_ptr<SPComponent>>&& components);

	template <class T>
	bool has_component() const
	{
		return has_component(typeid(T));
	}
	bool has_component(const std::type_index& type_info) const;
	void set_root_node(SPSNode& node);
	void set_nodes(std::vector<std::unique_ptr<SPSNode>>&& nodes);
	void add_node(std::unique_ptr<SPSNode>&& node);
	SPSNode& get_root_node();

	void add_child(SPSNode& child);
private:
	std::string name_;
	std::vector<std::unique_ptr<SPSNode>> nodes_;
	SPSNode* root_;
	std::unordered_map<std::type_index, std::vector<std::unique_ptr<SPComponent>>> components_;
};

#endif