#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Graphics::Vulkan {
    // We only have 4096 guaranteed allocations. 32M * 4096 is over 100GB, so this should be enough.
    constexpr VkDeviceSize MIN_ALLOCATION_SIZE = 32 * (1 << 20);

    MemoryPool::MemoryPool(const Context &context) :
        _context(context),
        _groups(_context.physical.memory.memoryTypeCount) {}

    MemoryPool::~MemoryPool() {
        // Free device memory
        for (const MemoryGroup &group : _groups) {
            for (const Memory &memory : group) {
                vkFreeMemory(_context.device, memory.memory, nullptr);
            }
        }
        _groups.clear();
    }

    unsigned MemoryPool::find_type_index(const VkMemoryRequirements &requirements,
                                         VkMemoryPropertyFlags properties) const {
        unsigned type_index = 0;
        while (type_index < _context.physical.memory.memoryTypeCount) {
            VkMemoryType type = _context.physical.memory.memoryTypes[type_index];
            bool has_type = requirements.memoryTypeBits & (1 << type_index);
            bool has_properties = (properties & type.propertyFlags) == properties;
            if (has_type && has_properties) {
                break;
            }
            type_index++;
        }
        DYN_ASSERT(type_index < _groups.size());
        return type_index;
    }

    VirtualMemory MemoryPool::allocate(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags properties) {
        // Find compatible memory block and suballocate
        unsigned type = find_type_index(requirements, properties);
        MemoryGroup &group = _groups[type];
        for (unsigned index = 0; index < group.size(); index++) {
            Memory &memory = group[index];
            char *base_ptr = static_cast<char *>(memory.mapped);

            std::optional<unsigned> result = memory.allocator.reserve(requirements.size, requirements.alignment);
            if (result.has_value()) {
                VirtualMemory allocation;
                allocation.key.type = type;
                allocation.key.index = index;
                allocation.key.offset = result.value();
                allocation.memory = memory.memory;
                allocation.mapped = nullptr;
                if (base_ptr) {
                    allocation.mapped = base_ptr + allocation.key.offset;
                }
                return allocation;
            }
        }

        // None found, build new memory block and suballocate
        VkDeviceSize heap_size = std::max(requirements.size, MIN_ALLOCATION_SIZE);
        VkDeviceMemory memory = VkDeviceMemory_allocate(_context.device, type, heap_size);
        void *mapped = nullptr;
        if (properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) {
            VkResult_check("Map Memory", vkMapMemory(_context.device, memory, 0, heap_size, 0, &mapped));
        }
        group.push_back({memory, heap_size, mapped});

        VirtualMemory allocation;
        allocation.key.type = type;
        allocation.key.index = group.size() - 1;
        allocation.key.offset = group.back().allocator.reserve(requirements.size, requirements.alignment).value();
        allocation.memory = memory;
        allocation.mapped = mapped;
        return allocation;
    }

    void MemoryPool::free(const VirtualMemory &allocation) {
        Memory &memory = _groups[allocation.key.type][allocation.key.index];
        memory.allocator.free(allocation.key.offset);
    }
} // namespace Dynamo::Graphics::Vulkan