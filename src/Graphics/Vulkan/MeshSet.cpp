#include <Graphics/Vulkan/MeshSet.hpp>

namespace Dynamo::Graphics::Vulkan {
    Mesh MeshSet::build(const MeshDescriptor &descriptor,
                        Buffer &vertex_buffer,
                        Buffer &index_buffer,
                        Buffer &staging_buffer) {
        // Set the vertex, instance, and index counts
        MeshAllocation allocation;
        allocation.vertex_count = descriptor.vertex_count;
        allocation.instance_count = descriptor.instance_count;
        allocation.index_count = descriptor.indices.size();

        // Write attributes to the buffers
        for (auto &attribute : descriptor.attributes) {
            unsigned offset = vertex_buffer.reserve(attribute.size());
            allocation.attribute_offsets.push_back(offset);
            allocation.buffers.push_back(vertex_buffer.handle());

            VkBufferCopy region;
            region.srcOffset = 0;
            region.dstOffset = offset;
            region.size = attribute.size();

            unsigned staging_offset = staging_buffer.reserve(attribute.size());
            staging_buffer.host_write(attribute.data(), staging_offset, attribute.size());
            staging_buffer.copy_to(vertex_buffer, &region, 1);
            staging_buffer.free(staging_offset);
        }

        // Write index array, if available
        switch (descriptor.index_type) {
        case IndexType::U16: {
            std::vector<uint16_t> short_indices;
            for (unsigned index : descriptor.indices) {
                short_indices.push_back(index);
            }
            unsigned size = short_indices.size() * sizeof(short_indices[0]);
            allocation.index_offset = index_buffer.reserve(size);

            VkBufferCopy region;
            region.srcOffset = 0;
            region.dstOffset = allocation.index_offset;
            region.size = size;

            unsigned staging_offset = staging_buffer.reserve(size);
            staging_buffer.host_write(reinterpret_cast<char *>(short_indices.data()), staging_offset, size);
            staging_buffer.copy_to(index_buffer, &region, 1);
            staging_buffer.free(staging_offset);

            allocation.index_type = VK_INDEX_TYPE_UINT16;
            break;
        }
        case IndexType::U32: {
            std::vector<uint32_t> short_indices;
            for (unsigned index : descriptor.indices) {
                short_indices.push_back(index);
            }
            unsigned size = short_indices.size() * sizeof(short_indices[0]);
            allocation.index_offset = index_buffer.reserve(size);

            VkBufferCopy region;
            region.srcOffset = 0;
            region.dstOffset = allocation.index_offset;
            region.size = size;

            unsigned staging_offset = staging_buffer.reserve(size);
            staging_buffer.host_write(reinterpret_cast<char *>(short_indices.data()), staging_offset, size);
            staging_buffer.copy_to(index_buffer, &region, 1);
            staging_buffer.free(staging_offset);

            allocation.index_type = VK_INDEX_TYPE_UINT32;
            break;
        }
        case IndexType::None:
            allocation.index_type = VK_INDEX_TYPE_NONE_KHR;
            break;
        }

        // Register the allocation
        return _allocations.insert(allocation);
    }

    MeshAllocation &MeshSet::get(Mesh mesh) { return _allocations.get(mesh); }

    void MeshSet::free(Mesh mesh, Buffer &vertex_buffer, Buffer &index_buffer) {
        MeshAllocation &allocation = _allocations.get(mesh);
        for (unsigned offset : allocation.attribute_offsets) {
            vertex_buffer.free(offset);
        }
        if (allocation.index_type != VK_INDEX_TYPE_NONE_KHR) {
            index_buffer.free(allocation.index_offset);
        }
        _allocations.remove(mesh);
    }
} // namespace Dynamo::Graphics::Vulkan