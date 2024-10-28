#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <spirv_reflect.h>
#include <vulkan/vulkan_core.h>

#include <Graphics/Shader.hpp>
#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics::Vulkan {
    /**
     * @brief Descriptor set layout key.
     *
     */
    struct DescriptorLayoutKey {
        std::vector<VkDescriptorSetLayoutBinding> bindings;

        inline bool operator==(const DescriptorLayoutKey &other) const {
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
            inline size_t operator()(const DescriptorLayoutKey &layout) const {
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

    /**
     * @brief Reflected descriptor binding.
     *
     */
    struct DescriptorBinding {
        std::string name;
        unsigned set;
        unsigned binding;
        unsigned descriptor_count;
        unsigned size;
    };

    /**
     * @brief Descriptor set.
     *
     */
    struct DescriptorSet {
        VkDescriptorSetLayout layout;
        std::vector<DescriptorBinding> bindings;
    };

    /**
     * @brief Push constant.
     *
     */
    struct PushConstant {
        std::string name;
        VkPushConstantRange range;
    };

    /**
     * @brief Shader module instance.
     *
     */
    struct ShaderModule {
        VkShaderModule handle;
        std::vector<VkVertexInputBindingDescription> bindings;
        std::vector<VkVertexInputAttributeDescription> attributes;
        std::vector<DescriptorSet> descriptor_sets;
        std::vector<PushConstant> push_constants;
    };

    /**
     * @brief Shader registry.
     *
     */
    class ShaderRegistry {
        VkDevice _device;
        SparseArray<Shader, ShaderModule> _modules;
        std::unordered_map<DescriptorLayoutKey, VkDescriptorSetLayout, DescriptorLayoutKey::Hash> _descriptor_layouts;

        /**
         * @brief Compile a shader source.
         *
         * @param name
         * @param code
         * @param stage
         * @param optimized
         * @return std::vector<uint32_t>
         */
        std::vector<uint32_t>
        compile(const std::string &name, const std::string &code, VkShaderStageFlagBits stage, bool optimized);

        /**
         * @brief Extract vertex inputs from the shader source.
         *
         * @param module
         * @param reflection
         */
        void reflect_vertex_input(ShaderModule &module, SpvReflectShaderModule reflection);

        /**
         * @brief Extract descriptor sets from the shader source.
         *
         * @param module
         * @param reflection
         */
        void reflect_descriptor_sets(ShaderModule &module, SpvReflectShaderModule reflection);

        /**
         * @brief Extract push constants from the shader source.
         *
         * @param module
         * @param reflection
         */
        void reflect_push_constants(ShaderModule &module, SpvReflectShaderModule reflection);

      public:
        ShaderRegistry(VkDevice device);
        ShaderRegistry() = default;

        /**
         * @brief Get a shader module.
         *
         * @param shader
         * @return ShaderModule&
         */
        const ShaderModule &get(Shader shader) const;

        /**
         * @brief Build a shader module from a descriptor.
         *
         * @param descriptor
         * @return Shader
         */
        Shader build(const ShaderDescriptor &descriptor);

        /**
         * @brief Destroy a shader module.
         *
         * @param shader
         */
        void destroy(Shader shader);

        /**
         * @brief Destroy all existing shader modules.
         *
         */
        void destroy();
    };
} // namespace Dynamo::Graphics::Vulkan
