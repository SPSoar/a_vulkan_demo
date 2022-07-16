#include "SPSImage.h"
#include "../RenderContext/SPImage.h"
#include "../RenderContext/SPImageView.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb/stb_image_resize.h>

#include <astc/Source/astc_codec_internals.h>


VKBP_DISABLE_WARNINGS()
#include <ktx.h>
#include <ktxvulkan.h>
VKBP_ENABLE_WARNINGS()

SPSBlockDim to_blockdim(const VkFormat format)
{
	switch (format)
	{
	case VK_FORMAT_ASTC_4x4_UNORM_BLOCK:
	case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
		return { 4, 4, 1 };
	case VK_FORMAT_ASTC_5x4_UNORM_BLOCK:
	case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
		return { 5, 4, 1 };
	case VK_FORMAT_ASTC_5x5_UNORM_BLOCK:
	case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
		return { 5, 5, 1 };
	case VK_FORMAT_ASTC_6x5_UNORM_BLOCK:
	case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
		return { 6, 5, 1 };
	case VK_FORMAT_ASTC_6x6_UNORM_BLOCK:
	case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
		return { 6, 6, 1 };
	case VK_FORMAT_ASTC_8x5_UNORM_BLOCK:
	case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
		return { 8, 5, 1 };
	case VK_FORMAT_ASTC_8x6_UNORM_BLOCK:
	case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
		return { 8, 6, 1 };
	case VK_FORMAT_ASTC_8x8_UNORM_BLOCK:
	case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
		return { 8, 8, 1 };
	case VK_FORMAT_ASTC_10x5_UNORM_BLOCK:
	case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
		return { 10, 5, 1 };
	case VK_FORMAT_ASTC_10x6_UNORM_BLOCK:
	case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
		return { 10, 6, 1 };
	case VK_FORMAT_ASTC_10x8_UNORM_BLOCK:
	case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
		return { 10, 8, 1 };
	case VK_FORMAT_ASTC_10x10_UNORM_BLOCK:
	case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
		return { 10, 10, 1 };
	case VK_FORMAT_ASTC_12x10_UNORM_BLOCK:
	case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
		return { 12, 10, 1 };
	case VK_FORMAT_ASTC_12x12_UNORM_BLOCK:
	case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
		return { 12, 12, 1 };
	default:
		throw std::runtime_error{ "Invalid astc format" };
	}
}

struct SPSAstcHeader
{
	uint8_t magic_[4];
	uint8_t blockdim_x_;
	uint8_t blockdim_y_;
	uint8_t blockdim_z_;
	uint8_t xsize_[3];        // x-size = xsize[0] + xsize[1] + xsize[2]
	uint8_t ysize_[3];        // x-size, y-size and z-size are given in texels;
	uint8_t zsize_[3];        // block count is inferred
};

#define MAGIC_FILE_CONSTANT 0x5CA1AB13

static ktx_error_code_e KTX_APIENTRY optimal_tiling_callback(int          mip_level,
	int          face,
	int          width,
	int          height,
	int          depth,
	ktx_uint64_t face_lod_size,
	void* pixels,
	void* user_data)
{
	// Get mipmaps
	auto& mipmaps = *reinterpret_cast<std::vector<SPSMipmap> *>(user_data);
	assert(static_cast<size_t>(mip_level) < mipmaps.size() && "Not enough space in the mipmap vector");

	auto& mipmap = mipmaps.at(mip_level);
	mipmap.level_ = mip_level;
	mipmap.extent_.width = width;
	mipmap.extent_.height = height;
	mipmap.extent_.depth = depth;

	// Set offset for the next mip level
	auto next_mip_level = static_cast<size_t>(mip_level + 1);
	if (next_mip_level < mipmaps.size())
	{
		mipmaps.at(next_mip_level).offset_ = mipmap.offset_ + static_cast<uint32_t>(face_lod_size);
	}

	return KTX_SUCCESS;
}


SPSImage::SPSImage(const std::string& name, std::vector<uint8_t>&& data, std::vector<SPSMipmap>&& mipmaps)
	: SPComponent{ name }
	, data_{ std::move(data) }
	, format_{ VK_FORMAT_R8G8B8A8_UNORM }
	, mipmaps_{ std::move(mipmaps) }
{
}

std::type_index SPSImage::get_type()
{
	return typeid(SPSImage);
}

