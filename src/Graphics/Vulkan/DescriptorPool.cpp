#include <Graphics/Vulkan/DescriptorPool.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    // Descriptor pool sizes, need to balance this for time spent allocating vs memory usage
    constexpr unsigned DESCRIPTOR_POOL_SIZE = 1024;
    constexpr std::array<VkDescriptorPoolSize, 2> DESCRIPTOR_TYPE_SIZES = {
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, DESCRIPTOR_POOL_SIZE},
        VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, DESCRIPTOR_POOL_SIZE},
    };

    DescriptorPool::DescriptorPool(VkDevice device) : _device(device) {}

    DescriptorPool::~DescriptorPool() {
        // Destroy descriptor pools
        for (const auto &[layout, cache] : _pools) {
            for (const VkDescriptorPool pool : cache.pools) {
                vkDestroyDescriptorPool(_device, pool, nullptr);
            }
        }
        _pools.clear();
    }

    VirtualDescriptorSet DescriptorPool::allocate_descriptor_set(VkDescriptorSetLayout layout) {
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
                VkDescriptorSet_allocate(_device, pool, &layout, &set.set, 1);
                return set;
            } catch (std::exception &e) {
            }
        }

        // Pools are full, so create a new one
        VkDescriptorPool pool = VkDescriptorPool_create(_device,
                                                        DESCRIPTOR_TYPE_SIZES.data(),
                                                        DESCRIPTOR_TYPE_SIZES.size(),
                                                        DESCRIPTOR_POOL_SIZE);
        pool_cache.pools.push_back(pool);

        VirtualDescriptorSet set;
        set.layout = layout;
        VkDescriptorSet_allocate(_device, pool, &layout, &set.set, 1);
        return set;
    }

    void DescriptorPool::free_descriptor_set(const VirtualDescriptorSet &set) {
        // Mark the set for recycling
        _pools.at(set.layout).inactive.push_back(set);
    }
} // namespace Dynamo::Graphics::Vulkan