#pragma once

#include <unordered_map>

#include <vulkan/vulkan_core.h>

#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/FrameContext.hpp>
#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/TextureRegistry.hpp>
#include <Utils/SparseArray.hpp>
#include <Utils/VirtualMemory.hpp>

namespace Dynamo::Graphics::Vulkan {
    // In Vulkan, uniform variables can be from a descriptor or push constant.
    // Renderer API should be able to access both types with the same API.
    enum class UniformType {
        Descriptor,
        PushConstant,
    };

    struct DescriptorData {
        VirtualBuffer buffer;
        VkDescriptorType type;
        VkDescriptorSet set;
        unsigned binding;
        unsigned size;
        unsigned count;
    };

    struct PushConstantData {
        unsigned offset;
        unsigned size;
    };

    struct UniformVariable {
        std::string name;
        UniformType type;
        union {
            DescriptorData descriptor;
            PushConstantData push_constant;
        };
    };

    // Ref-counted shared allocation information
    struct SharedVariable {
        unsigned ref_count;
        union {
            VirtualBuffer descriptor_buffer;
            unsigned push_constant_offset;
        };
    };

    struct DescriptorAllocation {
        VkDescriptorSet descriptor_set;
        std::vector<Uniform> uniforms;
    };

    struct PushConstantAllocation {
        Uniform uniform;
        VkPushConstantRange range;
        unsigned block_offset;
    };

    class UniformRegistry {
        VkDevice _device;
        VkDescriptorPool _pool;
        MemoryPool &_memory;
        VirtualMemory _push_constant_buffer;

        std::unordered_map<std::string, SharedVariable> _shared;
        SparseArray<Uniform, UniformVariable> _variables;

        VirtualBuffer allocate_uniform_buffer(VkDescriptorSet descriptor_set, DescriptorBinding &binding);

        void free_allocation(const UniformVariable &var);

      public:
        UniformRegistry(VkDevice device,
                        const PhysicalDevice &physical,
                        MemoryPool &memory,
                        VkCommandPool transfer_pool);
        ~UniformRegistry();

        DescriptorAllocation allocate(const DescriptorSet &set);

        PushConstantAllocation allocate(const PushConstant &push_constant);

        const UniformVariable &get(Uniform uniform);

        void *get_push_constant_data(unsigned block_offset);

        void write(Uniform uniform, void *data, unsigned index, unsigned count);

        void bind(Uniform uniform, const TextureInstance &texture, unsigned index);

        void destroy(Uniform uniform);
    };
} // namespace Dynamo::Graphics::Vulkan