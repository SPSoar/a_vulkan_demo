/* Copyright (c) 2019-2020, Arm Limited and Contributors
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 the "License";
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "spirv_reflection.h"

namespace vkb
{
namespace
{
template <SPShaderResourceType T>
inline void read_shader_resource(const spirv_cross::Compiler &compiler,
                                 VkShaderStageFlagBits        stage,
                                 std::vector<SPShaderResource> &resources,
                                 const SPShaderVariant &        variant)
{
	//LOGE("Not implemented! Read shader resources of type.");
}

template <spv::Decoration T>
inline void read_resource_decoration(const spirv_cross::Compiler & /*compiler*/,
                                     const spirv_cross::Resource & /*resource*/,
                                     SPShaderResource & /*shader_resource*/,
                                     const SPShaderVariant & /* variant */)
{
	//LOGE("Not implemented! Read resources decoration of type.");
}

template <>
inline void read_resource_decoration<spv::DecorationLocation>(const spirv_cross::Compiler &compiler,
                                                              const spirv_cross::Resource &resource,
                                                              SPShaderResource &             shader_resource,
                                                              const SPShaderVariant &        variant)
{
	shader_resource.location_ = compiler.get_decoration(resource.id, spv::DecorationLocation);
}

template <>
inline void read_resource_decoration<spv::DecorationDescriptorSet>(const spirv_cross::Compiler &compiler,
                                                                   const spirv_cross::Resource &resource,
                                                                   SPShaderResource &             shader_resource,
                                                                   const SPShaderVariant &        variant)
{
	shader_resource.set_ = compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
}

template <>
inline void read_resource_decoration<spv::DecorationBinding>(const spirv_cross::Compiler &compiler,
                                                             const spirv_cross::Resource &resource,
                                                             SPShaderResource &             shader_resource,
                                                             const SPShaderVariant &        variant)
{
	shader_resource.binding_ = compiler.get_decoration(resource.id, spv::DecorationBinding);
}

template <>
inline void read_resource_decoration<spv::DecorationInputAttachmentIndex>(const spirv_cross::Compiler &compiler,
                                                                          const spirv_cross::Resource &resource,
                                                                          SPShaderResource &             shader_resource,
                                                                          const SPShaderVariant &        variant)
{
	shader_resource.input_attachment_index_ = compiler.get_decoration(resource.id, spv::DecorationInputAttachmentIndex);
}

template <>
inline void read_resource_decoration<spv::DecorationNonWritable>(const spirv_cross::Compiler &compiler,
                                                                 const spirv_cross::Resource &resource,
                                                                 SPShaderResource &             shader_resource,
                                                                 const SPShaderVariant &        variant)
{
	shader_resource.qualifiers_ |= SPShaderResourceQualifiers::NonWritable;
}

template <>
inline void read_resource_decoration<spv::DecorationNonReadable>(const spirv_cross::Compiler &compiler,
                                                                 const spirv_cross::Resource &resource,
                                                                 SPShaderResource &             shader_resource,
                                                                 const SPShaderVariant &        variant)
{
	shader_resource.qualifiers_ |= SPShaderResourceQualifiers::NonReadable;
}

inline void read_resource_vec_size(const spirv_cross::Compiler &compiler,
                                   const spirv_cross::Resource &resource,
                                   SPShaderResource &             shader_resource,
                                   const SPShaderVariant &        variant)
{
	const auto &spirv_type = compiler.get_type_from_variable(resource.id);

	shader_resource.vec_size_ = spirv_type.vecsize;
	shader_resource.columns_ = spirv_type.columns;
}

inline void read_resource_array_size(const spirv_cross::Compiler &compiler,
                                     const spirv_cross::Resource &resource,
                                     SPShaderResource &             shader_resource,
                                     const SPShaderVariant &        variant)
{
	const auto &spirv_type = compiler.get_type_from_variable(resource.id);

	shader_resource.array_size_ = spirv_type.array.size() ? spirv_type.array[0] : 1;
}

inline void read_resource_size(const spirv_cross::Compiler &compiler,
                               const spirv_cross::Resource &resource,
                               SPShaderResource &             shader_resource,
                               const SPShaderVariant &        variant)
{
	const auto &spirv_type = compiler.get_type_from_variable(resource.id);

	uint64_t array_size = 0;
	if (variant.get_runtime_array_sizes().count(resource.name) != 0)
	{
		array_size = variant.get_runtime_array_sizes().at(resource.name);
	}

	shader_resource.size_ = sp::to_u32(compiler.get_declared_struct_size_runtime_array(spirv_type, array_size));
}

