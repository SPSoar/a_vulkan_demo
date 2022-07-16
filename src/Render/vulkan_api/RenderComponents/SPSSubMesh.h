#ifndef S_SPSSUBMESH_H_
#define S_SPSSUBMESH_H_

#include "SPComponent.h"

#include "../Resource/SPShaderSource.h"

struct SPVertexAttribute
{
	VkFormat format_ = VK_FORMAT_UNDEFINED;

	std::uint32_t stride_ = 0;

	std::uint32_t offset_ = 0;
};


struct SPSVertex
{
	glm::vec3 pos_;
	glm::vec3 normal_;
	glm::vec2 uv_;
	glm::vec4 joint0_;
	glm::vec4 weight0_;
};

class SPBuffer;
class SPSPBRMaterial;
class SPSSubMesh : public SPComponent
{
public:
	SPSSubMesh(const std::string& name = {});
	virtual ~SPSSubMesh() = default;
	SPSSubMesh(SPSSubMesh&& other) = default;
	virtual std::type_index get_type() override;
public:
	void set_attribute(const std::string& name, const SPVertexAttribute& attribute);
	void set_material(const SPSPBRMaterial& material);
private:
	void compute_shader_variant();
public:
	std::uint32_t vertices_count_;
	std::unordered_map<std::string, SPBuffer> vertex_buffers_;
	std::unique_ptr<SPBuffer> index_buffer_;
	std::uint32_t vertex_indices_ = 0;
	VkIndexType index_type_;

private:
	std::unordered_map<std::string, SPVertexAttribute> vertex_attributes_;
	SPShaderVariant shader_variant_;
	const SPSPBRMaterial* material_{ nullptr };
};

#endif