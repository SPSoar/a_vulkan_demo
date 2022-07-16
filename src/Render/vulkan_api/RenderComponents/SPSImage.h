#ifndef S_SPSIMAGE_H_
#define S_SPSIMAGE_H_

#include "SPComponent.h"

struct SPSMipmap
{
	/// Mipmap level
	uint32_t level_ = 0;

	/// Byte offset used for uploading
	uint32_t offset_ = 0;

	/// Width depth and height of the mipmap
	VkExtent3D extent_ = { 0, 0, 0 };
};

class SPImage;
class SPImageView;
class SPDevice;
class SPSImage : public SPComponent
{
public:
	SPSImage(const std::string& name, std::vector<uint8_t>&& data = {}, std::vector<SPSMipmap>&& mipmaps = { {} });
	virtual ~SPSImage() = default;
	SPSImage(SPSImage&& other) = default;
	virtual std::type_index get_type() override;
	static std::unique_ptr<SPSImage> load(const std::string& name, const std::string& uri);
public:
	void clear_data();
	const std::vector<uint8_t>& get_data() const;
	const SPImage& get_vk_image() const;
	const SPImageView& get_vk_image_view() const;
	const std::vector<SPSMipmap>& get_mipmaps() const;
	const uint32_t get_layers() const;
	const std::vector<std::vector<VkDeviceSize>>& get_offsets() const;
	VkFormat get_format() const;
	const VkExtent3D& get_extent() const;
	std::vector<SPSMipmap>& get_mut_mipmaps();
	std::vector<uint8_t>& get_mut_data();
	void generate_mipmaps();
	void create_vk_image(SPDevice & device, VkImageViewType image_view_type = VK_IMAGE_VIEW_TYPE_2D, VkImageCreateFlags flags = 0);

protected:
	void set_data(const uint8_t* raw_data, size_t size);
	void set_format(VkFormat format);
	void set_width(uint32_t width);
	void set_height(uint32_t height);
	void set_depth(uint32_t depth);
	void set_layers(uint32_t layers);
	void set_offsets(const std::vector<std::vector<VkDeviceSize>>& offsets);
private:
	std::vector<uint8_t> data_;
	VkFormat format_;
	std::vector<SPSMipmap> mipmaps_;;
	std::unique_ptr<SPImage> vk_image_;
	std::unique_ptr<SPImageView> vk_image_view_;
	uint32_t layers_{ 1 };
	std::vector<std::vector<VkDeviceSize>> offsets_;
};


class Stb : public SPSImage
{
public:
	Stb(const std::string& name, const std::vector<uint8_t>& data);

	virtual ~Stb() = default;
};

struct SPSBlockDim
{
	uint8_t x;
	uint8_t y;
	uint8_t z;
};

class SPSAstc : public SPSImage
{
public:
	SPSAstc(const SPSImage& image);
	SPSAstc(const std::string& name, const std::vector<uint8_t>& data);
	virtual ~SPSAstc() = default;
private:
	void decode(SPSBlockDim blockdim, VkExtent3D extent, const uint8_t * data);
	void init();

};


class SPSKtx : public SPSImage
{
public:
	SPSKtx(const std::string& name, const std::vector<uint8_t>& data);
	virtual ~SPSKtx() = default;
};

#endif