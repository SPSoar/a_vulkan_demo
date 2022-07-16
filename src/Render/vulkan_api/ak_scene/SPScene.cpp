#include "SPScene.h"
#include "../ak_engine/SPEngine.h"
#include "../Driver/SPDevice.h"

#include "../Buffer/SPBuffer.h"
#include "../RenderContext/SPImage.h"
#include "../RenderContext/SPSampler.h"
#include "../Command/SPQueue.h"
#include <random>


#include "../vk_tool/gltf_loader.h"


VKBP_DISABLE_WARNINGS()
#include <ktx.h>
#include <ktxvulkan.h>
VKBP_ENABLE_WARNINGS()

void* SPScene::aligned_alloc(uint64_t size, uint64_t alignment)
{
	void* data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
	data = _aligned_malloc(size, alignment);
#else
	int res = posix_memalign(&data, alignment, size);
	if (res != 0)
		data = nullptr;
#endif
	return data;
}

void SPScene::aligned_free(void* data)
{
#if defined(_MSC_VER) || defined(__MINGW32__)
	_aligned_free(data);
#else
	free(data);
#endif
}

SPScene::SPScene(SPEngine& engine)
	: engine_{ engine}
	, camera_{ "main_camera" }
{
}

SPScene::~SPScene()
{

}

void SPScene::init()
{
	
}

void SPScene::create_camera(uint16_t width, uint16_t height)
{
	spv_vec3 target{ 0.0, 0.0, 0.0 };
	spv_vec3 eye{0.0, 0.0, 2.0};
	spv_vec3 up{ 0, 1, 0 };
	camera_.look_at(eye, target, up);
	camera_.set_projection(60.0, (spd_decimal)width / (spd_decimal)height, 0.1, 256.0, Fov::HORIZONTAL);
}

std::unique_ptr<SPSSubMesh> SPScene::load_model(const std::string& file, uint32_t index)
{
	vkb::GLTFLoader loader{ engine_.get_device() };

	std::unique_ptr<SPSSubMesh> model = loader.read_model_from_file(file, index);

	if (!model)
	{
		//LOGE("Cannot load model from file: {}", file.c_str());
		throw std::runtime_error("Cannot load model from: " + file);
	}

	return model;
}

STexture SPScene::load_texture(const std::string& file)
{
	STexture texture{};

	texture.image = SPSImage::load(file, file);
	texture.image->create_vk_image(engine_.get_device());

	const auto& queue = engine_.get_device().get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);

	VkCommandBuffer command_buffer = engine_.get_device().create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

	SPBuffer stage_buffer{ engine_.get_device(),
								   texture.image->get_data().size(),
								   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								   VMA_MEMORY_USAGE_CPU_ONLY };

	stage_buffer.update(texture.image->get_data());

	// Setup buffer copy regions for each mip level
	std::vector<VkBufferImageCopy> bufferCopyRegions;

	auto& mipmaps = texture.image->get_mipmaps();

	for (size_t i = 0; i < mipmaps.size(); i++)
	{
		VkBufferImageCopy buffer_copy_region = {};
		buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		buffer_copy_region.imageSubresource.mipLevel = sp::to_u32(i);
		buffer_copy_region.imageSubresource.baseArrayLayer = 0;
		buffer_copy_region.imageSubresource.layerCount = 1;
		buffer_copy_region.imageExtent.width = texture.image->get_extent().width >> i;
		buffer_copy_region.imageExtent.height = texture.image->get_extent().height >> i;
		buffer_copy_region.imageExtent.depth = 1;
		buffer_copy_region.bufferOffset = mipmaps[i].offset_;

		bufferCopyRegions.push_back(buffer_copy_region);
	}

	VkImageSubresourceRange subresource_range = {};
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = sp::to_u32(mipmaps.size());
	subresource_range.layerCount = 1;

	// Image barrier for optimal image (target)
	// Optimal image will be used as destination for the copy
	vkTool::set_image_layout(
		command_buffer,
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		subresource_range);

	// Copy mip levels from staging buffer
	vkCmdCopyBufferToImage(
		command_buffer,
		stage_buffer.get_handle(),
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		static_cast<uint32_t>(bufferCopyRegions.size()),
		bufferCopyRegions.data());

	// Change texture image layout to shader read after all mip levels have been copied
	vkTool::set_image_layout(
		command_buffer,
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		subresource_range);

	engine_.get_device().flush_command_buffer(command_buffer, queue.get_handle());

	// Create a defaultsampler
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.magFilter = VK_FILTER_LINEAR;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.compareOp = VK_COMPARE_OP_NEVER;
	sampler_create_info.minLod = 0.0f;
	// Max level-of-detail should match mip level count
	sampler_create_info.maxLod = static_cast<float>(mipmaps.size());
	// Only enable anisotropic filtering if enabled on the device
	// Note that for simplicity, we will always be using max. available anisotropy level for the current device
	// This may have an impact on performance, esp. on lower-specced devices
	// In a real-world scenario the level of anisotropy should be a user setting or e.g. lowered for mobile devices by default
	sampler_create_info.maxAnisotropy = engine_.get_device().get_gpu().get_features().samplerAnisotropy ? (engine_.get_device().get_gpu().get_properties().limits.maxSamplerAnisotropy) : 1.0f;
	sampler_create_info.anisotropyEnable = engine_.get_device().get_gpu().get_features().samplerAnisotropy;
	sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	VK_CHECK(vkCreateSampler(engine_.get_device().get_handle(), &sampler_create_info, nullptr, &texture.sampler));

	return texture;
}

