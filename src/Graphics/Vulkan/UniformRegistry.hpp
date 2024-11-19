#pragma once

#include <unordered_map>

#include <vulkan/vulkan_core.h>

#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/Context.hpp>
#include <Graphics/Vulkan/DescriptorPool.hpp>
#include <Graphics/Vulkan/FrameContext.hpp>
#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/TextureRegistry.hpp>
#include <Utils/SparseArray.hpp>
#include <Utils/VirtualBuffer.hpp>

namespace Dynamo::Graphics::Vulkan {
    // Uniform group handle
    DYN_DEFINE_ID_TYPE(UniformGroup);

    // In Vulkan, uniform variables can be from a descriptor or push constant.
    // Renderer API should be able to access both types with the same API.
    enum class UniformType {
        Descriptor,
        PushConstant,
    };

    struct SharedDescriptor {
        unsigned ref_count;
        Buffer buffer;
    };

    struct SharedPushConstant {
        unsigned ref_count;
        unsigned offset;
    };

    struct Descriptor {
        Buffer buffer;
        VkDescriptorType type;
        VkDescriptorSet set;
        unsigned binding;
        unsigned size;
        unsigned count;
    };

    struct PushConstant {
        unsigned offset;
        unsigned size;
    };

    struct UniformGroupInstance {
        std::vector<Uniform> uniforms;
        std::vector<VirtualDescriptorSet> v_sets;
        std::vector<VkDescriptorSet> descriptor_sets;
        std::vector<VkPushConstantRange> push_constant_ranges;
        std::vector<unsigned> push_constant_offsets;
    };

    struct UniformInstance {
        std::string name;
        UniformType type;
        union {
            Descriptor descriptor;
            PushConstant push_constant;
        };
    };

    class UniformRegistry {
        const Context &_context;
        MemoryPool &_memory;
        BufferRegistry &_buffers;
        DescriptorPool &_descriptors;
        VirtualBuffer _push_constant_buffer;

        std::unordered_map<std::string, SharedDescriptor> _shared_descriptors;
        std::unordered_map<std::string, SharedPushConstant> _shared_push_constants;

        SparseArray<UniformGroup, UniformGroupInstance> _groups;
        SparseArray<Uniform, UniformInstance> _uniforms;

        Buffer allocate_descriptor_binding(VkDescriptorSet set, const DescriptorBinding &binding);

        unsigned allocate_push_constant_range(const PushConstantRange &range);

        void free_uniform(const UniformInstance &var);

        void free_group(const UniformGroupInstance &group);

      public:
        UniformRegistry(const Context &context,
                        MemoryPool &memory,
                        BufferRegistry &buffers,
                        DescriptorPool &descriptors);
        ~UniformRegistry();

        UniformGroup build(const std::vector<DescriptorSetLayout> &descriptor_set_layouts,
                           const std::vector<PushConstantRange> &push_constant_ranges);

        const UniformGroupInstance &get(UniformGroup group) const;

        std::optional<Uniform> find(UniformGroup group, const std::string &uniform_name) const;

        void *get_push_constant_data(unsigned block_offset);

        void write(Uniform uniform, void *data, unsigned index, unsigned count);

        void bind(Uniform uniform, const TextureInstance &texture, unsigned index);

        void destroy(UniformGroup group);
    };
} // namespace Dynamo::Graphics::Vulkan