std::unique_ptr<SPSImage> SPSImage::load(const std::string& name, const std::string& uri)
{
	std::unique_ptr<SPSImage> image{ nullptr };

	auto data = vkTool::read_binary_file(uri, 0);

	// Get extension
	auto extension = vkTool::get_extension(uri);

	if (extension == "png" || extension == "jpg")
	{
		image = std::make_unique<Stb>(name, data);
	}
	else if (extension == "astc")
	{
		image = std::make_unique<SPSAstc>(name, data);
	}
	else if (extension == "ktx")
	{
		image = std::make_unique<SPSKtx>(name, data);
	}
	else if (extension == "ktx2")
	{
		image = std::make_unique<SPSKtx>(name, data);
	}

	return image;
}

void SPSImage::clear_data()
{
	data_.clear();
	data_.shrink_to_fit();
}

const std::vector<uint8_t>& SPSImage::get_data() const
{
	return data_;
}

const SPImage& SPSImage::get_vk_image() const
{
	assert(vk_image_ && "Vulkan image was not created");
	return *vk_image_;
}

const SPImageView& SPSImage::get_vk_image_view() const
{
	assert(vk_image_view_ && "Vulkan image view was not created");
	return *vk_image_view_;
}

const std::vector<SPSMipmap>& SPSImage::get_mipmaps() const
{
	return mipmaps_;
}

const uint32_t SPSImage::get_layers() const
{
	return layers_;
}

const std::vector<std::vector<VkDeviceSize>>& SPSImage::get_offsets() const
{
	return offsets_;
}

VkFormat SPSImage::get_format() const
{
	return format_;
}

const VkExtent3D& SPSImage::get_extent() const
{
	return mipmaps_.at(0).extent_;
}

std::vector<SPSMipmap>& SPSImage::get_mut_mipmaps()
{
	return mipmaps_;
}

std::vector<uint8_t>& SPSImage::get_mut_data()
{
	return data_;
}

void SPSImage::generate_mipmaps()
{
	assert(mipmaps_.size() == 1 && "Mipmaps already generated");

	if (mipmaps_.size() > 1)
	{
		return;        // Do not generate again
	}

	auto extent = get_extent();
	auto next_width = std::max<uint32_t>(1u, extent.width / 2);
	auto next_height = std::max<uint32_t>(1u, extent.height / 2);
	auto channels = 4;
	auto next_size = next_width * next_height * channels;

	while (true)
	{
		// Make space for next mipmap
		auto old_size = sp::to_u32(data_.size());
		data_.resize(old_size + next_size);

		auto& prev_mipmap = mipmaps_.back();
		// Update mipmaps
		SPSMipmap next_mipmap{};
		next_mipmap.level_ = prev_mipmap.level_ + 1;
		next_mipmap.offset_ = old_size;
		next_mipmap.extent_ = { next_width, next_height, 1u };

		// Fill next mipmap memory
		stbir_resize_uint8(data_.data() + prev_mipmap.offset_, prev_mipmap.extent_.width, prev_mipmap.extent_.height, 0,
			data_.data() + next_mipmap.offset_, next_mipmap.extent_.width, next_mipmap.extent_.height, 0, channels);

		mipmaps_.emplace_back(std::move(next_mipmap));

		// Next mipmap values
		next_width = std::max<uint32_t>(1u, next_width / 2);
		next_height = std::max<uint32_t>(1u, next_height / 2);
		next_size = next_width * next_height * channels;

		if (next_width == 1 && next_height == 1)
		{
			break;
		}
	}
}

void SPSImage::create_vk_image(SPDevice & device, VkImageViewType image_view_type, VkImageCreateFlags flags)
{
	assert(!vk_image_ && !vk_image_view_ && "Vulkan image already constructed");

	vk_image_ = std::make_unique<SPImage>(device,
		get_extent(),
		format_,
		VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
		VMA_MEMORY_USAGE_GPU_ONLY,
		VK_SAMPLE_COUNT_1_BIT,
		sp::to_u32(mipmaps_.size()),
		layers_,
		VK_IMAGE_TILING_OPTIMAL,
		flags);

	vk_image_view_ = std::make_unique<SPImageView>(*vk_image_, image_view_type);
}

void SPSImage::set_data(const uint8_t* raw_data, size_t size)
{
	assert(data_.empty() && "Image data already set");
	data_ = { raw_data, raw_data + size };
}

void SPSImage::set_format(VkFormat format)
{
	format_ = format;
}

void SPSImage::set_width(uint32_t width)
{
	mipmaps_.at(0).extent_.width = width;
}

void SPSImage::set_height(uint32_t height)
{
	mipmaps_.at(0).extent_.height = height;
}

void SPSImage::set_depth(uint32_t depth)
{
	mipmaps_.at(0).extent_.depth = depth;
}