STexture SPScene::load_texture_array(const std::string& file)
{
	STexture texture{};

	texture.image = SPSImage::load(file, file);
	texture.image->create_vk_image(engine_.get_device(), VK_IMAGE_VIEW_TYPE_2D_ARRAY);

	const auto& queue = engine_.get_device().get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);

	VkCommandBuffer command_buffer = engine_.get_device().create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

	SPBuffer stage_buffer{ engine_.get_device(),
								   texture.image->get_data().size(),
								   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
								   VMA_MEMORY_USAGE_CPU_ONLY };

	stage_buffer.update(texture.image->get_data());

	// Setup buffer copy regions for each mip level
	std::vector<VkBufferImageCopy> buffer_copy_regions;

	auto& mipmaps = texture.image->get_mipmaps();
	const auto& layers = texture.image->get_layers();

	auto& offsets = texture.image->get_offsets();

	for (uint32_t layer = 0; layer < layers; layer++)
	{
		for (size_t i = 0; i < mipmaps.size(); i++)
		{
			VkBufferImageCopy buffer_copy_region = {};
			buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			buffer_copy_region.imageSubresource.mipLevel = sp::to_u32(i);
			buffer_copy_region.imageSubresource.baseArrayLayer = layer;
			buffer_copy_region.imageSubresource.layerCount = 1;
			buffer_copy_region.imageExtent.width = texture.image->get_extent().width >> i;
			buffer_copy_region.imageExtent.height = texture.image->get_extent().height >> i;
			buffer_copy_region.imageExtent.depth = 1;
			buffer_copy_region.bufferOffset = offsets[layer][i];

			buffer_copy_regions.push_back(buffer_copy_region);
		}
	}

	VkImageSubresourceRange subresource_range = {};
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = sp::to_u32(mipmaps.size());
	subresource_range.layerCount = layers;

	// Image barrier for optimal image (target)
	// Optimal image will be used as destination for the copy
	vkTool::set_image_layout(
		command_buffer,
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		subresource_range);

	// Copy mip levels from staging buffer
	vkCmdCopyBufferToImage(
		command_buffer,
		stage_buffer.get_handle(),
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		static_cast<uint32_t>(buffer_copy_regions.size()),
		buffer_copy_regions.data());

	// Change texture image layout to shader read after all mip levels have been copied
	vkTool::set_image_layout(
		command_buffer,
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		subresource_range);

	engine_.get_device().flush_command_buffer(command_buffer, queue.get_handle());

	// Create a defaultsampler
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.magFilter = VK_FILTER_LINEAR;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.compareOp = VK_COMPARE_OP_NEVER;
	sampler_create_info.minLod = 0.0f;
	// Max level-of-detail should match mip level count
	sampler_create_info.maxLod = static_cast<float>(mipmaps.size());
	// Only enable anisotropic filtering if enabled on the devicec
	sampler_create_info.maxAnisotropy = engine_.get_device().get_gpu().get_features().samplerAnisotropy ? engine_.get_device().get_gpu().get_properties().limits.maxSamplerAnisotropy : 1.0f;
	sampler_create_info.anisotropyEnable = engine_.get_device().get_gpu().get_features().samplerAnisotropy;
	sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	VK_CHECK(vkCreateSampler(engine_.get_device().get_handle(), &sampler_create_info, nullptr, &texture.sampler));

	return texture;
}

