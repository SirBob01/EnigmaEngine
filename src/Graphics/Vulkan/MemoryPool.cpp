#include <Graphics/Vulkan/MemoryPool.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Graphics::Vulkan {
    MemoryPool::MemoryPool(const Context &context) :
        _context(context),
        _groups(_context.physical.memory.memoryTypeCount) {}

    MemoryPool::~MemoryPool() {
        // Free device memory
        for (const std::vector<MainMemory> &group : _groups) {
            for (const MainMemory &main : group) {
                vkFreeMemory(_context.device, main.memory, nullptr);
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

    SubMemory MemoryPool::allocate(const VkMemoryRequirements &requirements, VkMemoryPropertyFlags properties) {
        // Find compatible memory block and suballocate
        unsigned type = find_type_index(requirements, properties);
        std::vector<MainMemory> &group = _groups[type];
        for (unsigned index = 0; index < group.size(); index++) {
            MainMemory &memory = group[index];
            unsigned char *base_ptr = static_cast<unsigned char *>(memory.mapped);

            std::optional<unsigned> result = memory.allocator.reserve(requirements.size, requirements.alignment);
            if (result.has_value()) {
                SubMemory submemory;
                submemory.allocation.type = type;
                submemory.allocation.index = index;
                submemory.allocation.offset = result.value();
                submemory.memory = memory.memory;
                submemory.mapped = nullptr;
                if (base_ptr) {
                    submemory.mapped = base_ptr + submemory.allocation.offset;
                }
                return submemory;
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

        Allocator &allocator = group.back().allocator;
        SubMemory submemory;
        submemory.allocation.type = type;
        submemory.allocation.index = group.size() - 1;
        submemory.allocation.offset = allocator.reserve(requirements.size, requirements.alignment).value();
        submemory.memory = memory;
        submemory.mapped = mapped;
        return submemory;
    }

    void MemoryPool::free(const Allocation &allocation) {
        MainMemory &memory = _groups[allocation.type][allocation.index];
        memory.allocator.free(allocation.offset);
    }
} // namespace Dynamo::Graphics::Vulkan