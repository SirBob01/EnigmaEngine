#include <Graphics/Vulkan/MeshRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    MeshRegistry::MeshRegistry(const Context &context, MemoryPool &memory) : _context(context), _memory(memory) {
        VkCommandBuffer_allocate(_context.device,
                                 _context.transfer_pool,
                                 VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                 &_command_buffer,
                                 1);
    }

    MeshRegistry::~MeshRegistry() {
        _instances.foreach ([&](MeshInstance &instance) {
            for (VirtualBuffer &buffer : instance.virtual_buffers) {
                _memory.free_buffer(buffer);
            }
        });
        _instances.clear();
    }

    void MeshRegistry::write_vertices(const void *src, VirtualBuffer &dst, unsigned size) {
        VkBufferCopy region;
        region.srcOffset = 0;
        region.dstOffset = dst.offset;
        region.size = size;

        VirtualBuffer staging =
            _memory.allocate_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                    VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                    size);
        std::memcpy(staging.mapped, src, size);

        VkCommandBuffer_immediate_start(_command_buffer);
        vkCmdCopyBuffer(_command_buffer, staging.buffer, dst.buffer, 1, &region);
        VkCommandBuffer_immediate_end(_command_buffer, _context.transfer_queue);

        _memory.free_buffer(staging);
    }

    Mesh MeshRegistry::build(const MeshDescriptor &descriptor) {
        // Set the vertex, instance, and index counts
        MeshInstance instance;
        instance.vertex_count = descriptor.vertex_count;
        instance.instance_count = descriptor.instance_count;
        instance.index_count = descriptor.indices.size();

        // Write attributes to the buffers
        for (auto &attribute : descriptor.attributes) {
            VirtualBuffer vertex =
                _memory.allocate_buffer(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        attribute.size());
            instance.attribute_offsets.push_back(vertex.offset);
            instance.buffers.push_back(vertex.buffer);
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
            unsigned size = u16_indices.size() * 2;
            VirtualBuffer index =
                _memory.allocate_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        size);
            instance.index_buffer = index.buffer;
            instance.index_offset = index.offset;
            instance.virtual_buffers.push_back(index);

            write_vertices(u16_indices.data(), index, size);
            break;
        }
        case IndexType::U32: {
            instance.index_type = VK_INDEX_TYPE_UINT32;

            std::vector<uint32_t> u32_indices;
            for (unsigned index : descriptor.indices) {
                u32_indices.push_back(index);
            }
            unsigned size = u32_indices.size() * 4;
            VirtualBuffer index =
                _memory.allocate_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                        size);
            instance.index_buffer = index.buffer;
            instance.index_offset = index.offset;
            instance.virtual_buffers.push_back(index);

            write_vertices(u32_indices.data(), index, size);
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
        for (VirtualBuffer &buffer : instance.virtual_buffers) {
            _memory.free_buffer(buffer);
        }
        _instances.remove(mesh);
    }
} // namespace Dynamo::Graphics::Vulkan