STexture SPScene::load_texture_cubemap(const std::string& file)
{
	STexture texture{};

	texture.image = SPSImage::load(file, file);
	texture.image->create_vk_image(engine_.get_device(), VK_IMAGE_VIEW_TYPE_CUBE, VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT);

	const auto& queue = engine_.get_device().get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);

	VkCommandBuffer command_buffer = engine_.get_device().create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);

	SPBuffer stage_buffer{ engine_.get_device(),
									texture.image->get_data().size(),
									VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
									VMA_MEMORY_USAGE_CPU_ONLY };

	stage_buffer.update(texture.image->get_data());

	// Setup buffer copy regions for each mip level
	std::vector<VkBufferImageCopy> buffer_copy_regions;

	auto& mipmaps = texture.image->get_mipmaps();
	const auto& layers = texture.image->get_layers();

	auto& offsets = texture.image->get_offsets();

	for (uint32_t layer = 0; layer < layers; layer++)
	{
		for (size_t i = 0; i < mipmaps.size(); i++)
		{
			VkBufferImageCopy buffer_copy_region = {};
			buffer_copy_region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			buffer_copy_region.imageSubresource.mipLevel = sp::to_u32(i);
			buffer_copy_region.imageSubresource.baseArrayLayer = layer;
			buffer_copy_region.imageSubresource.layerCount = 1;
			buffer_copy_region.imageExtent.width = texture.image->get_extent().width >> i;
			buffer_copy_region.imageExtent.height = texture.image->get_extent().height >> i;
			buffer_copy_region.imageExtent.depth = 1;
			buffer_copy_region.bufferOffset = offsets[layer][i];

			buffer_copy_regions.push_back(buffer_copy_region);
		}
	}

	VkImageSubresourceRange subresource_range = {};
	subresource_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	subresource_range.baseMipLevel = 0;
	subresource_range.levelCount = sp::to_u32(mipmaps.size());
	subresource_range.layerCount = layers;

	// Image barrier for optimal image (target)
	// Optimal image will be used as destination for the copy
	vkTool::set_image_layout(
		command_buffer,
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		subresource_range);

	// Copy mip levels from staging buffer
	vkCmdCopyBufferToImage(
		command_buffer,
		stage_buffer.get_handle(),
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		static_cast<uint32_t>(buffer_copy_regions.size()),
		buffer_copy_regions.data());

	// Change texture image layout to shader read after all mip levels have been copied
	vkTool::set_image_layout(
		command_buffer,
		texture.image->get_vk_image().get_handle(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		subresource_range);

	engine_.get_device().flush_command_buffer(command_buffer, queue.get_handle());

	// Create a defaultsampler
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.magFilter = VK_FILTER_LINEAR;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.compareOp = VK_COMPARE_OP_NEVER;
	sampler_create_info.minLod = 0.0f;
	// Max level-of-detail should match mip level count
	sampler_create_info.maxLod = static_cast<float>(mipmaps.size());
	// Only enable anisotropic filtering if enabled on the devicec
	sampler_create_info.maxAnisotropy = engine_.get_device().get_gpu().get_features().samplerAnisotropy ? engine_.get_device().get_gpu().get_properties().limits.maxSamplerAnisotropy : 1.0f;
	sampler_create_info.anisotropyEnable = engine_.get_device().get_gpu().get_features().samplerAnisotropy;
	sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
	VK_CHECK(vkCreateSampler(engine_.get_device().get_handle(), &sampler_create_info, nullptr, &texture.sampler));

	return texture;
}

void SPScene::create_scene()
{
	create_bottom_level_acceleration_structure();
	create_top_level_acceleration_structure();
}

