#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan.hpp>

#include <Graphics/Material.hpp>
#include <Graphics/Vulkan/Buffer.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/Swapchain.hpp>
#include <Graphics/Vulkan/UniformRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    /**
     * @brief Render pass configuration settings.
     *
     */
    struct RenderPassSettings {
        VkFormat color_format;
        VkFormat depth_format;

        bool clear_color = false;
        bool clear_depth = false;

        unsigned sample_count = 1;

        inline bool operator==(const RenderPassSettings &other) const {
            return color_format == other.color_format && depth_format == other.depth_format &&
                   clear_color == other.clear_color && clear_depth == other.clear_depth &&
                   sample_count == other.sample_count;
        }

        struct Hash {
            inline size_t operator()(const RenderPassSettings &settings) const {
                size_t hash0 = std::hash<bool>{}(settings.clear_color);
                size_t hash1 = std::hash<bool>{}(settings.clear_depth);

                size_t hash2 = std::hash<VkFormat>{}(settings.color_format);
                size_t hash3 = std::hash<VkFormat>{}(settings.depth_format);

                size_t hash4 = std::hash<unsigned>{}(settings.sample_count);

                return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4);
            }
        };
    };

    /**
     * @brief Pipeline layout settings.
     *
     */
    struct PipelineLayoutSettings {
        VkDescriptorSetLayout *descriptor_layouts;
        unsigned descriptor_layout_count;
        VkPushConstantRange *push_constant_ranges;
        unsigned push_constant_range_count;

        inline bool operator==(const PipelineLayoutSettings &other) const {
            if (descriptor_layout_count != other.descriptor_layout_count ||
                push_constant_range_count != other.push_constant_range_count) {
                return false;
            }
            for (unsigned i = 0; i < descriptor_layout_count; i++) {
                if (descriptor_layouts[i] != other.descriptor_layouts[i]) {
                    return false;
                }
            }
            for (unsigned i = 0; i < push_constant_range_count; i++) {
                VkPushConstantRange &a_range = push_constant_ranges[i];
                VkPushConstantRange &b_range = other.push_constant_ranges[i];
                if (a_range.offset != b_range.offset || a_range.size != b_range.size ||
                    a_range.stageFlags != b_range.stageFlags) {
                    return false;
                }
            }
            return true;
        }

        struct Hash {
            inline size_t operator()(const PipelineLayoutSettings &settings) const {
                size_t hash_base = 0;
                for (unsigned i = 0; i < settings.descriptor_layout_count; i++) {
                    VkDescriptorSetLayout layout = settings.descriptor_layouts[i];
                    size_t hash = std::hash<VkDescriptorSetLayout>{}(layout);
                    hash_base ^= (hash << i);
                }
                for (unsigned i = 0; i < settings.push_constant_range_count; i++) {
                    const VkPushConstantRange &range = settings.push_constant_ranges[i];
                    size_t hash0 = std::hash<unsigned>{}(range.size);
                    size_t hash1 = std::hash<unsigned>{}(range.offset);
                    size_t hash2 = std::hash<unsigned>{}(range.stageFlags);
                    hash_base ^= (hash0 << i) ^ (hash1 << (i + 1)) ^ (hash2 << (i + 2));
                }
                return hash_base;
            }
        };
    };

    /**
     * @brief Graphics pipeline configuration settings.
     *
     */
    struct GraphicsPipelineSettings {
        VkPrimitiveTopology topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        VkPolygonMode polygon_mode = VK_POLYGON_MODE_FILL;
        VkCullModeFlags cull_mode = VK_CULL_MODE_BACK_BIT;

        ShaderModule vertex;
        ShaderModule fragment;

        VkRenderPass renderpass;
        VkPipelineLayout layout;

        inline bool operator==(const GraphicsPipelineSettings &other) const {
            return topology == other.topology && polygon_mode == other.polygon_mode && cull_mode == other.cull_mode &&
                   vertex.handle == other.vertex.handle && fragment.handle == other.fragment.handle &&
                   renderpass == other.renderpass && layout == other.layout;
        }

        struct Hash {
            inline size_t operator()(const GraphicsPipelineSettings &settings) const {
                size_t hash0 = std::hash<VkPrimitiveTopology>{}(settings.topology);
                size_t hash1 = std::hash<VkPolygonMode>{}(settings.polygon_mode);
                size_t hash2 = std::hash<VkCullModeFlags>{}(settings.cull_mode);
                size_t hash3 = std::hash<VkShaderModule>{}(settings.vertex.handle);
                size_t hash4 = std::hash<VkShaderModule>{}(settings.fragment.handle);
                size_t hash5 = std::hash<VkRenderPass>{}(settings.renderpass);
                size_t hash6 = std::hash<VkPipelineLayout>{}(settings.layout);

                return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4) ^ (hash5 << 5) ^ (hash6 << 6);
            }
        };
    };

    /**
     * @brief Material instance with references to allocated Vulkan resources.
     *
     */
    struct MaterialInstance {
        VkRenderPass renderpass;
        VkPipelineLayout layout;
        VkPipeline pipeline;
        std::vector<Uniform> uniforms;
        std::vector<VkDescriptorSet> descriptor_sets;
        std::vector<VkPushConstantRange> push_constant_ranges;
        std::vector<unsigned> push_constant_offsets;
    };

    /**
     * @brief Material registry caches Vulkan objects associated with a material.
     *
     */
    class MaterialRegistry {
        VkDevice _device;
        std::ofstream _ofstream;
        VkPipelineCache _pipeline_cache;

        std::unordered_map<RenderPassSettings, VkRenderPass, RenderPassSettings::Hash> _renderpasses;
        std::unordered_map<PipelineLayoutSettings, VkPipelineLayout, PipelineLayoutSettings::Hash> _layouts;
        std::unordered_map<GraphicsPipelineSettings, VkPipeline, GraphicsPipelineSettings::Hash> _pipelines;

        SparseArray<Material, MaterialInstance> _instances;

        /**
         * @brief Create a Vulkan render pass.
         *
         * @param settings
         * @return VkRenderPass
         */
        VkRenderPass build_renderpass(const RenderPassSettings &settings) const;

        /**
         * @brief Create a Vulkan pipeline.
         *
         * @param settings
         * @return VkPipeline
         */
        VkPipeline build_pipeline(const GraphicsPipelineSettings &settings) const;

      public:
        MaterialRegistry(VkDevice device, const std::string &filename);
        MaterialRegistry() = default;

        /**
         * @brief Build a material and its resources.
         *
         * @param descriptor
         * @param swapchain
         * @param shaders
         * @param uniforms
         * @return MaterialInstance
         */
        Material build(const MaterialDescriptor &descriptor,
                       const Swapchain &swapchain,
                       const ShaderRegistry &shaders,
                       UniformRegistry &uniforms);

        /**
         * @brief Get a material instance.
         *
         * @param material
         * @return MaterialInstance&
         */
        MaterialInstance &get(Material material);

        /**
         * @brief Destroy a material instance.
         *
         * Pipeline, layout, and render pass are preserved, only uniforms are freed.
         *
         * @param material
         * @param uniforms
         */
        void destroy(Material material, UniformRegistry &uniforms);

        /**
         * @brief Destroy all Vulkan resources.
         *
         */
        void destroy();

        /**
         * @brief Write the pipeline cache to disk.
         *
         */
        void write_to_disk();
    };
} // namespace Dynamo::Graphics::Vulkan