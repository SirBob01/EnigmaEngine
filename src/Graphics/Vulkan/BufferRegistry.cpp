#include <Graphics/Vulkan/BufferRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    BufferRegistry::BufferRegistry(const Context &context, MemoryPool &memory) : _context(context), _memory(memory) {}

    BufferRegistry::~BufferRegistry() {
        _instances.foreach ([&](BufferInstance &instance) {
            vkDestroyBuffer(_context.device, instance.buffer, nullptr);
            _memory.free(instance.memory);
        });
    }

    Buffer BufferRegistry::build(const BufferDescriptor &descriptor) {
        VkBufferUsageFlags usage = convert_buffer_usage(descriptor.usage);
        VkMemoryPropertyFlags properties = convert_memory_property(descriptor.property);

        BufferInstance instance;
        instance.buffer = VkBuffer_create(_context.device, usage, descriptor.size, nullptr, 0);

        // Allocate memory and bind to buffer
        VkMemoryRequirements requirements;
        vkGetBufferMemoryRequirements(_context.device, instance.buffer, &requirements);
        instance.memory = _memory.allocate(requirements, properties);
        vkBindBufferMemory(_context.device, instance.buffer, instance.memory.memory, instance.memory.key.offset);

        // TODO: Buffer suballocation
        instance.offset = 0;

        return _instances.insert(instance);
    }

    const BufferInstance &BufferRegistry::get(Buffer buffer) const { return _instances.get(buffer); }

    void BufferRegistry::destroy(Buffer buffer) {
        BufferInstance &instance = _instances.get(buffer);
        vkDestroyBuffer(_context.device, instance.buffer, nullptr);
        _memory.free(instance.memory);
        _instances.remove(buffer);
    }
} // namespace Dynamo::Graphics::Vulkan