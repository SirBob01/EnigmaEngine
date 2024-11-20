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

        // Clear main buffers
        for (const std::vector<MainBuffer> &group : _groups) {
            for (const MainBuffer &main : group) {
                vkDestroyBuffer(_context.device, main.buffer, nullptr);
                _memory.free(main.allocation);
            }
        }
        _groups.clear();
    }

    MainBuffer
    BufferRegistry::build_main(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties) {
        VkBuffer buffer = VkBuffer_create(_context.device, usage, size, nullptr, 0);

        // Allocate memory and bind to buffer
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(_context.device, buffer, &requirements);
        SubMemory submemory = _memory.allocate(requirements, properties);
        vkBindBufferMemory(_context.device, buffer, submemory.memory, submemory.allocation.offset);

        MainBuffer main;
        main.buffer = buffer;
        main.allocator = Allocator(size);
        main.allocation = submemory.allocation;
        main.mapped = submemory.mapped;

        return main;
    }

    unsigned BufferRegistry::find_type_index(BufferUsage usage, MemoryProperty properties) {
        return static_cast<unsigned>(usage) * MAX_MEMORY_PROPERTY + static_cast<unsigned>(properties);
    }

    Buffer BufferRegistry::build(const BufferDescriptor &descriptor) {
        // Find compatible main buffer and suballocate
        unsigned type = find_type_index(descriptor.usage, descriptor.property);
        std::vector<MainBuffer> &group = _groups[type];
        for (unsigned index = 0; index < group.size(); index++) {
            MainBuffer &main = group[index];
            unsigned char *base_ptr = static_cast<unsigned char *>(main.mapped);

            std::optional<unsigned> result = main.allocator.reserve(descriptor.size, 1);
            if (result.has_value()) {
                BufferInstance instance;
                instance.buffer = main.buffer;
                instance.main_group = type;
                instance.main_index = index;
                instance.offset = result.value();
                instance.mapped = nullptr;
                if (base_ptr) {
                    instance.mapped = base_ptr + instance.offset;
                }
                return _instances.insert(instance);
            }
        }

        // None found, build new main buffer and suballocate
        VkDeviceSize size = std::max(static_cast<VkDeviceSize>(descriptor.size), MIN_ALLOCATION_SIZE);
        VkBufferUsageFlags usage = convert_buffer_usage(descriptor.usage);
        VkMemoryPropertyFlags properties = convert_memory_property(descriptor.property);
        group.emplace_back(build_main(size, usage, properties));

        MainBuffer &main = group.back();
        BufferInstance instance;
        instance.buffer = main.buffer;
        instance.main_group = type;
        instance.main_index = group.size() - 1;
        instance.offset = main.allocator.reserve(descriptor.size, 1).value();
        instance.mapped = main.mapped;
        return _instances.insert(instance);
    }

    const BufferInstance &BufferRegistry::get(Buffer buffer) const { return _instances.get(buffer); }

    void BufferRegistry::destroy(Buffer buffer) {
        BufferInstance &instance = _instances.get(buffer);
        MainBuffer &main = _groups[instance.main_group][instance.main_index];
        main.allocator.free(instance.offset);
        _instances.remove(buffer);
    }
} // namespace Dynamo::Graphics::Vulkan