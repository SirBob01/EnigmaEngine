#include <Graphics/Vulkan/UniformRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    UniformRegistry::UniformRegistry(VkDevice device,
                                     const PhysicalDevice &physical,
                                     MemoryPool &memory,
                                     VkCommandPool transfer_pool) :
        _device(device),
        _memory(memory) {
        std::array<VkDescriptorPoolSize, 2> sizes = {
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1024},
            VkDescriptorPoolSize{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1024},
        };
        _pool = VkDescriptorPool_create(device, sizes.data(), sizes.size(), 1024);

        // Limit of 128 bytes for push constants
        _push_constant_buffer = VirtualMemory(128);
    }

    UniformRegistry::~UniformRegistry() {
        vkDestroyDescriptorPool(_device, _pool, nullptr);
        _variables.foreach ([&](UniformVariable &var) { free_allocation(var); });
        _variables.clear();
    }

    VirtualBuffer UniformRegistry::allocate_uniform_buffer(VkDescriptorSet descriptor_set, DescriptorBinding &binding) {
        // Allocate shared uniform binding once only
        VirtualBuffer buffer;
        unsigned size = binding.size * binding.count;
        if (binding.shared) {
            auto shared_it = _shared.find(binding.name);
            if (shared_it != _shared.end()) {
                SharedVariable &shared = shared_it->second;
                shared.ref_count++;
                buffer = shared.descriptor_buffer;
            } else {
                buffer = _memory.build(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                       VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                       size);

                SharedVariable shared;
                shared.ref_count = 1;
                shared.descriptor_buffer = buffer;
                _shared.emplace(binding.name, shared);
            }
        } else {
            buffer = _memory.build(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                   size);
        }

        // Write each binding array element
        for (unsigned i = 0; i < binding.count; i++) {
            VkDescriptorBufferInfo buffer_info;
            buffer_info.buffer = buffer.buffer;
            buffer_info.offset = buffer.offset + i * binding.size;
            buffer_info.range = binding.size;

            VkWriteDescriptorSet write = {};
            write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            write.descriptorType = binding.type;
            write.dstSet = descriptor_set;
            write.dstBinding = binding.binding;
            write.dstArrayElement = i;
            write.descriptorCount = 1;
            write.pBufferInfo = &buffer_info;
            vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
        }

        return buffer;
    }

    void UniformRegistry::free_allocation(const UniformVariable &var) {
        switch (var.type) {
        case UniformType::Descriptor: {
            auto shared_it = _shared.find(var.name);
            if (var.descriptor.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                if (shared_it == _shared.end() || shared_it->second.ref_count == 1) {
                    _memory.free(var.descriptor.buffer);
                } else if (shared_it != _shared.end()) {
                    shared_it->second.ref_count--;
                }
            }
            break;
        }
        case UniformType::PushConstant: {
            auto shared_it = _shared.find(var.name);
            if (shared_it == _shared.end() || shared_it->second.ref_count == 1) {
                _push_constant_buffer.free(var.push_constant.offset);
            } else if (shared_it != _shared.end()) {
                shared_it->second.ref_count--;
            }
            break;
        }
        }
    }

    DescriptorAllocation UniformRegistry::allocate(const DescriptorSet &set) {
        // TODO: Recycle descriptor sets that are not used
        // TODO: Need to allocate a new descriptor pool if this fails
        DescriptorAllocation allocation;
        VkDescriptorSet_allocate(_device, _pool, &set.layout, &allocation.descriptor_set, 1);

        // Process uniform bindings
        for (DescriptorBinding binding : set.bindings) {
            UniformVariable var;
            var.name = binding.name;
            var.type = UniformType::Descriptor;
            var.descriptor.type = binding.type;
            var.descriptor.set = allocation.descriptor_set;
            var.descriptor.binding = binding.binding;
            var.descriptor.size = binding.size;
            var.descriptor.count = binding.count;

            // Handle each descriptor type
            if (binding.type == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER) {
                var.descriptor.buffer = allocate_uniform_buffer(allocation.descriptor_set, binding);
            }
            allocation.uniforms.push_back(_variables.insert(var));
        }

        return allocation;
    }

    PushConstantAllocation UniformRegistry::allocate(const PushConstant &push_constant) {
        UniformVariable var;
        var.name = push_constant.name;
        var.type = UniformType::PushConstant;
        var.push_constant.size = push_constant.range.size;

        // Allocate shared uniform once only
        if (push_constant.shared) {
            auto shared_it = _shared.find(push_constant.name);
            if (shared_it != _shared.end()) {
                SharedVariable &shared = shared_it->second;
                shared.ref_count++;
                var.push_constant.offset = shared.push_constant_offset;
            } else {
                var.push_constant.offset = _push_constant_buffer.reserve(var.push_constant.size);

                SharedVariable shared;
                shared.ref_count = 1;
                shared.push_constant_offset = var.push_constant.offset;
                _shared.emplace(push_constant.name, shared);
            }
        } else {
            var.push_constant.offset = _push_constant_buffer.reserve(var.push_constant.size);
        }

        PushConstantAllocation allocation;
        allocation.uniform = _variables.insert(var);
        allocation.range = push_constant.range;
        allocation.block_offset = var.push_constant.offset;

        return allocation;
    }

    const UniformVariable &UniformRegistry::get(Uniform uniform) { return _variables.get(uniform); }

    void *UniformRegistry::get_push_constant_data(unsigned block_offset) {
        return _push_constant_buffer.get_mapped(block_offset);
    }

    void UniformRegistry::write(Uniform uniform, void *data, unsigned index, unsigned count) {
        const UniformVariable &var = _variables.get(uniform);
        switch (var.type) {
        case UniformType::Descriptor: {
            char *dst = static_cast<char *>(var.descriptor.buffer.mapped);
            std::memcpy(dst + index * var.descriptor.size, data, var.descriptor.size * count);
            break;
        }
        case UniformType::PushConstant: {
            char *dst = static_cast<char *>(_push_constant_buffer.get_mapped(var.push_constant.offset));
            std::memcpy(dst + index * var.push_constant.size, data, var.push_constant.size * count);
            break;
        }
        }
    }

    void UniformRegistry::bind(Uniform uniform, const TextureInstance &texture, unsigned index) {
        UniformVariable &var = _variables.get(uniform);

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

    void UniformRegistry::destroy(Uniform uniform) {
        const UniformVariable &var = _variables.get(uniform);
        free_allocation(var);
        _variables.remove(uniform);
    }
} // namespace Dynamo::Graphics::Vulkan