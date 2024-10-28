#pragma once

#include <string>

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Material.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Graphics::Vulkan {
    /**
     * @brief Compute the size of VkFormat
     *
     * @param format
     * @return unsigned
     */
    unsigned VkFormat_size(VkFormat format);

    /**
     * @brief Convert VkPhysicalDeviceType to string.
     *
     * @param type
     * @return const char*
     */
    const char *VkPhysicalDeviceType_string(VkPhysicalDeviceType type);

    /**
     * @brief Convert VkShaderStageFlagBits to string.
     *
     * @param stage
     * @return const char*
     */
    const char *VkShaderStageFlagBits_string(VkShaderStageFlagBits stage);

    /**
     * @brief Convert VkDescriptorType to string.
     *
     * @param type
     * @return const char*
     */
    const char *VkDescriptorType_string(VkDescriptorType type);

    /**
     * @brief Convert VkResult to string.
     *
     * @param result
     * @return const char*
     */
    const char *VkResult_string(VkResult result);

    /**
     * @brief Log the result of a Vulkan operation.
     *
     * @param op_message
     * @param result
     */
    void VkResult_log(const std::string &op_message, VkResult result);

    /**
     * @brief Convert Fill to VkPolygonMode.
     *
     * @param fill
     * @return VkPolygonMode
     */
    VkPolygonMode convert_fill(Fill fill);

    /**
     * @brief Convert Cull to VkCullModeFlags
     *
     * @param cull
     * @return VkCullModeFlags
     */
    VkCullModeFlags convert_cull(Cull cull);

    /**
     * @brief Convert Topology to VkPrimitiveTopology
     *
     * @param topology
     * @return VkPrimitiveTopology
     */
    VkPrimitiveTopology convert_topology(Topology topology);

    /**
     * @brief Create a Vulkan instance.
     *
     * @param display
     * @return VkInstance
     */
    VkInstance VkInstance_create(const Display &display);

    /**
     * @brief Create a Vulkan debug messenger.
     *
     * @param instance
     * @return VkDebugUtilsMessengerEXT
     */
    VkDebugUtilsMessengerEXT VkDebugUtilsMessengerEXT_create(VkInstance instance);

    /**
     * @brief Create the Vulkan logical device.
     *
     * @param physical
     * @return VkDevice
     */
    VkDevice VkDevice_create(const PhysicalDevice &physical);

    /**
     * @brief Allocate Vulkan device memory.
     *
     * @param device
     * @param type_index
     * @param size
     * @return VkDeviceMemory
     */
    VkDeviceMemory VkDeviceMemory_allocate(VkDevice device, unsigned type_index, unsigned size);

    /**
     * @brief Create a Vulkan buffer.
     *
     * @param device
     * @param usage
     * @param size
     * @param queue_families
     * @param queue_family_count
     * @return VkBuffer
     */
    VkBuffer VkBuffer_create(VkDevice device,
                             VkBufferUsageFlags usage,
                             unsigned size,
                             const QueueFamily *queue_families,
                             unsigned queue_family_count);

    /**
     * @brief Submit a command to copy the contents of a Vulkan buffer to another buffer.
     *
     * @param src
     * @param dst
     * @param queue
     * @param command_buffer
     * @param regions
     * @param region_count
     */
    void VkBuffer_immediate_copy(VkBuffer src,
                                 VkBuffer dst,
                                 VkQueue queue,
                                 VkCommandBuffer command_buffer,
                                 VkBufferCopy *regions,
                                 unsigned region_count);

    /**
     * @brief Create a Vulkan image view.
     *
     * @param device
     * @param image
     * @param format
     * @param type
     * @param subresources
     * @param swizzle
     * @return VkImageView
     */
    VkImageView VkImageView_create(VkDevice device,
                                   VkImage image,
                                   VkFormat format,
                                   VkImageViewType type,
                                   const VkImageSubresourceRange &subresources,
                                   const VkComponentMapping &swizzle = {});

    /**
     * @brief Create a Vulkan descriptor set layout.
     *
     * @param device
     * @param bindings
     * @param binding_count
     * @return VkDescriptorSetLayout
     */
    VkDescriptorSetLayout
    VkDescriptorSetLayout_create(VkDevice device, const VkDescriptorSetLayoutBinding *bindings, unsigned binding_count);

    /**
     * @brief Create a Vulkan pipeline layout.
     *
     * @param device
     * @param descriptor_layouts
     * @param descriptor_layout_count
     * @param push_constant_ranges
     * @param push_constant_range_count
     * @return VkPipelineLayout
     */
    VkPipelineLayout VkPipelineLayout_create(VkDevice device,
                                             VkDescriptorSetLayout *descriptor_layouts,
                                             unsigned descriptor_layout_count,
                                             VkPushConstantRange *push_constant_ranges,
                                             unsigned push_constant_range_count);

    /**
     * @brief Create a Vulkan shader module.
     *
     * @param device
     * @param bytecode
     * @return VkShaderModule
     */
    VkShaderModule VkShaderModule_create(VkDevice device, const std::vector<uint32_t> &bytecode);

    /**
     * @brief Create a Vulkan framebuffer.
     *
     * @param device
     * @param renderpass
     * @param extent
     * @param views
     * @param view_count
     * @param layer_count
     * @return VkFramebuffer
     */
    VkFramebuffer VkFramebuffer_create(VkDevice device,
                                       VkRenderPass renderpass,
                                       const VkExtent2D &extent,
                                       const VkImageView *views,
                                       unsigned view_count,
                                       unsigned layer_count);

    /**
     * @brief Create a Vulkan command pool.
     *
     * @param device
     * @param family
     * @return VkCommandPool
     */
    VkCommandPool VkCommandPool_create(VkDevice device, QueueFamily family);

    /**
     * @brief Allocate Vulkan command buffers from a pool.
     *
     * @param device
     * @param pool
     * @param level
     * @param dst
     * @param count
     */
    void VkCommandBuffer_allocate(VkDevice device,
                                  VkCommandPool pool,
                                  VkCommandBufferLevel level,
                                  VkCommandBuffer *dst,
                                  unsigned count);

    /**
     * @brief Create a Vulkan descriptor pool.
     *
     * @param device
     * @param sizes
     * @param size_count
     * @param max_sets
     * @return VkDescriptorPool
     */
    VkDescriptorPool
    VkDescriptorPool_create(VkDevice device, VkDescriptorPoolSize *sizes, unsigned size_count, unsigned max_sets);

    /**
     * @brief Allocate Vulkan descriptor sets from a pool.
     *
     * @param device
     * @param pool
     * @param layouts
     * @param dst
     * @param count
     */
    void VkDescriptorSet_allocate(VkDevice device,
                                  VkDescriptorPool pool,
                                  const VkDescriptorSetLayout *layouts,
                                  VkDescriptorSet *dst,
                                  unsigned count);

    /**
     * @brief Create a Vulkan fence.
     *
     * @param device
     * @return VkFence
     */
    VkFence VkFence_create(VkDevice device);

    /**
     * @brief Create a Vulkan semaphore.
     *
     * @param device
     * @return VkSemaphore
     */
    VkSemaphore VkSemaphore_create(VkDevice device);
} // namespace Dynamo::Graphics::Vulkan
