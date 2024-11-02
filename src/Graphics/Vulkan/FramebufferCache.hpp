#pragma once

#include <unordered_map>

#include <vulkan/vulkan_core.h>

namespace Dynamo::Graphics::Vulkan {
    struct FramebufferSettings {
        VkImageView view;
        VkExtent2D extent;
        VkRenderPass renderpass;

        inline bool operator==(const FramebufferSettings &other) const {
            return view == other.view && extent.width == other.extent.width && extent.height == other.extent.height &&
                   renderpass == other.renderpass;
        }

        struct Hash {
            inline size_t operator()(const FramebufferSettings &settings) const {
                size_t hash0 = std::hash<unsigned>{}(settings.extent.width);
                size_t hash1 = std::hash<unsigned>{}(settings.extent.height);
                size_t hash2 = std::hash<VkRenderPass>{}(settings.renderpass);
                size_t hash3 = std::hash<VkImageView>{}(settings.view);

                return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3);
            }
        };
    };

    class FramebufferCache {
        VkDevice _device;
        std::unordered_map<FramebufferSettings, VkFramebuffer, FramebufferSettings::Hash> _cache;

      public:
        FramebufferCache(VkDevice device);
        FramebufferCache() = default;

        VkFramebuffer get(const FramebufferSettings &settings);

        void destroy();
    };
} // namespace Dynamo::Graphics::Vulkan