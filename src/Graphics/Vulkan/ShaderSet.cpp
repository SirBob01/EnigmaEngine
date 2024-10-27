#include <Graphics/Vulkan/ShaderSet.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/Log.hpp>
#include <shaderc/shaderc.hpp>

namespace Dynamo::Graphics::Vulkan {
    constexpr char INSTANCE_VAR_PREFIX[] = "instance";

    ShaderSet::ShaderSet(VkDevice device) : _device(device) {}

    std::vector<uint32_t>
    ShaderSet::compile(const std::string &name, const std::string &code, VkShaderStageFlagBits stage, bool optimized) {
        shaderc::Compiler compiler;
        shaderc::CompileOptions options;
        if (optimized) {
            options.SetOptimizationLevel(shaderc_optimization_level_performance);
        }

        shaderc_shader_kind kind;
        switch (stage) {
        case VK_SHADER_STAGE_VERTEX_BIT:
            kind = shaderc_vertex_shader;
            break;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            kind = shaderc_fragment_shader;
            break;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            kind = shaderc_compute_shader;
            break;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            kind = shaderc_tess_control_shader;
            break;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            kind = shaderc_tess_evaluation_shader;
            break;
        default:
            Log::error("Unsupported shader type.");
            return {};
        }

        shaderc::SpvCompilationResult module = compiler.CompileGlslToSpv(code, kind, name.c_str(), options);

        if (module.GetCompilationStatus() != shaderc_compilation_status_success) {
            Log::error("Vulkan Shader Compile Error:\n{}", module.GetErrorMessage());
            return {};
        }

        return {module.cbegin(), module.cend()};
    }

    void ShaderSet::reflect_vertex_input(ShaderModule &module, SpvReflectShaderModule reflection) {
        unsigned count = 0;
        SpvReflectResult result = spvReflectEnumerateInputVariables(&reflection, &count, NULL);
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        std::vector<SpvReflectInterfaceVariable *> shader_inputs(count);
        result = spvReflectEnumerateInputVariables(&reflection, &count, shader_inputs.data());
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        for (unsigned i = 0; i < count; i++) {
            SpvReflectInterfaceVariable &input_var = *shader_inputs[i];
            VkFormat format = static_cast<VkFormat>(input_var.format);
            if (input_var.decoration_flags & SPV_REFLECT_DECORATION_BUILT_IN) {
                continue;
            }

            // Variables with prefix are per-instance
            bool instanced = std::string(input_var.name).rfind(INSTANCE_VAR_PREFIX) == 0;

            VkVertexInputBindingDescription binding = {};
            binding.binding = input_var.location;
            binding.inputRate = instanced ? VK_VERTEX_INPUT_RATE_INSTANCE : VK_VERTEX_INPUT_RATE_VERTEX;
            binding.stride = VkFormat_size(format);
            module.bindings.push_back(binding);

            VkVertexInputAttributeDescription attribute = {};
            attribute.location = input_var.location;
            attribute.binding = input_var.location;
            attribute.format = format;
            attribute.offset = 0;
            module.attributes.push_back(attribute);
        }

        Log::info("* Inputs");
        for (auto &binding : module.bindings) {
            Log::info(" -> Binding (binding: {}, stride: {}, rate: {})",
                      binding.binding,
                      binding.stride,
                      binding.inputRate == VK_VERTEX_INPUT_RATE_INSTANCE ? "per-instance" : "per-vertex");
        }
        for (auto &attribute : module.attributes) {
            Log::info(" -> Attribute (location: {}, binding: {})", attribute.location, attribute.binding);
        }
    }