void SPSImage::set_layers(uint32_t layers)
{
	layers_ = layers;
}

void SPSImage::set_offsets(const std::vector<std::vector<VkDeviceSize>>& offsets)
{
	offsets_ = offsets;
}

Stb::Stb(const std::string& name, const std::vector<uint8_t>& data)
	: SPSImage{ name }
{
	int width;
	int height;
	int comp;
	int req_comp = 4;

	auto data_buffer = reinterpret_cast<const stbi_uc*>(data.data());
	auto data_size = static_cast<int>(data.size());

	auto raw_data = stbi_load_from_memory(data_buffer, data_size, &width, &height, &comp, req_comp);

	if (!raw_data)
	{
		throw std::runtime_error{ "Failed to load " + name + ": " + stbi_failure_reason() };
	}

	set_data(raw_data, width * height * req_comp);
	stbi_image_free(raw_data);

	set_format(VK_FORMAT_R8G8B8A8_UNORM);
	set_width(sp::to_u32(width));
	set_height(sp::to_u32(height));
	set_depth(1u);
}

SPSAstc::SPSAstc(const SPSImage& image)
	: SPSImage{ image.get_name() }
{
	init();
	decode(to_blockdim(image.get_format()), image.get_extent(), image.get_data().data());
}

SPSAstc::SPSAstc(const std::string& name, const std::vector<uint8_t>& data)
	: SPSImage{ name }
{
	init();

	// Read header
	if (data.size() < sizeof(SPSAstcHeader))
	{
		throw std::runtime_error{ "Error reading astc: invalid memory" };
	}
	SPSAstcHeader header{};
	std::memcpy(&header, data.data(), sizeof(SPSAstcHeader));
	uint32_t magicval = header.magic_[0] + 256 * static_cast<uint32_t>(header.magic_[1]) + 65536 * static_cast<uint32_t>(header.magic_[2]) + 16777216 * static_cast<uint32_t>(header.magic_[3]);
	if (magicval != MAGIC_FILE_CONSTANT)
	{
		throw std::runtime_error{ "Error reading astc: invalid magic" };
	}

	SPSBlockDim blockdim = {
		/* xdim = */ header.blockdim_x_,
		/* ydim = */ header.blockdim_y_,
		/* zdim = */ header.blockdim_z_ };

	VkExtent3D extent = {
		/* width  = */ static_cast<uint32_t>(header.xsize_[0] + 256 * header.xsize_[1] + 65536 * header.xsize_[2]),
		/* height = */ static_cast<uint32_t>(header.ysize_[0] + 256 * header.ysize_[1] + 65536 * header.ysize_[2]),
		/* depth  = */ static_cast<uint32_t>(header.zsize_[0] + 256 * header.zsize_[1] + 65536 * header.zsize_[2]) };

	decode(blockdim, extent, data.data() + sizeof(SPSAstcHeader));
}

void SPSAstc::decode(SPSBlockDim blockdim, VkExtent3D extent, const uint8_t* data)
{
	// Actual decoding
	astc_decode_mode decode_mode = DECODE_LDR_SRGB;
	uint32_t         bitness = 8;
	swizzlepattern   swz_decode = { 0, 1, 2, 3 };

	int xdim = blockdim.x;
	int ydim = blockdim.y;
	int zdim = blockdim.z;

	if ((xdim < 3 || xdim > 6 || ydim < 3 || ydim > 6 || zdim < 3 || zdim > 6) &&
		(xdim < 4 || xdim == 7 || xdim == 9 || xdim == 11 || xdim > 12 ||
			ydim < 4 || ydim == 7 || ydim == 9 || ydim == 11 || ydim > 12 || zdim != 1))
	{
		throw std::runtime_error{ "Error reading astc: invalid block" };
	}

	int xsize = extent.width;
	int ysize = extent.height;
	int zsize = extent.depth;

	if (xsize == 0 || ysize == 0 || zsize == 0)
	{
		throw std::runtime_error{ "Error reading astc: invalid size" };
	}

	int xblocks = (xsize + xdim - 1) / xdim;
	int yblocks = (ysize + ydim - 1) / ydim;
	int zblocks = (zsize + zdim - 1) / zdim;

	auto astc_image = allocate_image(bitness, xsize, ysize, zsize, 0);
	initialize_image(astc_image);

	imageblock pb;
	for (int z = 0; z < zblocks; z++)
	{
		for (int y = 0; y < yblocks; y++)
		{
			for (int x = 0; x < xblocks; x++)
			{
				int            offset = (((z * yblocks + y) * xblocks) + x) * 16;
				const uint8_t* bp = data + offset;

				physical_compressed_block pcb = *reinterpret_cast<const physical_compressed_block*>(bp);
				symbolic_compressed_block scb;

				physical_to_symbolic(xdim, ydim, zdim, pcb, &scb);
				decompress_symbolic_block(decode_mode, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, &scb, &pb);
				write_imageblock(astc_image, &pb, xdim, ydim, zdim, x * xdim, y * ydim, z * zdim, swz_decode);
			}
		}
	}

	set_data(astc_image->imagedata8[0][0], astc_image->xsize * astc_image->ysize * astc_image->zsize * 4);
	set_format(VK_FORMAT_R8G8B8A8_SRGB);
	set_width(static_cast<uint32_t>(astc_image->xsize));
	set_height(static_cast<uint32_t>(astc_image->ysize));
	set_depth(static_cast<uint32_t>(astc_image->zsize));

	destroy_image(astc_image);
}

