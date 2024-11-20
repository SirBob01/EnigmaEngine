#include <Graphics/Vulkan/BufferRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    constexpr unsigned MAX_BUFFER_USAGE = static_cast<unsigned>(BufferUsage::Staging) + 1;
    constexpr unsigned MAX_MEMORY_PROPERTY = static_cast<unsigned>(MemoryProperty::DeviceLocal) + 1;

    BufferRegistry::BufferRegistry(const Context &context, MemoryPool &memory) :
        _context(context),
        _memory(memory),
        _groups(MAX_BUFFER_USAGE * MAX_MEMORY_PROPERTY) {}

    BufferRegistry::~BufferRegistry() {
        // Clear suballocations
        _instances.clear();

        // Clear primary buffers
        for (const std::vector<PrimaryBuffer> &group : _groups) {
            for (const PrimaryBuffer &primary : group) {
                vkDestroyBuffer(_context.device, primary.buffer, nullptr);
                _memory.free(primary.allocation);
            }
        }
        _groups.clear();
    }

    PrimaryBuffer
    BufferRegistry::build_primary(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
        VkBuffer buffer = VkBuffer_create(_context.device, usage, size, nullptr, 0);

        // Allocate memory and bind to buffer
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(_context.device, buffer, &requirements);
        VirtualMemory virtual_memory = _memory.allocate(requirements, properties);
        vkBindBufferMemory(_context.device, buffer, virtual_memory.memory, virtual_memory.allocation.offset);

        PrimaryBuffer primary;
        primary.buffer = buffer;
        primary.allocator = Allocator(size);
        primary.allocation = virtual_memory.allocation;
        primary.mapped = virtual_memory.mapped;

        return primary;
    }

    unsigned BufferRegistry::find_type_index(BufferUsage usage, MemoryProperty properties) {
        return static_cast<unsigned>(usage) * MAX_MEMORY_PROPERTY + static_cast<unsigned>(properties);
    }

    Buffer BufferRegistry::build(const BufferDescriptor &descriptor) {
        // Find compatible primary buffer and suballocate
        unsigned type = find_type_index(descriptor.usage, descriptor.property);
        std::vector<PrimaryBuffer> &group = _groups[type];
        for (unsigned index = 0; index < group.size(); index++) {
            PrimaryBuffer &primary = group[index];
            unsigned char *base_ptr = static_cast<unsigned char *>(primary.mapped);

            std::optional<unsigned> result = primary.allocator.reserve(descriptor.size, 1);
            if (result.has_value()) {
                BufferInstance instance;
                instance.buffer = primary.buffer;
                instance.primary_group = type;
                instance.primary_index = index;
                instance.offset = result.value();
                instance.mapped = nullptr;
                if (base_ptr) {
                    instance.mapped = base_ptr + instance.offset;
                }
                return _instances.insert(instance);
            }
        }

        // None found, build new primary buffer and suballocate
        VkDeviceSize size = std::max(static_cast<VkDeviceSize>(descriptor.size), MIN_ALLOCATION_SIZE);
        VkBufferUsageFlags usage = convert_buffer_usage(descriptor.usage);
        VkMemoryPropertyFlags properties = convert_memory_property(descriptor.property);
        group.push_back(build_primary(size, usage, properties));

        PrimaryBuffer &primary = group.back();
        BufferInstance instance;
        instance.buffer = primary.buffer;
        instance.primary_group = type;
        instance.primary_index = group.size() - 1;
        instance.offset = primary.allocator.reserve(descriptor.size, 1).value();
        instance.mapped = primary.mapped;
        return _instances.insert(instance);
    }

    const BufferInstance &BufferRegistry::get(Buffer buffer) const { return _instances.get(buffer); }

    void BufferRegistry::destroy(Buffer buffer) {
        BufferInstance &instance = _instances.get(buffer);
        PrimaryBuffer &primary = _groups[instance.primary_group][instance.primary_index];
        primary.allocator.free(instance.offset);
        _instances.remove(buffer);
    }
} // namespace Dynamo::Graphics::Vulkan