#include <Graphics/Vulkan/UniformRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    UniformRegistry::UniformRegistry(VkDevice device, const PhysicalDevice &physical, VkCommandPool transfer_pool) :
        _device(device) {
        std::array<VkDescriptorPoolSize, 1> sizes;
        sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        sizes[0].descriptorCount = 1024;

        _pool = VkDescriptorPool_create(device, sizes.data(), sizes.size(), 1024);

        VkCommandBuffer transfer_buffer;
        VkCommandBuffer_allocate(_device, transfer_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, &transfer_buffer, 1);
        _uniform_buffer = Buffer(_device,
                                 physical,
                                 transfer_buffer,
                                 VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // Minimum of 256 bytes for push constants
        _push_constant_buffer = VirtualMemory(256);
    }

    DescriptorAllocation UniformRegistry::allocate(const DescriptorSet &set) {
        DescriptorAllocation allocation;

        // TODO: Recycle descriptor sets that are not used
        // TODO: Need to allocate a new descriptor pool if this fails
        VkDescriptorSet_allocate(_device, _pool, &set.layout, &allocation.set, 1);

        // Map descriptors to buffer
        for (DescriptorBinding binding : set.bindings) {
            unsigned block_size = binding.size * binding.descriptor_count;

            // Allocate shared uniform once only
            unsigned block_offset;
            if (binding.shared) {
                auto shared_it = _shared_offsets.find(binding.name);
                if (shared_it != _shared_offsets.end()) {
                    block_offset = shared_it->second;
                } else {
                    block_offset = _uniform_buffer.reserve(block_size);
                    _shared_offsets.emplace(binding.name, block_offset);
                }
            } else {
                block_offset = _uniform_buffer.reserve(block_size);
            }

            // Write each binding array element
            for (unsigned i = 0; i < binding.descriptor_count; i++) {
                VkDescriptorBufferInfo buffer_info;
                buffer_info.buffer = _uniform_buffer.handle();
                buffer_info.offset = block_offset + i * binding.size;
                buffer_info.range = binding.size;

                VkWriteDescriptorSet write = {};
                write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                write.dstSet = allocation.set;
                write.dstBinding = binding.binding;
                write.dstArrayElement = i;
                write.descriptorCount = 1;
                write.pBufferInfo = &buffer_info;
                vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
            }

            UniformVariable var;
            var.name = binding.name;
            var.type = UniformVariableType::Descriptor;
            var.block_offset = block_offset;
            var.block_size = block_size;
            allocation.uniforms.push_back(_variables.insert(var));
        }

        return allocation;
    }

    PushConstantAllocation UniformRegistry::allocate(const PushConstant &push_constant) {
        UniformVariable var;
        var.name = push_constant.name;
        var.type = UniformVariableType::PushConstant;
        var.block_size = push_constant.range.size;

        // Allocate shared uniform once only
        if (push_constant.shared) {
            auto shared_it = _shared_offsets.find(push_constant.name);
            if (shared_it != _shared_offsets.end()) {
                var.block_offset = shared_it->second;
            } else {
                var.block_offset = _push_constant_buffer.reserve(var.block_size);
                _shared_offsets.emplace(push_constant.name, var.block_offset);
            }
        } else {
            var.block_offset = _push_constant_buffer.reserve(var.block_size);
        }

        PushConstantAllocation allocation;
        allocation.uniform = _variables.insert(var);
        allocation.range = push_constant.range;
        allocation.block_offset = var.block_offset;

        return allocation;
    }

    const UniformVariable &UniformRegistry::get(Uniform uniform) { return _variables.get(uniform); }

    void *UniformRegistry::get_push_constant_data(unsigned block_offset) {
        return _push_constant_buffer.get_mapped(block_offset);
    }

    void UniformRegistry::write(Uniform uniform, void *data) {
        const UniformVariable &var = _variables.get(uniform);
        void *ptr = nullptr;
        switch (var.type) {
        case UniformVariableType::Descriptor:
            ptr = _uniform_buffer.get_mapped(var.block_offset);
            break;
        case UniformVariableType::PushConstant:
            ptr = _push_constant_buffer.get_mapped(var.block_offset);
            break;
        }
        std::memcpy(ptr, data, var.block_size);
    }

    void UniformRegistry::free(Uniform uniform) {
        const UniformVariable &var = _variables.get(uniform);
        switch (var.type) {
        case UniformVariableType::Descriptor:
            _uniform_buffer.free(var.block_offset);
            break;
        case UniformVariableType::PushConstant:
            _push_constant_buffer.free(var.block_offset);
            break;
        }
        _variables.remove(uniform);
    }

    void UniformRegistry::destroy() {
        vkDestroyDescriptorPool(_device, _pool, nullptr);
        _uniform_buffer.destroy();
    }
} // namespace Dynamo::Graphics::Vulkan