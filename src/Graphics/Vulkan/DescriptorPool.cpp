#include <Graphics/Vulkan/DescriptorPool.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    constexpr unsigned DESCRIPTOR_POOL_SIZE = 512;
    constexpr std::array<VkDescriptorPoolSize, 11> DESCRIPTOR_TYPE_SIZES = {
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLER, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, DESCRIPTOR_POOL_SIZE},
    };

    DescriptorPool::DescriptorPool(const Context &context) : _context(context) {}

    DescriptorPool::~DescriptorPool() {
        // Destroy descriptor pools
        for (const auto &[layout, cache] : _pools) {
            for (const VkDescriptorPool pool : cache.pools) {
                vkDestroyDescriptorPool(_context.device, pool, nullptr);
            }
        }
        _pools.clear();
    }

    VirtualDescriptorSet DescriptorPool::allocate(VkDescriptorSetLayout layout) {
        DescriptorPoolCache &pool_cache = _pools[layout];

        // Check if we can reuse a descriptor set
        if (pool_cache.inactive.size()) {
            VirtualDescriptorSet set = pool_cache.inactive.back();
            pool_cache.inactive.pop_back();
            return set;
        }

        // Allocate a new descriptor set from a pool
        for (VkDescriptorPool &pool : pool_cache.pools) {
            try {
                VirtualDescriptorSet set;
                set.layout = layout;
                VkDescriptorSet_allocate(_context.device, pool, &layout, &set.set, 1);
                return set;
            } catch (std::exception &e) {
            }
        }

        // Pools are full, so create a new one
        VkDescriptorPool pool = VkDescriptorPool_create(_context.device,
                                                        DESCRIPTOR_TYPE_SIZES.data(),
                                                        DESCRIPTOR_TYPE_SIZES.size(),
                                                        DESCRIPTOR_POOL_SIZE);
        pool_cache.pools.push_back(pool);

        VirtualDescriptorSet set;
        set.layout = layout;
        VkDescriptorSet_allocate(_context.device, pool, &layout, &set.set, 1);
        return set;
    }

    void DescriptorPool::free(const VirtualDescriptorSet &set) {
        // Mark the set for recycling
        _pools.at(set.layout).inactive.push_back(set);
    }
} // namespace Dynamo::Graphics::Vulkan