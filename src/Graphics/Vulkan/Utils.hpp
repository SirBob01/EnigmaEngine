#pragma once

#include <string>

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Pipeline.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Graphics::Vulkan {
    unsigned VkFormat_size(VkFormat format);

    const char *VkFormat_string(VkFormat format);

    const char *VkSampleCountFlagBits_string(VkSampleCountFlagBits samples);

    const char *VkPhysicalDeviceType_string(VkPhysicalDeviceType type);

    const char *VkShaderStageFlagBits_string(VkShaderStageFlagBits stage);

    const char *VkDescriptorType_string(VkDescriptorType type);

    const char *VkResult_string(VkResult result);

    const char *VkImageLayout_string(VkImageLayout layout);

    void VkResult_check(const std::string &op_message, VkResult result);

    VkPolygonMode convert_fill(Fill fill);

    VkCullModeFlags convert_cull(Cull cull);

    VkPrimitiveTopology convert_topology(Topology topology);

    VkCompareOp convert_compare_op(CompareOp op);

    VkFormat convert_texture_format(TextureFormat format,
                                    const VkSurfaceFormatKHR &surface_format,
                                    VkFormat physical_depth_format);

    VkFilter convert_texture_filter(TextureFilter filter);

    VkSamplerMipmapMode convert_texture_mipmap_filter(TextureFilter filter);

    VkSamplerAddressMode convert_texture_address_mode(TextureAddressMode address_mode);

    VkInstance VkInstance_create(const Display &display);

    VkDebugUtilsMessengerEXT VkDebugUtilsMessengerEXT_create(VkInstance instance);

    VkDevice VkDevice_create(const PhysicalDevice &physical);

    VkDeviceMemory VkDeviceMemory_allocate(VkDevice device, unsigned type_index, VkDeviceSize size);

    VkBuffer VkBuffer_create(VkDevice device,
                             VkBufferUsageFlags usage,
                             VkDeviceSize size,
                             const QueueFamily *queue_families,
                             unsigned queue_family_count);

    VkImage VkImage_create(VkDevice device,
                           const VkExtent3D &extent,
                           VkFormat format,
                           VkImageLayout layout,
                           VkImageType type,
                           VkImageTiling tiling,
                           VkImageUsageFlags usage,
                           VkSampleCountFlagBits samples,
                           VkImageCreateFlags flags,
                           unsigned mip_levels,
                           unsigned array_layers,
                           const QueueFamily *queue_families,
                           unsigned queue_family_count);

    void VkImage_transition_layout(VkImage image,
                                   VkCommandBuffer command_buffer,
                                   VkImageLayout prev,
                                   VkImageLayout next,
                                   const VkImageSubresourceRange &subresources);

    VkImageView VkImageView_create(VkDevice device,
                                   VkImage image,
                                   VkFormat format,
                                   VkImageViewType type,
                                   const VkImageSubresourceRange &subresources,
                                   const VkComponentMapping &swizzle = {});

    VkSampler VkSampler_create(VkDevice device,
                               VkSamplerAddressMode address_mode_u,
                               VkSamplerAddressMode address_mode_v,
                               VkSamplerAddressMode address_mode_w,
                               VkFilter mag_filter,
                               VkFilter min_filter,
                               VkSamplerMipmapMode mipmap_mode,
                               VkBorderColor border_color,
                               float max_anisotropy,
                               unsigned mip_levels);

    VkDescriptorSetLayout
    VkDescriptorSetLayout_create(VkDevice device, const VkDescriptorSetLayoutBinding *bindings, unsigned binding_count);

    VkPipelineLayout VkPipelineLayout_create(VkDevice device,
                                             VkDescriptorSetLayout *descriptor_layouts,
                                             unsigned descriptor_layout_count,
                                             VkPushConstantRange *push_constant_ranges,
                                             unsigned push_constant_range_count);

    VkShaderModule VkShaderModule_create(VkDevice device, const std::vector<uint32_t> &bytecode);

    // Very simple forward render pass configuration
    VkRenderPass VkRenderPass_create(VkDevice device,
                                     VkSampleCountFlagBits samples,
                                     VkFormat color_format,
                                     VkFormat depth_stencil_format);

    VkPipelineCache VkPipelineCache_create(VkDevice device, const std::string &filename);

    VkPipeline VkPipeline_create(VkDevice device,
                                 VkPipelineCache cache,
                                 VkPipelineLayout layout,
                                 VkRenderPass renderpass,
                                 VkShaderModule vertex,
                                 VkShaderModule fragment,
                                 VkPrimitiveTopology topology,
                                 VkPolygonMode fill,
                                 VkCullModeFlags cull,
                                 VkSampleCountFlagBits samples,
                                 VkColorComponentFlags color_mask,
                                 bool depth_test,
                                 bool depth_write,
                                 VkCompareOp depth_test_op,
                                 const VkVertexInputBindingDescription *vertex_bindings,
                                 unsigned vertex_binding_count,
                                 const VkVertexInputAttributeDescription *vertex_attributes,
                                 unsigned vertex_attribute_count);

    VkFramebuffer VkFramebuffer_create(VkDevice device,
                                       VkRenderPass renderpass,
                                       const VkExtent2D &extent,
                                       const VkImageView *views,
                                       unsigned view_count,
                                       unsigned layer_count);

    VkCommandPool VkCommandPool_create(VkDevice device, QueueFamily family);

    void VkCommandBuffer_allocate(VkDevice device,
                                  VkCommandPool pool,
                                  VkCommandBufferLevel level,
                                  VkCommandBuffer *dst,
                                  unsigned count);

    VkDescriptorPool
    VkDescriptorPool_create(VkDevice device, VkDescriptorPoolSize *sizes, unsigned size_count, unsigned max_sets);

    void VkDescriptorSet_allocate(VkDevice device,
                                  VkDescriptorPool pool,
                                  const VkDescriptorSetLayout *layouts,
                                  VkDescriptorSet *dst,
                                  unsigned count);

    void VkCommandBuffer_immediate_start(VkCommandBuffer command_buffer);

    void VkCommandBuffer_immediate_end(VkCommandBuffer command_buffer, VkQueue queue);

    VkFence VkFence_create(VkDevice device);

    VkSemaphore VkSemaphore_create(VkDevice device);
} // namespace Dynamo::Graphics::Vulkan
