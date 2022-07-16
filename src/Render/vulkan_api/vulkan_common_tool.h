#ifndef S_VULKAN_COMMON_TOOL_H_
#define S_VULKAN_COMMON_TOOL_H_

#include <vector>
#include <stdint.h>
#include <string>
#include "vulkan_common.h"

class vkTool 
{
public:
    static std::string get_extension(const std::string& uri);
	static std::vector<uint8_t> read_binary_file(const std::string& filename, const uint32_t count);
	static std::vector<std::string> split(const std::string& input, char delim);
	static std::string read_text_file(const std::string& filename);
    static bool is_astc(const VkFormat format);


    static bool is_depth_only_format(VkFormat format);
    static VkFormat get_suitable_depth_format(VkPhysicalDevice physical_device, bool depth_only = false,
        const std::vector<VkFormat>& depth_format_priority_list = {
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM 
        }
    );
    static bool is_depth_stencil_format(VkFormat format);
    static VkSemaphoreCreateInfo semaphore_create_info();
    static VkSubmitInfo submit_info();
    static VkCommandBufferAllocateInfo command_buffer_allocate_info(VkCommandPool command_pool,
        VkCommandBufferLevel level, uint32_t buffer_count);
    static VkFenceCreateInfo fence_create_info(VkFenceCreateFlags flags = 0);
    static VkDescriptorSetLayoutBinding descriptor_set_layout_binding(
        VkDescriptorType   type,
        VkShaderStageFlags flags,
        uint32_t           binding,
        uint32_t           count = 1);
    static VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info(
        const VkDescriptorSetLayoutBinding* bindings,
        uint32_t                            binding_count);
    static VkPipelineLayoutCreateInfo pipeline_layout_create_info(
        const VkDescriptorSetLayout* set_layouts,
        uint32_t                     set_layout_count = 1);
    static VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info(
        VkPrimitiveTopology                     topology,
        VkPipelineInputAssemblyStateCreateFlags flags,
        VkBool32                                primitive_restart_enable);
    static VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info(
        VkPolygonMode                           polygon_mode,
        VkCullModeFlags                         cull_mode,
        VkFrontFace                             front_face,
        VkPipelineRasterizationStateCreateFlags flags = 0);
    static VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state(
        VkColorComponentFlags color_write_mask,
        VkBool32              blend_enable);
    static VkPipelineColorBlendStateCreateInfo pipeline_color_blend_state_create_info(
        uint32_t                                   attachment_count,
        const VkPipelineColorBlendAttachmentState* attachments);
    static VkPipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_create_info(
        VkBool32    depth_test_enable,
        VkBool32    depth_write_enable,
        VkCompareOp depth_compare_op);
    static VkPipelineViewportStateCreateInfo pipeline_viewport_state_create_info(
        uint32_t                           viewport_count,
        uint32_t                           scissor_count,
        VkPipelineViewportStateCreateFlags flags = 0);
    static VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info(
        VkSampleCountFlagBits                 rasterization_samples,
        VkPipelineMultisampleStateCreateFlags flags = 0);
    static VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info(
        const std::vector<VkDynamicState>& dynamic_states,
        VkPipelineDynamicStateCreateFlags  flags = 0);
    static VkPipelineDynamicStateCreateInfo pipeline_dynamic_state_create_info(
        const VkDynamicState* dynamic_states,
        uint32_t                          dynamicStateCount,
        VkPipelineDynamicStateCreateFlags flags = 0);
    static VkShaderModule load_shader(const std::string& filename, VkDevice device, VkShaderStageFlagBits stage);
    static VkShaderStageFlagBits find_shader_stage(const std::string& ext);
    static VkVertexInputBindingDescription vertex_input_binding_description(
        uint32_t          binding,
        uint32_t          stride,
        VkVertexInputRate input_rate);
    static VkVertexInputAttributeDescription vertex_input_attribute_description(
        uint32_t binding,
        uint32_t location,
        VkFormat format,
        uint32_t offset);
    static VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info();
    static VkGraphicsPipelineCreateInfo pipeline_create_info(
        VkPipelineLayout      layout,
        VkRenderPass          render_pass,
        VkPipelineCreateFlags flags = 0);
    static VkDescriptorPoolSize descriptor_pool_size(
        VkDescriptorType type,
        uint32_t         count);
    static VkDescriptorPoolCreateInfo descriptor_pool_create_info(
        uint32_t              count,
        VkDescriptorPoolSize* pool_sizes,
        uint32_t              max_sets);
    static VkDescriptorPoolCreateInfo descriptor_pool_create_info(
        const std::vector<VkDescriptorPoolSize>& pool_sizes,
        uint32_t                                 max_sets);
    static inline VkDescriptorSetAllocateInfo descriptor_set_allocate_info(
        VkDescriptorPool             descriptor_pool,
        const VkDescriptorSetLayout* set_layouts,
        uint32_t                     descriptor_set_count)
    {
        VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
        descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptor_set_allocate_info.descriptorPool = descriptor_pool;
        descriptor_set_allocate_info.pSetLayouts = set_layouts;
        descriptor_set_allocate_info.descriptorSetCount = descriptor_set_count;
        return descriptor_set_allocate_info;
    }