    void ShaderSet::reflect_descriptor_sets(ShaderModule &module, SpvReflectShaderModule reflection) {
        uint32_t count = 0;
        SpvReflectResult result = spvReflectEnumerateDescriptorSets(&reflection, &count, NULL);
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        std::vector<SpvReflectDescriptorSet *> sets(count);
        result = spvReflectEnumerateDescriptorSets(&reflection, &count, sets.data());
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        for (unsigned i = 0; i < count; i++) {
            const SpvReflectDescriptorSet &refl_set = *sets[i];
            Log::info("* Descriptor Set: {}", refl_set.set);

            std::vector<VkDescriptorSetLayoutBinding> bindings;
            for (unsigned j = 0; j < refl_set.binding_count; j++) {
                const SpvReflectDescriptorBinding &refl_binding = *refl_set.bindings[j];
                VkDescriptorSetLayoutBinding binding;
                binding.binding = refl_binding.binding;
                binding.pImmutableSamplers = nullptr;
                binding.stageFlags = static_cast<VkShaderStageFlagBits>(reflection.shader_stage);
                binding.descriptorType = static_cast<VkDescriptorType>(refl_binding.descriptor_type);
                binding.descriptorCount = 1;
                for (unsigned k = 0; k < refl_binding.array.dims_count; k++) {
                    binding.descriptorCount *= refl_binding.array.dims[k];
                }
                bindings.push_back(binding);

                Log::info(" -> Binding (name: {}, binding: {}, count: {}, type: {}, stage: {})",
                          refl_binding.name,
                          binding.binding,
                          binding.descriptorCount,
                          VkDescriptorType_string(binding.descriptorType),
                          VkShaderStageFlagBits_string(static_cast<VkShaderStageFlagBits>(binding.stageFlags)));
            }

            // Sort the bindings
            std::sort(bindings.begin(), bindings.end(), [](auto &a, auto &b) { return a.binding < b.binding; });

            // Build descriptor set layout
            DescriptorLayoutKey key;
            key.bindings = bindings;

            VkDescriptorSetLayout layout;
            auto layout_it = _descriptor_layouts.find(key);
            if (layout_it != _descriptor_layouts.end()) {
                layout = layout_it->second;
            } else {
                layout = VkDescriptorSetLayout_create(_device, bindings.data(), bindings.size());
                _descriptor_layouts.emplace(key, layout);
            }
            module.descriptor_layouts.push_back(layout);
        }
    }

    void ShaderSet::reflect_push_constants(ShaderModule &module, SpvReflectShaderModule reflection) {
        uint32_t count = 0;
        SpvReflectResult result = spvReflectEnumeratePushConstantBlocks(&reflection, &count, NULL);
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        std::vector<SpvReflectBlockVariable *> push_constants(count);
        result = spvReflectEnumeratePushConstantBlocks(&reflection, &count, push_constants.data());
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        for (unsigned i = 0; i < count; i++) {
            SpvReflectBlockVariable &block = *push_constants[i];

            VkPushConstantRange range;
            range.offset = block.offset;
            range.size = block.size;
            range.stageFlags = static_cast<VkShaderStageFlagBits>(reflection.shader_stage);

            module.push_constant_ranges.push_back(range);
            Log::info("* Push Constant Range (name: {}, offset: {}, size: {}, stage: {})",
                      block.name,
                      range.offset,
                      range.size,
                      VkShaderStageFlagBits_string(static_cast<VkShaderStageFlagBits>(range.stageFlags)));
        }
    }

    Shader ShaderSet::build(const ShaderDescriptor &descriptor) {
        // Determine shader stage
        VkShaderStageFlagBits stage;
        switch (descriptor.stage) {
        case ShaderStage::Vertex:
            stage = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case ShaderStage::Fragment:
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        case ShaderStage::Compute:
            stage = VK_SHADER_STAGE_COMPUTE_BIT;
            break;
        case ShaderStage::TesselationControl:
            stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
            break;
        case ShaderStage::TesselationEvaluation:
            stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
            break;
        }

        // Compile the shader code (optimized and non-optimized for reflection)
        std::vector<uint32_t> bytecode = compile(descriptor.name, descriptor.code, stage, false);
        std::vector<uint32_t> bytecode_optimized = compile(descriptor.name, descriptor.code, stage, true);

        // Create the shader module
        ShaderModule module;
        module.handle = VkShaderModule_create(_device, bytecode_optimized);

        // Reflection
        SpvReflectShaderModule reflection;
        unsigned bytecode_length = bytecode.size() * sizeof(uint32_t);
        SpvReflectResult result = spvReflectCreateShaderModule(bytecode_length, bytecode.data(), &reflection);
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        Log::info("Shader {} reflection:", descriptor.name);
        reflect_vertex_input(module, reflection);
        reflect_descriptor_sets(module, reflection);
        reflect_push_constants(module, reflection);
        spvReflectDestroyShaderModule(&reflection);
        Log::info("");

        // Register the shader module
        return _modules.insert(module);
    }

    const ShaderModule &ShaderSet::get(Shader shader) const { return _modules.get(shader); }

    void ShaderSet::destroy(Shader shader) {
        ShaderModule &module = _modules.get(shader);
        vkDestroyShaderModule(_device, module.handle, nullptr);
        _modules.remove(shader);
    }

    void ShaderSet::destroy() {
        for (const auto &[key, layout] : _descriptor_layouts) {
            vkDestroyDescriptorSetLayout(_device, layout, nullptr);
        }
        _descriptor_layouts.clear();

        _modules.foreach ([&](ShaderModule &module) { vkDestroyShaderModule(_device, module.handle, nullptr); });
        _modules.clear();
    }
} // namespace Dynamo::Graphics::Vulkan