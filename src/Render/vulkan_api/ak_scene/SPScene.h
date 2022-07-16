#ifndef S_SPSCENE_H_
#define S_SPSCENE_H_

#include "../vulkan_common.h"
#include "../RenderComponents/SPSCamera.h"
#include "../RenderComponents/SPSSubMesh.h"
#include "../RenderComponents/SPSTexture.h"
#include "../RenderComponents/SPSImage.h"
#include "../RenderComponents/SPSHeightMap.h"
#include "../RenderComponents/SPSFrustum.h"

class SPEngine;
class SPBuffer;

struct STexture
{
	std::unique_ptr<SPSImage> image;
	VkSampler                 sampler;
};

struct Vertex
{
	float pos[3];
};

// Holds data for a scratch buffer used as a temporary storage during acceleration structure builds
struct ScratchBuffer
{
	uint64_t       device_address_;
	VkBuffer       handle_;
	VkDeviceMemory memory_;
};

// Wraps all data required for an acceleration structure
struct AccelerationStructure
{
	VkAccelerationStructureKHR handle_;
	uint64_t                   device_address_;
	std::unique_ptr<SPBuffer>  buffer_;
};


class SPScene
{
	friend class SPRenderWorker;
private:
	void* aligned_alloc(uint64_t size, uint64_t alignment);
	void  aligned_free(void* data);
public:
	SPScene() = delete;
	SPScene(SPEngine& engine);
	~SPScene();
	SPScene(SPScene const& v) = delete;
	SPScene(SPScene&& v) = delete;
	SPScene& operator=(SPScene& v) = delete;
	SPScene& operator=(SPScene&& v) = delete;
public:
	void init();
public :
	void                        create_camera(uint16_t width, uint16_t height);
	std::unique_ptr<SPSSubMesh> load_model(const std::string &file, uint32_t index = 0);
	STexture                    load_texture(const std::string &file);
	STexture                    load_texture_array(const std::string &file);
	STexture                    load_texture_cubemap(const std::string &file);

	//------------------------------------------new

	void create_scene();
	void create_bottom_level_acceleration_structure();
	void create_top_level_acceleration_structure();

	void create_uniform_buffer();
	void update_uniform_buffers();
private:
	uint64_t get_buffer_device_address(VkBuffer buffer);
	ScratchBuffer create_scratch_buffer(VkDeviceSize size);
	void          delete_scratch_buffer(ScratchBuffer& scratch_buffer);
private:
	SPEngine& engine_;
public:
	SPPCamera camera_;
public:
	//-----------------------
	std::unique_ptr<SPBuffer> vertex_buffer_;
	std::unique_ptr<SPBuffer> index_buffer_;
	uint32_t                  index_count_;

	AccelerationStructure bottom_level_acceleration_structure_;
	AccelerationStructure top_level_acceleration_structure_;

	struct UniformData
	{
		spm_dmat4 view_inverse_;
		spm_dmat4 proj_inverse_;
	} uniform_data_;

	std::unique_ptr<SPBuffer> ubo_;

	//-----------------------
};

#endif