void SPScene::create_bottom_level_acceleration_structure()
{
	// Setup vertices and indices for a single triangle

	std::vector<Vertex> vertices = {
		{{1.0f, 1.0f, 0.0f}},
		{{-1.0f, 1.0f, 0.0f}},
		{{0.0f, -1.0f, 0.0f}} };
	std::vector<uint32_t> indices = { 0, 1, 2 };

	auto vertex_buffer_size = vertices.size() * sizeof(Vertex);
	auto index_buffer_size = indices.size() * sizeof(uint32_t);

	// Create buffers for the bottom level geometry
	// For the sake of simplicity we won't stage the vertex data to the GPU memory

	// Note that the buffer usage flags for buffers consumed by the bottom level acceleration structure require special flags
	const VkBufferUsageFlags buffer_usage_flags = VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

	vertex_buffer_ = std::make_unique<SPBuffer>(engine_.get_device(), vertex_buffer_size, buffer_usage_flags, VMA_MEMORY_USAGE_CPU_TO_GPU);
	vertex_buffer_->update(vertices.data(), vertex_buffer_size);

	index_buffer_ = std::make_unique<SPBuffer>(engine_.get_device(), index_buffer_size, buffer_usage_flags, VMA_MEMORY_USAGE_CPU_TO_GPU);
	index_buffer_->update(indices.data(), index_buffer_size);

	// Setup a single transformation matrix that can be used to transform the whole geometry for a single bottom level acceleration structure
	VkTransformMatrixKHR transform_matrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f };
	std::unique_ptr<SPBuffer> transform_matrix_buffer = std::make_unique<SPBuffer>(engine_.get_device(), sizeof(transform_matrix), buffer_usage_flags, VMA_MEMORY_USAGE_CPU_TO_GPU);
	transform_matrix_buffer->update(&transform_matrix, sizeof(transform_matrix));

	VkDeviceOrHostAddressConstKHR vertex_data_device_address{};
	VkDeviceOrHostAddressConstKHR index_data_device_address{};
	VkDeviceOrHostAddressConstKHR transform_matrix_device_address{};

	vertex_data_device_address.deviceAddress = get_buffer_device_address(vertex_buffer_->get_handle());
	index_data_device_address.deviceAddress = get_buffer_device_address(index_buffer_->get_handle());
	transform_matrix_device_address.deviceAddress = get_buffer_device_address(transform_matrix_buffer->get_handle());

	// The bottom level acceleration structure contains one set of triangles as the input geometry
	VkAccelerationStructureGeometryKHR acceleration_structure_geometry{};
	acceleration_structure_geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	acceleration_structure_geometry.geometryType = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
	acceleration_structure_geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	acceleration_structure_geometry.geometry.triangles.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
	acceleration_structure_geometry.geometry.triangles.vertexFormat = VK_FORMAT_R32G32B32_SFLOAT;
	acceleration_structure_geometry.geometry.triangles.vertexData = vertex_data_device_address;
	acceleration_structure_geometry.geometry.triangles.maxVertex = 3;
	acceleration_structure_geometry.geometry.triangles.vertexStride = sizeof(Vertex);
	acceleration_structure_geometry.geometry.triangles.indexType = VK_INDEX_TYPE_UINT32;
	acceleration_structure_geometry.geometry.triangles.indexData = index_data_device_address;
	acceleration_structure_geometry.geometry.triangles.transformData = transform_matrix_device_address;

	// Get the size requirements for buffers involved in the acceleration structure build process
	VkAccelerationStructureBuildGeometryInfoKHR acceleration_structure_build_geometry_info{};
	acceleration_structure_build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	acceleration_structure_build_geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	acceleration_structure_build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	acceleration_structure_build_geometry_info.geometryCount = 1;
	acceleration_structure_build_geometry_info.pGeometries = &acceleration_structure_geometry;

	const uint32_t primitive_count = 1;

	VkAccelerationStructureBuildSizesInfoKHR acceleration_structure_build_sizes_info{};
	acceleration_structure_build_sizes_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	vkGetAccelerationStructureBuildSizesKHR(
		engine_.get_device().get_handle(),
		VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
		&acceleration_structure_build_geometry_info,
		&primitive_count,
		&acceleration_structure_build_sizes_info);

	// Create a buffer to hold the acceleration structure
	bottom_level_acceleration_structure_.buffer_ = std::make_unique<SPBuffer>(
		engine_.get_device(),
		acceleration_structure_build_sizes_info.accelerationStructureSize,
		VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
		VMA_MEMORY_USAGE_GPU_ONLY);

	// Create the acceleration structure
	VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info{};
	acceleration_structure_create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	acceleration_structure_create_info.buffer = bottom_level_acceleration_structure_.buffer_->get_handle();
	acceleration_structure_create_info.size = acceleration_structure_build_sizes_info.accelerationStructureSize;
	acceleration_structure_create_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	vkCreateAccelerationStructureKHR(engine_.get_device().get_handle(), &acceleration_structure_create_info, nullptr, &bottom_level_acceleration_structure_.handle_);

	// The actual build process starts here

	// Create a scratch buffer as a temporary storage for the acceleration structure build
	ScratchBuffer scratch_buffer = create_scratch_buffer(acceleration_structure_build_sizes_info.buildScratchSize);

	VkAccelerationStructureBuildGeometryInfoKHR acceleration_build_geometry_info{};
	acceleration_build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	acceleration_build_geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
	acceleration_build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	acceleration_build_geometry_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	acceleration_build_geometry_info.dstAccelerationStructure = bottom_level_acceleration_structure_.handle_;
	acceleration_build_geometry_info.geometryCount = 1;
	acceleration_build_geometry_info.pGeometries = &acceleration_structure_geometry;
	acceleration_build_geometry_info.scratchData.deviceAddress = scratch_buffer.device_address_;

	VkAccelerationStructureBuildRangeInfoKHR acceleration_structure_build_range_info;
	acceleration_structure_build_range_info.primitiveCount = 1;
	acceleration_structure_build_range_info.primitiveOffset = 0;
	acceleration_structure_build_range_info.firstVertex = 0;
	acceleration_structure_build_range_info.transformOffset = 0;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> acceleration_build_structure_range_infos = { &acceleration_structure_build_range_info };

	// Build the acceleration structure on the device via a one-time command buffer submission
	// Some implementations may support acceleration structure building on the host (VkPhysicalDeviceAccelerationStructureFeaturesKHR->accelerationStructureHostCommands), but we prefer device builds
	VkCommandBuffer command_buffer = engine_.get_device().create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	vkCmdBuildAccelerationStructuresKHR(
		command_buffer,
		1,
		&acceleration_build_geometry_info,
		acceleration_build_structure_range_infos.data());

	const auto& queue = engine_.get_device().get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);
	engine_.get_device().flush_command_buffer(command_buffer, queue.get_handle());

	delete_scratch_buffer(scratch_buffer);

	// Get the bottom acceleration structure's handle, which will be used during the top level acceleration build
	VkAccelerationStructureDeviceAddressInfoKHR acceleration_device_address_info{};
	acceleration_device_address_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	acceleration_device_address_info.accelerationStructure = bottom_level_acceleration_structure_.handle_;
	bottom_level_acceleration_structure_.device_address_ = vkGetAccelerationStructureDeviceAddressKHR(engine_.get_device().get_handle(), &acceleration_device_address_info);

}

