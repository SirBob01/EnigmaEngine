#pragma once

#include <unordered_map>
#include <vector>

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
        VkDevice _device;

        std::unordered_map<VkDescriptorSetLayout, DescriptorPoolCache> _pools;

      public:
        DescriptorPool(VkDevice device);
        ~DescriptorPool();

        VirtualDescriptorSet allocate_descriptor_set(VkDescriptorSetLayout layout);

        void free_descriptor_set(const VirtualDescriptorSet &set);
    };
} // namespace Dynamo::Graphics::Vulkan