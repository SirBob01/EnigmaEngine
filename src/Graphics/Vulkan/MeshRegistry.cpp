#include <Graphics/Vulkan/MeshRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    MeshRegistry::MeshRegistry(VkDevice device, const PhysicalDevice &physical, VkCommandPool transfer_pool) {
        std::array<VkCommandBuffer, 3> transfer_commands;
        VkCommandBuffer_allocate(_device, transfer_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, transfer_commands.data(), 3);

        _vertex = Buffer(device,
                         physical,
                         transfer_commands[0],
                         VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _index = Buffer(device,
                        physical,
                        transfer_commands[1],
                        VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _staging = Buffer(device,
                          physical,
                          transfer_commands[2],
                          VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                          VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
    Mesh MeshRegistry::build(const MeshDescriptor &descriptor) {
        // Set the vertex, instance, and index counts
        MeshAllocation allocation;
        allocation.index_buffer = _index.handle();
        allocation.vertex_count = descriptor.vertex_count;
        allocation.instance_count = descriptor.instance_count;
        allocation.index_count = descriptor.indices.size();

        // Write attributes to the buffers
        for (auto &attribute : descriptor.attributes) {
            unsigned offset = _vertex.reserve(attribute.size());
            allocation.attribute_offsets.push_back(offset);
            allocation.buffers.push_back(_vertex.handle());

            VkBufferCopy region;
            region.srcOffset = 0;
            region.dstOffset = offset;
            region.size = attribute.size();

            unsigned staging_offset = _staging.reserve(attribute.size());
            void *ptr = _staging.get_mapped(staging_offset);
            std::memcpy(ptr, attribute.data(), attribute.size());

            _staging.copy_to(_vertex, &region, 1);
            _staging.free(staging_offset);
        }

        // Write index array, if available
        switch (descriptor.index_type) {
        case IndexType::U16: {
            std::vector<uint16_t> u16_indices;
            for (unsigned index : descriptor.indices) {
                u16_indices.push_back(index);
            }
            unsigned size = u16_indices.size() * sizeof(u16_indices[0]);
            allocation.index_offset = _index.reserve(size);

            VkBufferCopy region;
            region.srcOffset = 0;
            region.dstOffset = allocation.index_offset;
            region.size = size;

            unsigned staging_offset = _staging.reserve(size);
            void *ptr = _staging.get_mapped(staging_offset);
            std::memcpy(ptr, u16_indices.data(), size);

            _staging.copy_to(_index, &region, 1);
            _staging.free(staging_offset);

            allocation.index_type = VK_INDEX_TYPE_UINT16;
            break;
        }
        case IndexType::U32: {
            std::vector<uint32_t> u32_indices;
            for (unsigned index : descriptor.indices) {
                u32_indices.push_back(index);
            }
            unsigned size = u32_indices.size() * sizeof(u32_indices[0]);
            allocation.index_offset = _index.reserve(size);

            VkBufferCopy region;
            region.srcOffset = 0;
            region.dstOffset = allocation.index_offset;
            region.size = size;

            unsigned staging_offset = _staging.reserve(size);
            void *ptr = _staging.get_mapped(staging_offset);
            std::memcpy(ptr, u32_indices.data(), size);

            _staging.copy_to(_index, &region, 1);
            _staging.free(staging_offset);

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

    MeshAllocation &MeshRegistry::get(Mesh mesh) { return _allocations.get(mesh); }

    void MeshRegistry::destroy(Mesh mesh) {
        MeshAllocation &allocation = _allocations.get(mesh);
        for (unsigned offset : allocation.attribute_offsets) {
            _vertex.free(offset);
        }
        if (allocation.index_type != VK_INDEX_TYPE_NONE_KHR) {
            _index.free(allocation.index_offset);
        }
        _allocations.remove(mesh);
    }

    void MeshRegistry::destroy() {
        _vertex.destroy();
        _index.destroy();
        _staging.destroy();
    }
} // namespace Dynamo::Graphics::Vulkan