void SPScene::create_top_level_acceleration_structure()
{
	VkTransformMatrixKHR transform_matrix = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f };

	VkAccelerationStructureInstanceKHR acceleration_structure_instance{};
	acceleration_structure_instance.transform = transform_matrix;
	acceleration_structure_instance.instanceCustomIndex = 0;
	acceleration_structure_instance.mask = 0xFF;
	acceleration_structure_instance.instanceShaderBindingTableRecordOffset = 0;
	acceleration_structure_instance.flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
	acceleration_structure_instance.accelerationStructureReference = bottom_level_acceleration_structure_.device_address_;

	std::unique_ptr<SPBuffer> instances_buffer = std::make_unique<SPBuffer>(engine_.get_device(),
		sizeof(VkAccelerationStructureInstanceKHR),
		VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
		VMA_MEMORY_USAGE_CPU_TO_GPU);
	instances_buffer->update(&acceleration_structure_instance, sizeof(VkAccelerationStructureInstanceKHR));

	VkDeviceOrHostAddressConstKHR instance_data_device_address{};
	instance_data_device_address.deviceAddress = get_buffer_device_address(instances_buffer->get_handle());

	// The top level acceleration structure contains (bottom level) instance as the input geometry
	VkAccelerationStructureGeometryKHR acceleration_structure_geometry{};
	acceleration_structure_geometry.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
	acceleration_structure_geometry.geometryType = VK_GEOMETRY_TYPE_INSTANCES_KHR;
	acceleration_structure_geometry.flags = VK_GEOMETRY_OPAQUE_BIT_KHR;
	acceleration_structure_geometry.geometry.instances.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
	acceleration_structure_geometry.geometry.instances.arrayOfPointers = VK_FALSE;
	acceleration_structure_geometry.geometry.instances.data = instance_data_device_address;

	// Get the size requirements for buffers involved in the acceleration structure build process
	VkAccelerationStructureBuildGeometryInfoKHR acceleration_structure_build_geometry_info{};
	acceleration_structure_build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	acceleration_structure_build_geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	acceleration_structure_build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	acceleration_structure_build_geometry_info.geometryCount = 1;
	acceleration_structure_build_geometry_info.pGeometries = &acceleration_structure_geometry;

	const uint32_t primitive_count = 1;

	VkAccelerationStructureBuildSizesInfoKHR acceleration_structure_build_sizes_info{};
	acceleration_structure_build_sizes_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;
	vkGetAccelerationStructureBuildSizesKHR(
		engine_.get_device().get_handle(), VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
		&acceleration_structure_build_geometry_info,
		&primitive_count,
		&acceleration_structure_build_sizes_info);

	// Create a buffer to hold the acceleration structure
	top_level_acceleration_structure_.buffer_ = std::make_unique<SPBuffer>(
		engine_.get_device(),
		acceleration_structure_build_sizes_info.accelerationStructureSize,
		VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR,
		VMA_MEMORY_USAGE_GPU_ONLY);

	// Create the acceleration structure
	VkAccelerationStructureCreateInfoKHR acceleration_structure_create_info{};
	acceleration_structure_create_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
	acceleration_structure_create_info.buffer = top_level_acceleration_structure_.buffer_->get_handle();
	acceleration_structure_create_info.size = acceleration_structure_build_sizes_info.accelerationStructureSize;
	acceleration_structure_create_info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	vkCreateAccelerationStructureKHR(engine_.get_device().get_handle(), &acceleration_structure_create_info, nullptr, &top_level_acceleration_structure_.handle_);

	// The actual build process starts here

	// Create a scratch buffer as a temporary storage for the acceleration structure build
	ScratchBuffer scratch_buffer = create_scratch_buffer(acceleration_structure_build_sizes_info.buildScratchSize);

	VkAccelerationStructureBuildGeometryInfoKHR acceleration_build_geometry_info{};
	acceleration_build_geometry_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
	acceleration_build_geometry_info.type = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
	acceleration_build_geometry_info.flags = VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR;
	acceleration_build_geometry_info.mode = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
	acceleration_build_geometry_info.dstAccelerationStructure = top_level_acceleration_structure_.handle_;
	acceleration_build_geometry_info.geometryCount = 1;
	acceleration_build_geometry_info.pGeometries = &acceleration_structure_geometry;
	acceleration_build_geometry_info.scratchData.deviceAddress = scratch_buffer.device_address_;

	VkAccelerationStructureBuildRangeInfoKHR acceleration_structure_build_range_info;
	acceleration_structure_build_range_info.primitiveCount = 1;
	acceleration_structure_build_range_info.primitiveOffset = 0;
	acceleration_structure_build_range_info.firstVertex = 0;
	acceleration_structure_build_range_info.transformOffset = 0;
	std::vector<VkAccelerationStructureBuildRangeInfoKHR*> acceleration_build_structure_range_infos = { &acceleration_structure_build_range_info };

	// Build the acceleration structure on the device via a one-time command buffer submission
	// Some implementations may support acceleration structure building on the host (VkPhysicalDeviceAccelerationStructureFeaturesKHR->accelerationStructureHostCommands), but we prefer device builds
	VkCommandBuffer command_buffer = engine_.get_device().create_command_buffer(VK_COMMAND_BUFFER_LEVEL_PRIMARY, true);
	vkCmdBuildAccelerationStructuresKHR(
		command_buffer,
		1,
		&acceleration_build_geometry_info,
		acceleration_build_structure_range_infos.data());

	const auto& queue = engine_.get_device().get_queue_by_flags(VK_QUEUE_GRAPHICS_BIT, 0);

	engine_.get_device().flush_command_buffer(command_buffer, queue.get_handle());

	delete_scratch_buffer(scratch_buffer);

	// Get the top acceleration structure's handle, which will be used to setup it's descriptor
	VkAccelerationStructureDeviceAddressInfoKHR acceleration_device_address_info{};
	acceleration_device_address_info.sType = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
	acceleration_device_address_info.accelerationStructure = top_level_acceleration_structure_.handle_;
	top_level_acceleration_structure_.device_address_ = vkGetAccelerationStructureDeviceAddressKHR(engine_.get_device().get_handle(), &acceleration_device_address_info);

}