void SPSAstc::init()
{
	// Initializes ASTC library
	static bool                  initialized{ false };
	static std::mutex            initialization;
	std::unique_lock<std::mutex> lock{ initialization };
	if (!initialized)
	{
		// Init stuff
		prepare_angular_tables();
		build_quantization_mode_table();
		initialized = true;
	}
}

SPSKtx::SPSKtx(const std::string& name, const std::vector<uint8_t>& data)
	: SPSImage{ name }
{
	auto data_buffer = reinterpret_cast<const ktx_uint8_t*>(data.data());
	auto data_size = static_cast<ktx_size_t>(data.size());

	ktxTexture* texture;
	auto        load_ktx_result = ktxTexture_CreateFromMemory(data_buffer,
		data_size,
		KTX_TEXTURE_CREATE_NO_FLAGS,
		&texture);
	if (load_ktx_result != KTX_SUCCESS)
	{
		throw std::runtime_error{ "Error loading KTX texture: " + name };
	}

	if (texture->pData)
	{
		// Already loaded
		set_data(texture->pData, texture->dataSize);
	}
	else
	{
		// Load
		auto& mut_data = get_mut_data();
		auto  size = texture->dataSize;
		mut_data.resize(size);
		auto load_data_result = ktxTexture_LoadImageData(texture, mut_data.data(), size);
		if (load_data_result != KTX_SUCCESS)
		{
			throw std::runtime_error{ "Error loading KTX image data: " + name };
		}
	}

	// Update width and height
	set_width(texture->baseWidth);
	set_height(texture->baseHeight);
	set_depth(texture->baseDepth);
	set_layers(texture->numLayers);

	bool cubemap = false;

	// Use the faces if there are 6 (for cubemap)
	if (texture->numLayers == 1 && texture->numFaces == 6)
	{
		cubemap = true;
		set_layers(texture->numFaces);
	}

	// Update format
	auto updated_format = ktxTexture_GetVkFormat(texture);
	set_format(updated_format);

	// Update mip levels
	auto& mipmap_levels = get_mut_mipmaps();
	mipmap_levels.resize(texture->numLevels);
	auto result = ktxTexture_IterateLevels(texture, optimal_tiling_callback, &mipmap_levels);
	if (result != KTX_SUCCESS)
	{
		throw std::runtime_error("Error loading KTX texture");
	}

	// If the texture contains more than one layer, then populate the offsets otherwise take the mipmap level offsets
	if (texture->numLayers > 1 || cubemap)
	{
		uint32_t layer_count = cubemap ? texture->numFaces : texture->numLayers;

		std::vector<std::vector<VkDeviceSize>> offsets;
		for (uint32_t layer = 0; layer < layer_count; layer++)
		{
			std::vector<VkDeviceSize> layer_offsets{};
			for (uint32_t level = 0; level < texture->numLevels; level++)
			{
				ktx_size_t     offset;
				KTX_error_code result;
				if (cubemap)
				{
					result = ktxTexture_GetImageOffset(texture, level, 0, layer, &offset);
				}
				else
				{
					result = ktxTexture_GetImageOffset(texture, level, layer, 0, &offset);
				}
				layer_offsets.push_back(static_cast<VkDeviceSize>(offset));
			}
			offsets.push_back(layer_offsets);
		}
		set_offsets(offsets);
	}
	else
	{
		std::vector<std::vector<VkDeviceSize>> offsets{};
		offsets.resize(1);
		for (size_t level = 0; level < mipmap_levels.size(); level++)
		{
			offsets[0].push_back(static_cast<VkDeviceSize>(mipmap_levels[level].offset_));
		}
		set_offsets(offsets);
	}

	ktxTexture_Destroy(texture);
}
