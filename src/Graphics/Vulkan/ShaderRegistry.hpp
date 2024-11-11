#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <spirv_reflect.h>
#include <vulkan/vulkan_core.h>

#include <Graphics/Shader.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    struct DescriptorSetLayoutKey {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        inline bool operator==(const DescriptorSetLayoutKey &other) const {
            // Descriptor set layouts are compatible as long as bindings are the same
            if (bindings.size() != other.bindings.size()) {
                return false;
            }

            for (unsigned i = 0; i < bindings.size(); i++) {
                const VkDescriptorSetLayoutBinding &a = bindings[i];
                const VkDescriptorSetLayoutBinding &b = other.bindings[i];

                if (a.binding != b.binding || a.descriptorType != b.descriptorType ||
                    a.descriptorCount != b.descriptorCount || a.stageFlags != b.stageFlags) {
                    return false;
                }
            }
            return true;
        }

        struct Hash {
            inline size_t operator()(const DescriptorSetLayoutKey &layout) const {
                size_t hash_base = 0;
                for (const VkDescriptorSetLayoutBinding &binding : layout.bindings) {
                    size_t hash0 = std::hash<unsigned>{}(binding.binding);
                    size_t hash1 = std::hash<unsigned>{}(binding.descriptorCount);
                    size_t hash2 = std::hash<VkDescriptorType>{}(binding.descriptorType);
                    size_t hash3 = std::hash<VkShaderStageFlags>{}(binding.stageFlags);
                    size_t hash4 = std::hash<const VkSampler *>{}(binding.pImmutableSamplers);

                    size_t binding_hash = hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4);
                    hash_base ^= binding_hash;
                }
                return hash_base;
            }
        };
    };

    struct DescriptorBinding {
        std::string name;
        VkDescriptorType type;
        unsigned binding;
        unsigned count;
        unsigned size;
        bool shared;
    };

    struct DescriptorSetLayout {
        VkDescriptorSetLayout handle;
        std::vector<DescriptorBinding> bindings;
    };

    struct PushConstantRange {
        std::string name;
        VkPushConstantRange block;
        bool shared;
    };

    struct ShaderModule {
        VkShaderModule handle;
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<DescriptorSetLayout> descriptor_set_layouts;
        std::vector<PushConstantRange> push_constant_ranges;
    };

    class ShaderRegistry {
        VkDevice _device;
        SparseArray<Shader, ShaderModule> _modules;
        std::unordered_map<DescriptorSetLayoutKey, VkDescriptorSetLayout, DescriptorSetLayoutKey::Hash> _layouts;

        std::vector<uint32_t>
        compile(const std::string &name, const std::string &code, VkShaderStageFlagBits stage, bool optimized);

        void reflect_vertex_input(ShaderModule &module, SpvReflectShaderModule reflection);

        void reflect_descriptor_sets(ShaderModule &module,
                                     SpvReflectShaderModule reflection,
                                     const std::vector<std::string> &shared_uniforms);

        void reflect_push_constants(ShaderModule &module,
                                    SpvReflectShaderModule reflection,
                                    const std::vector<std::string> &shared_uniforms);

      public:
        ShaderRegistry(VkDevice device);
        ~ShaderRegistry();

        const ShaderModule &get(Shader shader) const;

        Shader build(const ShaderDescriptor &descriptor);

        void destroy(Shader shader);
    };
} // namespace Dynamo::Graphics::Vulkan
