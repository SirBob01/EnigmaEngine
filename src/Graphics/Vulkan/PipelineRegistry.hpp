#pragma once

#include <fstream>
#include <string>
#include <unordered_map>

#include <vulkan/vulkan_core.h>

#include <Graphics/Pipeline.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/Swapchain.hpp>
#include <Graphics/Vulkan/UniformRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct PipelineLayoutSettings {
        std::vector<VkDescriptorSetLayout> descriptor_layouts;
        std::vector<VkPushConstantRange> push_constant_ranges;

        inline bool operator==(const PipelineLayoutSettings &other) const {
            if (descriptor_layouts.size() != other.descriptor_layouts.size() ||
                push_constant_ranges.size() != other.push_constant_ranges.size()) {
                return false;
            }
            for (unsigned i = 0; i < descriptor_layouts.size(); i++) {
                if (descriptor_layouts[i] != other.descriptor_layouts[i]) {
                    return false;
                }
            }
            for (unsigned i = 0; i < push_constant_ranges.size(); i++) {
                const VkPushConstantRange &a_range = push_constant_ranges[i];
                const VkPushConstantRange &b_range = other.push_constant_ranges[i];
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
                for (unsigned i = 0; i < settings.descriptor_layouts.size(); i++) {
                    VkDescriptorSetLayout layout = settings.descriptor_layouts[i];
                    size_t hash = std::hash<VkDescriptorSetLayout>{}(layout);
                    hash_base ^= (hash << i);
                }
                for (unsigned i = 0; i < settings.push_constant_ranges.size(); i++) {
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

    struct GraphicsPipelineSettings {
        VkPipelineLayout layout;
        VkShaderModule vertex;
        VkShaderModule fragment;
        VkPrimitiveTopology topology;
        VkPolygonMode fill;
        VkCullModeFlags cull;
        VkSampleCountFlagBits samples;
        VkColorComponentFlags color_mask;
        bool depth_test;
        bool depth_write;
        VkCompareOp depth_test_op;

        inline bool operator==(const GraphicsPipelineSettings &other) const {
            return layout == other.layout && vertex == other.vertex && fragment == other.fragment &&
                   topology == other.topology && fill == other.fill && cull == other.cull && samples == other.samples &&
                   color_mask == other.color_mask && depth_test == other.depth_test &&
                   depth_write == other.depth_write && depth_test_op == other.depth_test_op;
        }

        struct Hash {
            inline size_t operator()(const GraphicsPipelineSettings &settings) const {
                size_t hash0 = std::hash<VkPipelineLayout>{}(settings.layout);
                size_t hash1 = std::hash<VkShaderModule>{}(settings.vertex);
                size_t hash2 = std::hash<VkShaderModule>{}(settings.fragment);
                size_t hash3 = std::hash<VkPrimitiveTopology>{}(settings.topology);
                size_t hash4 = std::hash<VkPolygonMode>{}(settings.fill);
                size_t hash5 = std::hash<VkCullModeFlags>{}(settings.cull);
                size_t hash6 = std::hash<VkSampleCountFlagBits>{}(settings.samples);
                size_t hash7 = std::hash<VkColorComponentFlags>{}(settings.color_mask);
                size_t hash8 = std::hash<bool>{}(settings.depth_test);
                size_t hash9 = std::hash<bool>{}(settings.depth_write);
                size_t hash10 = std::hash<VkCompareOp>{}(settings.depth_test_op);

                return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4) ^ (hash5 << 5) ^ (hash6 << 6) ^
                       (hash7 << 7) ^ (hash8 << 8) ^ (hash9 << 9) ^ (hash10 << 10);
            }
        };
    };

    struct PipelineInstance {
        VkPipelineLayout layout;
        VkPipeline handle;
        std::vector<Uniform> uniforms;
        std::vector<VkDescriptorSet> descriptor_sets;
        std::vector<VkPushConstantRange> push_constant_ranges;
        std::vector<unsigned> push_constant_offsets;
    };

    class PipelineRegistry {
        VkDevice _device;
        const PhysicalDevice &_physical;

        std::ofstream _ofstream;
        VkPipelineCache _pipeline_cache;

        std::unordered_map<PipelineLayoutSettings, VkPipelineLayout, PipelineLayoutSettings::Hash> _layouts;
        std::unordered_map<GraphicsPipelineSettings, VkPipeline, GraphicsPipelineSettings::Hash> _pipelines;

        SparseArray<Pipeline, PipelineInstance> _instances;

      public:
        PipelineRegistry(VkDevice device, const PhysicalDevice &physical, const std::string &filename);
        ~PipelineRegistry();

        Pipeline build(const PipelineDescriptor &descriptor,
                       VkRenderPass renderpass,
                       const Swapchain &swapchain,
                       const ShaderRegistry &shaders,
                       UniformRegistry &uniforms,
                       MemoryPool &memory);

        PipelineInstance &get(Pipeline pipeline);

        void write_to_disk();
    };
} // namespace Dynamo::Graphics::Vulkan