inline void read_resource_size(const spirv_cross::Compiler &    compiler,
                               const spirv_cross::SPIRConstant &constant,
                               SPShaderResource &                 shader_resource,
                               const SPShaderVariant &            variant)
{
	auto spirv_type = compiler.get_type(constant.constant_type);

	switch (spirv_type.basetype)
	{
		case spirv_cross::SPIRType::BaseType::Boolean:
		case spirv_cross::SPIRType::BaseType::Char:
		case spirv_cross::SPIRType::BaseType::Int:
		case spirv_cross::SPIRType::BaseType::UInt:
		case spirv_cross::SPIRType::BaseType::Float:
			shader_resource.size_ = 4;
			break;
		case spirv_cross::SPIRType::BaseType::Int64:
		case spirv_cross::SPIRType::BaseType::UInt64:
		case spirv_cross::SPIRType::BaseType::Double:
			shader_resource.size_ = 8;
			break;
		default:
			shader_resource.size_ = 0;
			break;
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::Input>(const spirv_cross::Compiler &compiler,
                                                            VkShaderStageFlagBits        stage,
                                                            std::vector<SPShaderResource> &resources,
                                                            const SPShaderVariant &        variant)
{
	auto input_resources = compiler.get_shader_resources().stage_inputs;

	for (auto &resource : input_resources)
	{
		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::Input;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;

		read_resource_vec_size(compiler, resource, shader_resource, variant);
		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationLocation>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::InputAttachment>(const spirv_cross::Compiler &compiler,
                                                                      VkShaderStageFlagBits /*stage*/,
                                                                      std::vector<SPShaderResource> &resources,
                                                                      const SPShaderVariant &        variant)
{
	auto subpass_resources = compiler.get_shader_resources().subpass_inputs;

	for (auto &resource : subpass_resources)
	{
		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::InputAttachment;
		shader_resource.stages_ = VK_SHADER_STAGE_FRAGMENT_BIT;
		shader_resource.name_ = resource.name;

		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationInputAttachmentIndex>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::Output>(const spirv_cross::Compiler &compiler,
                                                             VkShaderStageFlagBits        stage,
                                                             std::vector<SPShaderResource> &resources,
                                                             const SPShaderVariant &        variant)
{
	auto output_resources = compiler.get_shader_resources().stage_outputs;

	for (auto &resource : output_resources)
	{
		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::Output;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;

		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_vec_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationLocation>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::Image>(const spirv_cross::Compiler &compiler,
                                                            VkShaderStageFlagBits        stage,
                                                            std::vector<SPShaderResource> &resources,
                                                            const SPShaderVariant &        variant)
{
	auto image_resources = compiler.get_shader_resources().separate_images;

	for (auto &resource : image_resources)
	{
		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::Image;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;

		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::ImageSampler>(const spirv_cross::Compiler &compiler,
                                                                   VkShaderStageFlagBits        stage,
                                                                   std::vector<SPShaderResource> &resources,
                                                                   const SPShaderVariant &        variant)
{
	auto image_resources = compiler.get_shader_resources().sampled_images;

	for (auto &resource : image_resources)
	{
		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::ImageSampler;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;

		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::ImageStorage>(const spirv_cross::Compiler &compiler,
                                                                   VkShaderStageFlagBits        stage,
                                                                   std::vector<SPShaderResource> &resources,
                                                                   const SPShaderVariant &        variant)
{
	auto storage_resources = compiler.get_shader_resources().storage_images;

	for (auto &resource : storage_resources)
	{
		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::ImageStorage;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;

		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationNonReadable>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationNonWritable>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::Sampler>(const spirv_cross::Compiler &compiler,
                                                              VkShaderStageFlagBits        stage,
                                                              std::vector<SPShaderResource> &resources,
                                                              const SPShaderVariant &        variant)
{
	auto sampler_resources = compiler.get_shader_resources().separate_samplers;

	for (auto &resource : sampler_resources)
	{
		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::Sampler;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;

		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::BufferUniform>(const spirv_cross::Compiler &compiler,
                                                                    VkShaderStageFlagBits        stage,
                                                                    std::vector<SPShaderResource> &resources,
                                                                    const SPShaderVariant &        variant)
{
	auto uniform_resources = compiler.get_shader_resources().uniform_buffers;

	for (auto &resource : uniform_resources)
	{
		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::BufferUniform;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;

		read_resource_size(compiler, resource, shader_resource, variant);
		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}

template <>
inline void read_shader_resource<SPShaderResourceType::BufferStorage>(const spirv_cross::Compiler &compiler,
                                                                    VkShaderStageFlagBits        stage,
                                                                    std::vector<SPShaderResource> &resources,
                                                                    const SPShaderVariant &        variant)
{
	auto storage_resources = compiler.get_shader_resources().storage_buffers;

	for (auto &resource : storage_resources)
	{
		SPShaderResource shader_resource;
		shader_resource.type_ = SPShaderResourceType::BufferStorage;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;

		read_resource_size(compiler, resource, shader_resource, variant);
		read_resource_array_size(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationNonReadable>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationNonWritable>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationDescriptorSet>(compiler, resource, shader_resource, variant);
		read_resource_decoration<spv::DecorationBinding>(compiler, resource, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}
}        // namespace

bool SPIRVReflection::reflect_shader_resources(VkShaderStageFlagBits stage, const std::vector<uint32_t> &spirv, std::vector<SPShaderResource> &resources, const SPShaderVariant &variant)
{
	spirv_cross::CompilerGLSL compiler{spirv};

	auto opts                     = compiler.get_common_options();
	opts.enable_420pack_extension = true;

	compiler.set_common_options(opts);

	parse_shader_resources(compiler, stage, resources, variant);
	parse_push_constants(compiler, stage, resources, variant);
	parse_specialization_constants(compiler, stage, resources, variant);

	return true;
}

void SPIRVReflection::parse_shader_resources(const spirv_cross::Compiler &compiler, VkShaderStageFlagBits stage, std::vector<SPShaderResource> &resources, const SPShaderVariant &variant)
{
	read_shader_resource<SPShaderResourceType::Input>(compiler, stage, resources, variant);
	read_shader_resource<SPShaderResourceType::InputAttachment>(compiler, stage, resources, variant);
	read_shader_resource<SPShaderResourceType::Output>(compiler, stage, resources, variant);
	read_shader_resource<SPShaderResourceType::Image>(compiler, stage, resources, variant);
	read_shader_resource<SPShaderResourceType::ImageSampler>(compiler, stage, resources, variant);
	read_shader_resource<SPShaderResourceType::ImageStorage>(compiler, stage, resources, variant);
	read_shader_resource<SPShaderResourceType::Sampler>(compiler, stage, resources, variant);
	read_shader_resource<SPShaderResourceType::BufferUniform>(compiler, stage, resources, variant);
	read_shader_resource<SPShaderResourceType::BufferStorage>(compiler, stage, resources, variant);
}

void SPIRVReflection::parse_push_constants(const spirv_cross::Compiler &compiler, VkShaderStageFlagBits stage, std::vector<SPShaderResource> &resources, const SPShaderVariant &variant)
{
	auto shader_resources = compiler.get_shader_resources();

	for (auto &resource : shader_resources.push_constant_buffers)
	{
		const auto &spivr_type = compiler.get_type_from_variable(resource.id);

		std::uint32_t offset = std::numeric_limits<std::uint32_t>::max();

		for (auto i = 0U; i < spivr_type.member_types.size(); ++i)
		{
			auto mem_offset = compiler.get_member_decoration(spivr_type.self, i, spv::DecorationOffset);

			offset = std::min(offset, mem_offset);
		}

		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::PushConstant;
		shader_resource.stages_ = stage;
		shader_resource.name_ = resource.name;
		shader_resource.offset_ = offset;

		read_resource_size(compiler, resource, shader_resource, variant);

		shader_resource.size_ -= shader_resource.offset_;

		resources.push_back(shader_resource);
	}
}

void SPIRVReflection::parse_specialization_constants(const spirv_cross::Compiler &compiler, VkShaderStageFlagBits stage, std::vector<SPShaderResource> &resources, const SPShaderVariant &variant)
{
	auto specialization_constants = compiler.get_specialization_constants();

	for (auto &resource : specialization_constants)
	{
		auto &spirv_value = compiler.get_constant(resource.id);

		SPShaderResource shader_resource{};
		shader_resource.type_ = SPShaderResourceType::SpecializationConstant;
		shader_resource.stages_ = stage;
		shader_resource.name_ = compiler.get_name(resource.id);
		shader_resource.offset_ = 0;
		shader_resource.constant_id_ = resource.constant_id;

		read_resource_size(compiler, spirv_value, shader_resource, variant);

		resources.push_back(shader_resource);
	}
}
}        // namespace vkb
