#include <Graphics/Vulkan/UniformRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    constexpr VkMemoryPropertyFlags UNIFORM_MEMORY_PROPERTIES =
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    // Limit of 128 bytes for push constants
    constexpr unsigned PUSH_CONSTANT_HEAP_SIZE = 128;

    UniformRegistry::UniformRegistry(VkDevice device,
                                     const PhysicalDevice &physical,
                                     MemoryPool &memory,
                                     DescriptorPool &descriptors,
                                     VkCommandPool transfer_pool) :
        _device(device),
        _memory(memory),
        _descriptors(descriptors),
        _push_constant_buffer(PUSH_CONSTANT_HEAP_SIZE) {}

    UniformRegistry::~UniformRegistry() {
        // Free uniform groups
        _groups.foreach ([&](UniformGroupInstance &group) { free_group(group); });
        _groups.clear();
    }

    VirtualBuffer UniformRegistry::allocate_descriptor_binding(VkDescriptorSet set, const DescriptorBinding &binding) {
        unsigned size = binding.size * binding.count;

        // Not shared, allocate a new buffer
        if (!binding.shared) {
            return _memory.allocate_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, UNIFORM_MEMORY_PROPERTIES, size);
        }

        // If shared, find the allocation and increase ref count
        auto shared_it = _shared_descriptors.find(binding.name);
        if (shared_it != _shared_descriptors.end()) {
            shared_it->second.ref_count++;
            return shared_it->second.buffer;
        }

        // No allocation was found, create a new one
        SharedDescriptor shared;
        shared.ref_count = 1;
        shared.buffer = _memory.allocate_buffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, UNIFORM_MEMORY_PROPERTIES, size);
        _shared_descriptors.emplace(binding.name, shared);
        return shared.buffer;
    }

    unsigned UniformRegistry::allocate_push_constant_range(const PushConstantRange &range) {
        // Not shared, allocate a new buffer
        if (!range.shared) {
            return _push_constant_buffer.reserve(range.block.size).value();
        }

        // If shared, find the allocation and increase ref count
        auto shared_it = _shared_push_constants.find(range.name);
        if (shared_it != _shared_push_constants.end()) {
            shared_it->second.ref_count++;
            return shared_it->second.offset;
        }

        // No allocation was found, create a new one
        SharedPushConstant shared;
        shared.ref_count = 1;
        shared.offset = _push_constant_buffer.reserve(range.block.size).value();
        _shared_push_constants.emplace(range.name, shared);
        return shared.offset;
    }

    void UniformRegistry::free_uniform(const UniformInstance &var) {
        switch (var.type) {
        case UniformType::Descriptor: {
            auto shared_it = _shared_descriptors.find(var.name);
            if (var.descriptor.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                if (shared_it == _shared_descriptors.end() || shared_it->second.ref_count == 1) {
                    _memory.free_buffer(var.descriptor.buffer);
                } else if (shared_it != _shared_descriptors.end()) {
                    shared_it->second.ref_count--;
                }
            }
            break;
        }
        case UniformType::PushConstant: {
            auto shared_it = _shared_push_constants.find(var.name);
            if (shared_it == _shared_push_constants.end() || shared_it->second.ref_count == 1) {
                _push_constant_buffer.free(var.push_constant.offset);
            } else if (shared_it != _shared_push_constants.end()) {
                shared_it->second.ref_count--;
            }
            break;
        }
        }
    }

    void UniformRegistry::free_group(const UniformGroupInstance &group) {
        // Free the descriptor sets
        for (const VirtualDescriptorSet &set : group.v_sets) {
            _descriptors.free_descriptor_set(set);
        }

        // Free uniform variables
        for (const Uniform uniform : group.uniforms) {
            const UniformInstance &var = _uniforms.get(uniform);
            free_uniform(var);
            _uniforms.remove(uniform);
        }
    }

    UniformGroup UniformRegistry::build(const std::vector<DescriptorSetLayout> &descriptor_set_layouts,
                                        const std::vector<PushConstantRange> &push_constant_ranges) {
        UniformGroupInstance group;
        for (const DescriptorSetLayout &layout : descriptor_set_layouts) {
            VirtualDescriptorSet v_set = _descriptors.allocate_descriptor_set(layout.handle);
            group.v_sets.push_back(v_set);
            group.descriptor_sets.push_back(v_set.set);

            for (const DescriptorBinding &binding : layout.bindings) {
                UniformInstance var;
                var.name = binding.name;
                var.type = UniformType::Descriptor;
                var.descriptor.type = binding.type;
                var.descriptor.set = v_set.set;
                var.descriptor.binding = binding.binding;
                var.descriptor.size = binding.size;
                var.descriptor.count = binding.count;

                // Allocate uniform buffers
                if (binding.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                    var.descriptor.buffer = allocate_descriptor_binding(v_set.set, binding);
                    for (unsigned i = 0; i < binding.count; i++) {
                        VkDescriptorBufferInfo buffer_info;
                        buffer_info.buffer = var.descriptor.buffer.buffer;
                        buffer_info.offset = var.descriptor.buffer.offset + i * binding.size;
                        buffer_info.range = binding.size;

                        VkWriteDescriptorSet write = {};
                        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                        write.descriptorType = binding.type;
                        write.dstSet = v_set.set;
                        write.dstBinding = binding.binding;
                        write.dstArrayElement = i;
                        write.descriptorCount = 1;
                        write.pBufferInfo = &buffer_info;
                        vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
                    }
                }

                group.uniforms.push_back(_uniforms.insert(var));
            }
        }
        for (const PushConstantRange &range : push_constant_ranges) {
            UniformInstance var;
            var.name = range.name;
            var.type = UniformType::PushConstant;
            var.push_constant.size = range.block.size;
            var.push_constant.offset = allocate_push_constant_range(range);
            group.uniforms.push_back(_uniforms.insert(var));

            group.push_constant_ranges.push_back(range.block);
            group.push_constant_offsets.push_back(var.push_constant.offset);
        }

        return _groups.insert(group);
    }

    const UniformGroupInstance &UniformRegistry::get(UniformGroup group) const { return _groups.get(group); }

    std::optional<Uniform> UniformRegistry::find(UniformGroup group, const std::string &uniform_name) const {
        const UniformGroupInstance &instance = _groups.get(group);
        for (const Uniform uniform : instance.uniforms) {
            const UniformInstance &var = _uniforms.get(uniform);
            if (var.name == uniform_name) {
                return uniform;
            }
        }
        return {};
    }

    void *UniformRegistry::get_push_constant_data(unsigned block_offset) {
        return _push_constant_buffer.mapped(block_offset);
    }

    void UniformRegistry::write(Uniform uniform, void *data, unsigned index, unsigned count) {
        const UniformInstance &var = _uniforms.get(uniform);
        switch (var.type) {
        case UniformType::Descriptor: {
            char *dst = static_cast<char *>(var.descriptor.buffer.mapped);
            std::memcpy(dst + index * var.descriptor.size, data, var.descriptor.size * count);
            break;
        }
        case UniformType::PushConstant: {
            char *dst = static_cast<char *>(_push_constant_buffer.mapped(var.push_constant.offset));
            std::memcpy(dst + index * var.push_constant.size, data, var.push_constant.size * count);
            break;
        }
        }
    }

    void UniformRegistry::bind(Uniform uniform, const TextureInstance &texture, unsigned index) {
        UniformInstance &var = _uniforms.get(uniform);

        VkDescriptorImageInfo image_info;
        image_info.imageView = texture.view;
        image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        image_info.sampler = texture.sampler;

        VkWriteDescriptorSet write = {};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.dstSet = var.descriptor.set;
        write.dstBinding = var.descriptor.binding;
        write.dstArrayElement = index;
        write.descriptorCount = 1;
        write.pImageInfo = &image_info;

        vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
    }

    void UniformRegistry::destroy(UniformGroup group) {
        const UniformGroupInstance &instance = _groups.get(group);
        free_group(instance);
        _groups.remove(group);
    }
} // namespace Dynamo::Graphics::Vulkan