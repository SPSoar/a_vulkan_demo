#ifndef S_SPRESOURCEBINDINGSTATE_H_
#define S_SPRESOURCEBINDINGSTATE_H_

#include "../vulkan_common.h"

class SPBuffer;
class SPImageView;
class SPSampler;

struct SPResourceInfo
{
	bool dirty_{ false };

	const SPBuffer* buffer_{ nullptr };

	VkDeviceSize offset_{ 0 };

	VkDeviceSize range_{ 0 };

	const SPImageView* image_view_{ nullptr };

	const SPSampler* sampler_{ nullptr };
};


class SPResourceSet
{
public:
	void reset();

	bool is_dirty() const;

	void clear_dirty();

	void clear_dirty(uint32_t binding, uint32_t array_element);

	void bind_buffer(const SPBuffer& buffer, VkDeviceSize offset, VkDeviceSize range, uint32_t binding, uint32_t array_element);

	void bind_image(const SPImageView& image_view, const SPSampler& sampler, uint32_t binding, uint32_t array_element);

	void bind_image(const SPImageView& image_view, uint32_t binding, uint32_t array_element);

	void bind_input(const SPImageView& image_view, uint32_t binding, uint32_t array_element);

	const SPBindingMap<SPResourceInfo>& get_resource_bindings() const;

private:
	bool dirty_{ false };

	SPBindingMap<SPResourceSet> resource_bindings_;
};

class SPResourceBindingState
{
public:
	void reset();
	void clear_dirty();
	void clear_dirty(uint32_t set);

private:
	bool dirty_{ false };

	std::unordered_map<uint32_t, SPResourceSet> resource_sets_;
};

#endif