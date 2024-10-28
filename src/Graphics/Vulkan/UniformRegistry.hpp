#pragma once

#include <vulkan/vulkan_core.h>

#include <Graphics/Vulkan/Buffer.hpp>
#include <Graphics/Vulkan/FrameContext.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Utils/SparseArray.hpp>
#include <Utils/VirtualMemory.hpp>

namespace Dynamo::Graphics::Vulkan {
    /**
     * @brief In Vulkan, shader variables can be from a descriptor or push constant.
     *
     * Renderer API should be able to access both types with the same API.
     *
     */
    enum class UniformVariableType {
        Descriptor,
        PushConstant,
    };

    /**
     * @brief Uniform variable block data.
     *
     */
    struct UniformVariable {
        std::string name;
        UniformVariableType type;
        unsigned block_offset;
        unsigned block_size;
    };

    /**
     * @brief Descriptor allocation result.
     *
     */
    struct DescriptorAllocation {
        VkDescriptorSet set;
        std::vector<Uniform> uniforms;
    };

    /**
     * @brief Push constant allocation result.
     *
     */
    struct PushConstantAllocation {
        Uniform uniform;
        VkPushConstantRange range;
        unsigned block_offset;
    };

    /**
     * @brief Uniform shader variable registry.
     *
     */
    class UniformRegistry {
        VkDevice _device;
        VkDescriptorPool _pool;
        Buffer _uniform_buffer;
        VirtualMemory _push_constant_buffer;
        SparseArray<Uniform, UniformVariable> _variables;

      public:
        UniformRegistry(VkDevice device, const PhysicalDevice &physical, VkCommandPool transfer_pool);
        UniformRegistry() = default;

        /**
         * @brief Reserve memory for uniforms from a descriptor set.
         *
         * @param set
         * @return DescriptorAllocation
         */
        DescriptorAllocation allocate(const DescriptorSet &set);

        /**
         * @brief Reserve memory for uniforms from a push constant.
         *
         * @param push_constant
         * @return PushConstantAllocation
         */
        PushConstantAllocation allocate(const PushConstant &push_constant);

        /**
         * @brief Get a uniform variable.
         *
         * @param uniform
         * @return const UniformVariable&
         */
        const UniformVariable &get(Uniform uniform);

        /**
         * @brief Get the data pointer to a push constant variable
         *
         * @param block_offset
         * @return void*
         */
        void *get_push_constant_data(unsigned block_offset);

        /**
         * @brief Write a value to a uniform variable.
         *
         * @param uniform
         * @param data
         */
        void write(Uniform uniform, void *data);

        /**
         * @brief Free a uniform variable.
         *
         * @param uniform
         */
        void free(Uniform uniform);

        /**
         * @brief Destroy all uniform allocations.
         *
         */
        void destroy();
    };
} // namespace Dynamo::Graphics::Vulkan