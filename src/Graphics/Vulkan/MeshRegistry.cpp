#include <Graphics/Vulkan/MeshRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    MeshRegistry::MeshRegistry(const Context &context, BufferRegistry &buffers) :
        _context(context),
        _buffers(buffers) {}

    MeshRegistry::~MeshRegistry() {
        _instances.foreach ([&](MeshInstance &instance) {
            for (Buffer buffer : instance.virtual_buffers) {
                _buffers.destroy(buffer);
            }
        });
        _instances.clear();
    }

    void MeshRegistry::write_vertices(const void *src, Buffer dst, unsigned size) {
        BufferDescriptor staging_descriptor;
        staging_descriptor.size = size;
        staging_descriptor.usage = BufferUsage::Staging;
        staging_descriptor.property = MemoryProperty::HostVisible;
        Buffer staging = _buffers.build(staging_descriptor);

        const BufferInstance &dst_instance = _buffers.get(dst);
        const BufferInstance &src_instance = _buffers.get(staging);
        std::memcpy(src_instance.memory.mapped, src, size);

        VkBufferCopy region;
        region.srcOffset = 0;
        region.dstOffset = dst_instance.offset;
        region.size = size;

        VkCommandBuffer_immediate_start(_context.transfer_command_buffer);
        vkCmdCopyBuffer(_context.transfer_command_buffer, src_instance.buffer, dst_instance.buffer, 1, &region);
        VkCommandBuffer_immediate_end(_context.transfer_command_buffer, _context.transfer_queue);

        _buffers.destroy(staging);
    }

    Mesh MeshRegistry::build(const MeshDescriptor &descriptor) {
        // Set the vertex, instance, and index counts
        MeshInstance instance;
        instance.vertex_count = descriptor.vertex_count;
        instance.instance_count = descriptor.instance_count;
        instance.index_count = descriptor.indices.size();

        // Write attributes to the buffers
        for (auto &attribute : descriptor.attributes) {
            BufferDescriptor vertex_descriptor;
            vertex_descriptor.size = attribute.size();
            vertex_descriptor.usage = BufferUsage::Vertex;
            vertex_descriptor.property = MemoryProperty::DeviceLocal;

            Buffer vertex = _buffers.build(vertex_descriptor);
            const BufferInstance &vertex_instance = _buffers.get(vertex);
            instance.attribute_offsets.push_back(vertex_instance.offset);
            instance.buffers.push_back(vertex_instance.buffer);
            instance.virtual_buffers.push_back(vertex);

            write_vertices(attribute.data(), vertex, attribute.size());
        }

        // Write index array, if available
        switch (descriptor.index_type) {
        case IndexType::U16: {
            instance.index_type = VK_INDEX_TYPE_UINT16;

            std::vector<uint16_t> u16_indices;
            for (unsigned index : descriptor.indices) {
                u16_indices.push_back(index);
            }
            BufferDescriptor index_descriptor;
            index_descriptor.size = u16_indices.size() * 2;
            index_descriptor.usage = BufferUsage::Index;
            index_descriptor.property = MemoryProperty::DeviceLocal;

            Buffer index = _buffers.build(index_descriptor);
            const BufferInstance &index_instance = _buffers.get(index);

            instance.index_buffer = index_instance.buffer;
            instance.index_offset = index_instance.offset;
            instance.virtual_buffers.push_back(index);

            write_vertices(u16_indices.data(), index, index_descriptor.size);
            break;
        }
        case IndexType::U32: {
            instance.index_type = VK_INDEX_TYPE_UINT32;

            std::vector<uint32_t> u32_indices;
            for (unsigned index : descriptor.indices) {
                u32_indices.push_back(index);
            }
            BufferDescriptor index_descriptor;
            index_descriptor.size = u32_indices.size() * 4;
            index_descriptor.usage = BufferUsage::Index;
            index_descriptor.property = MemoryProperty::DeviceLocal;

            Buffer index = _buffers.build(index_descriptor);
            const BufferInstance &index_instance = _buffers.get(index);

            instance.index_buffer = index_instance.buffer;
            instance.index_offset = index_instance.offset;
            instance.virtual_buffers.push_back(index);

            write_vertices(u32_indices.data(), index, index_descriptor.size);
            break;
        }
        case IndexType::None:
            instance.index_type = VK_INDEX_TYPE_NONE_KHR;
            break;
        }

        // Register the allocation
        return _instances.insert(instance);
    }

    MeshInstance &MeshRegistry::get(Mesh mesh) { return _instances.get(mesh); }

    void MeshRegistry::destroy(Mesh mesh) {
        MeshInstance &instance = _instances.get(mesh);
        for (Buffer buffer : instance.virtual_buffers) {
            _buffers.destroy(buffer);
        }
        _instances.remove(mesh);
    }
} // namespace Dynamo::Graphics::Vulkan