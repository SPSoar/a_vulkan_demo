#include "SPSSubMesh.h"
#include "../Buffer/SPBuffer.h"

#include "SPSPBRMaterial.h"

SPSSubMesh::SPSSubMesh(const std::string& name)
	: SPComponent{ name }
	, vertices_count_{ 0 }
{
}

std::type_index SPSSubMesh::get_type()
{
	return typeid(SPSSubMesh);
}

void SPSSubMesh::set_attribute(const std::string& attribute_name, const SPVertexAttribute& attribute)
{
	vertex_attributes_[attribute_name] = attribute;

	compute_shader_variant();
}

void SPSSubMesh::set_material(const SPSPBRMaterial& new_material)
{
	material_ = &new_material;

	compute_shader_variant();
}

void SPSSubMesh::compute_shader_variant()
{
	shader_variant_.clear();

	if (material_ != nullptr)
	{
		for (auto& texture : material_->textures_)
		{
			std::string tex_name = texture.first;
			std::transform(tex_name.begin(), tex_name.end(), tex_name.begin(), ::toupper);

			shader_variant_.add_define("HAS_" + tex_name);
		}
	}

	for (auto& attribute : vertex_attributes_)
	{
		std::string attrib_name = attribute.first;
		std::transform(attrib_name.begin(), attrib_name.end(), attrib_name.begin(), ::toupper);
		shader_variant_.add_define("HAS_" + attrib_name);
	}
}