    static inline VkWriteDescriptorSet write_descriptor_set(
        VkDescriptorSet         dst_set,
        VkDescriptorType        type,
        uint32_t                binding,
        VkDescriptorBufferInfo* buffer_info,
        uint32_t                descriptor_count = 1)
    {
        VkWriteDescriptorSet write_descriptor_set{};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet = dst_set;
        write_descriptor_set.descriptorType = type;
        write_descriptor_set.dstBinding = binding;
        write_descriptor_set.pBufferInfo = buffer_info;
        write_descriptor_set.descriptorCount = descriptor_count;
        return write_descriptor_set;
    }

    static inline VkWriteDescriptorSet write_descriptor_set(
        VkDescriptorSet        dst_set,
        VkDescriptorType       type,
        uint32_t               binding,
        VkDescriptorImageInfo* image_info,
        uint32_t               descriptor_count = 1)
    {
        VkWriteDescriptorSet write_descriptor_set{};
        write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write_descriptor_set.dstSet = dst_set;
        write_descriptor_set.descriptorType = type;
        write_descriptor_set.dstBinding = binding;
        write_descriptor_set.pImageInfo = image_info;
        write_descriptor_set.descriptorCount = descriptor_count;
        return write_descriptor_set;
    }

    static void insert_image_memory_barrier(
        VkCommandBuffer         command_buffer,
        VkImage                 image,
        VkAccessFlags           src_access_mask,
        VkAccessFlags           dst_access_mask,
        VkImageLayout           old_layout,
        VkImageLayout           new_layout,
        VkPipelineStageFlags    src_stage_mask,
        VkPipelineStageFlags    dst_stage_mask,
        VkImageSubresourceRange subresource_range)
    {
        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.srcAccessMask = src_access_mask;
        barrier.dstAccessMask = dst_access_mask;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.image = image;
        barrier.subresourceRange = subresource_range;

        vkCmdPipelineBarrier(
            command_buffer,
            src_stage_mask,
            dst_stage_mask,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

    static inline VkRenderingInfoKHR rendering_info(VkRect2D                            render_area = {},
        uint32_t                            color_attachment_count = 0,
        const VkRenderingAttachmentInfoKHR* pColorAttachments = VK_NULL_HANDLE,
        VkRenderingFlagsKHR                 flags = 0)
    {
        VkRenderingInfoKHR rendering_info = {};
        rendering_info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO_KHR;
        rendering_info.pNext = VK_NULL_HANDLE;
        rendering_info.flags = flags;
        rendering_info.renderArea = render_area;
        rendering_info.layerCount = 0;
        rendering_info.viewMask = 0;
        rendering_info.colorAttachmentCount = color_attachment_count;
        rendering_info.pColorAttachments = pColorAttachments;
        rendering_info.pDepthAttachment = VK_NULL_HANDLE;
        rendering_info.pStencilAttachment = VK_NULL_HANDLE;
        return rendering_info;
    }

    static inline VkImageMemoryBarrier image_memory_barrier()
    {
        VkImageMemoryBarrier image_memory_barrier{};
        image_memory_barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        image_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        image_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        return image_memory_barrier;
    }
    static inline VkRenderingAttachmentInfoKHR rendering_attachment_info()
    {
        VkRenderingAttachmentInfoKHR attachment_info{};
        attachment_info.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO_KHR;
        attachment_info.pNext = VK_NULL_HANDLE;
        return attachment_info;
    }
    static inline VkDescriptorImageInfo descriptor_image_info(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout)
    {
        VkDescriptorImageInfo descriptor_image_info{};
        descriptor_image_info.sampler = sampler;
        descriptor_image_info.imageView = image_view;
        descriptor_image_info.imageLayout = image_layout;
        return descriptor_image_info;
    }
    static inline VkCommandBufferBeginInfo command_buffer_begin_info() 
    {
        VkCommandBufferBeginInfo cmdBufferBeginInfo{};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        return cmdBufferBeginInfo;
    }
    static inline VkRenderPassBeginInfo render_pass_begin_info()
    {
        VkRenderPassBeginInfo render_pass_begin_info{};
        render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        return render_pass_begin_info;
    }

    static inline VkMemoryAllocateInfo memory_allocate_info()
    {
        VkMemoryAllocateInfo memory_allocation{};
        memory_allocation.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        return memory_allocation;
    }

    static inline VkFramebufferCreateInfo framebuffer_create_info()
    {
        VkFramebufferCreateInfo framebuffer_create_info{};
        framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        return framebuffer_create_info;
    }


    static inline VkBufferCreateInfo buffer_create_info()
    {
        VkBufferCreateInfo buffer_create_info{};
        buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        return buffer_create_info;
    }

    static inline VkBufferCreateInfo buffer_create_info(
        VkBufferUsageFlags usage,
        VkDeviceSize       size)
    {
        VkBufferCreateInfo buffer_create_info{};
        buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        buffer_create_info.usage = usage;
        buffer_create_info.size = size;
        return buffer_create_info;
    }

    static inline VkPipelineTessellationStateCreateInfo pipeline_tessellation_state_create_info(uint32_t patch_control_points)
    {
        VkPipelineTessellationStateCreateInfo pipeline_tessellation_state_create_info{};
        pipeline_tessellation_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
        pipeline_tessellation_state_create_info.patchControlPoints = patch_control_points;
        return pipeline_tessellation_state_create_info;
    }

    static inline VkComputePipelineCreateInfo compute_pipeline_create_info(
        VkPipelineLayout      layout,
        VkPipelineCreateFlags flags = 0)
    {
        VkComputePipelineCreateInfo compute_pipeline_create_info{};
        compute_pipeline_create_info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        compute_pipeline_create_info.layout = layout;
        compute_pipeline_create_info.flags = flags;
        return compute_pipeline_create_info;
    }

    static inline VkBufferMemoryBarrier buffer_memory_barrier()
    {
        VkBufferMemoryBarrier buffer_memory_barrier{};
        buffer_memory_barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
        buffer_memory_barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        buffer_memory_barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        return buffer_memory_barrier;
    }
    static inline VkPushConstantRange push_constant_range(
        VkShaderStageFlags stage_flags,
        uint32_t           size,
        uint32_t           offset)
    {
        VkPushConstantRange push_constant_range{};
        push_constant_range.stageFlags = stage_flags;
        push_constant_range.offset = offset;
        push_constant_range.size = size;
        return push_constant_range;
    }

    static inline VkSamplerCreateInfo sampler_create_info()
    {
        VkSamplerCreateInfo sampler_create_info{};
        sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_create_info.maxAnisotropy = 1.0f;
        return sampler_create_info;
    }

    static inline VkImageViewCreateInfo image_view_create_info()
    {
        VkImageViewCreateInfo image_view_create_info{};
        image_view_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        return image_view_create_info;
    }
    static inline VkMemoryBarrier memory_barrier()
    {
        VkMemoryBarrier memory_barrier{};
        memory_barrier.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        return memory_barrier;
    }

    static inline VkImageCreateInfo image_create_info()
    {
        VkImageCreateInfo image_create_info{};
        image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        return image_create_info;
    }

    static inline VkSpecializationMapEntry specialization_map_entry(uint32_t constant_id, uint32_t offset, size_t size)
    {
        VkSpecializationMapEntry specialization_map_entry{};
        specialization_map_entry.constantID = constant_id;
        specialization_map_entry.offset = offset;
        specialization_map_entry.size = size;
        return specialization_map_entry;
    }

    static inline VkSpecializationInfo specialization_info(uint32_t map_entry_count, const VkSpecializationMapEntry* map_entries, size_t data_size, const void* data)
    {
        VkSpecializationInfo specialization_info{};
        specialization_info.mapEntryCount = map_entry_count;
        specialization_info.pMapEntries = map_entries;
        specialization_info.dataSize = data_size;
        specialization_info.pData = data;
        return specialization_info;
    }

    static inline VkViewport viewport(
        float width,
        float height,
        float min_depth,
        float max_depth)
    {
        VkViewport viewport{};
        viewport.width = width;
        viewport.height = height;
        viewport.minDepth = min_depth;
        viewport.maxDepth = max_depth;
        return viewport;
    }

    static VkRect2D rect2D(
        int32_t width,
        int32_t height,
        int32_t offset_x,
        int32_t offset_y)
    {
        VkRect2D rect2D{};
        rect2D.extent.width = width;
        rect2D.extent.height = height;
        rect2D.offset.x = offset_x;
        rect2D.offset.y = offset_y;
        return rect2D;
    }

    static void set_image_layout(
        VkCommandBuffer         command_buffer,
        VkImage                 image,
        VkImageLayout           old_layout,
        VkImageLayout           new_layout,
        VkImageSubresourceRange subresource_range,
        VkPipelineStageFlags    src_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VkPipelineStageFlags    dst_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

    static void set_image_layout(
        VkCommandBuffer      command_buffer,
        VkImage              image,
        VkImageAspectFlags   aspect_mask,
        VkImageLayout        old_layout,
        VkImageLayout        new_layout,
        VkPipelineStageFlags src_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VkPipelineStageFlags dst_mask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);


};


#endif