void SPScene::create_uniform_buffer()
{
	ubo_ = std::make_unique<SPBuffer>(engine_.get_device(),
		sizeof(uniform_data_),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		VMA_MEMORY_USAGE_CPU_TO_GPU);
	ubo_->convert_and_update(uniform_data_);

	update_uniform_buffers();
}

void SPScene::update_uniform_buffers()
{
	uniform_data_.proj_inverse_ = camera_.projection_;
	uniform_data_.view_inverse_ = camera_.model_matrix_; 
	ubo_->convert_and_update(uniform_data_);
}

uint64_t SPScene::get_buffer_device_address(VkBuffer buffer)
{
	VkBufferDeviceAddressInfoKHR buffer_device_address_info{};
	buffer_device_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	buffer_device_address_info.buffer = buffer;
	return vkGetBufferDeviceAddressKHR(engine_.get_device().get_handle(), &buffer_device_address_info);
}

ScratchBuffer SPScene::create_scratch_buffer(VkDeviceSize size)
{
	ScratchBuffer scratch_buffer{};

	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = size;
	buffer_create_info.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
	VK_CHECK(vkCreateBuffer(engine_.get_device().get_handle(), &buffer_create_info, nullptr, &scratch_buffer.handle_));

	VkMemoryRequirements memory_requirements = {};
	vkGetBufferMemoryRequirements(engine_.get_device().get_handle(), scratch_buffer.handle_, &memory_requirements);

	VkMemoryAllocateFlagsInfo memory_allocate_flags_info = {};
	memory_allocate_flags_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
	memory_allocate_flags_info.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;

	VkMemoryAllocateInfo memory_allocate_info = {};
	memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memory_allocate_info.pNext = &memory_allocate_flags_info;
	memory_allocate_info.allocationSize = memory_requirements.size;
	memory_allocate_info.memoryTypeIndex = engine_.get_device().get_memory_type(memory_requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	VK_CHECK(vkAllocateMemory(engine_.get_device().get_handle(), &memory_allocate_info, nullptr, &scratch_buffer.memory_));
	VK_CHECK(vkBindBufferMemory(engine_.get_device().get_handle(), scratch_buffer.handle_, scratch_buffer.memory_, 0));

	VkBufferDeviceAddressInfoKHR buffer_device_address_info{};
	buffer_device_address_info.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
	buffer_device_address_info.buffer = scratch_buffer.handle_;
	scratch_buffer.device_address_ = vkGetBufferDeviceAddressKHR(engine_.get_device().get_handle(), &buffer_device_address_info);

	return scratch_buffer;
}

void SPScene::delete_scratch_buffer(ScratchBuffer& scratch_buffer)
{
	if (scratch_buffer.memory_ != VK_NULL_HANDLE)
	{
		vkFreeMemory(engine_.get_device().get_handle(), scratch_buffer.memory_, nullptr);
	}
	if (scratch_buffer.handle_ != VK_NULL_HANDLE)
	{
		vkDestroyBuffer(engine_.get_device().get_handle(), scratch_buffer.handle_, nullptr);
	}
}


