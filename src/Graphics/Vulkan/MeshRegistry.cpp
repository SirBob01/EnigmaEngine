#include <Graphics/Vulkan/MeshRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    MeshRegistry::MeshRegistry(const BufferRegistry &buffers) : _buffers(buffers) {}

    MeshRegistry::~MeshRegistry() { _instances.clear(); }

    Mesh MeshRegistry::build(const MeshDescriptor &descriptor) {
        MeshInstance instance;

        for (const VertexAttribute &attribute : descriptor.attributes) {
            const BufferInstance &attribute_instance = _buffers.get(attribute.buffer);
            instance.attribute_buffers.push_back(attribute_instance.buffer);
            instance.attribute_offsets.push_back(attribute_instance.offset + attribute.offset);
        }

        if (descriptor.index_type != IndexType::None) {
            const BufferInstance &index_instance = _buffers.get(descriptor.indices.buffer);
            instance.index_buffer = index_instance.buffer;
            instance.index_offset = index_instance.offset + descriptor.indices.offset;
        }

        instance.index_type = convert_index_type(descriptor.index_type);
        instance.vertex_count = descriptor.vertex_count;
        instance.instance_count = descriptor.instance_count;
        instance.index_count = descriptor.index_count;

        // Register the mesh
        return _instances.insert(instance);
    }

    MeshInstance &MeshRegistry::get(Mesh mesh) { return _instances.get(mesh); }

    void MeshRegistry::destroy(Mesh mesh) { _instances.remove(mesh); }
} // namespace Dynamo::Graphics::Vulkan