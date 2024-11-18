#pragma once

#include <unordered_map>
#include <vector>

#include <Graphics/Vulkan/Context.hpp>
#include <vulkan/vulkan_core.h>

namespace Dynamo::Graphics::Vulkan {
    struct VirtualDescriptorSet {
        VkDescriptorSetLayout layout;
        VkDescriptorSet set;
    };

    struct DescriptorPoolCache {
        std::vector<VkDescriptorPool> pools;
        std::vector<VirtualDescriptorSet> inactive;
    };

    class DescriptorPool {
        const Context &_context;

        std::unordered_map<VkDescriptorSetLayout, DescriptorPoolCache> _pools;

      public:
        DescriptorPool(const Context &context);
        ~DescriptorPool();

        VirtualDescriptorSet allocate(VkDescriptorSetLayout layout);

        void free(const VirtualDescriptorSet &set);
    };
} // namespace Dynamo::Graphics::Vulkan