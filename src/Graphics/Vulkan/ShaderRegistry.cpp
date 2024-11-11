#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>
#include <Utils/Log.hpp>
#include <shaderc/shaderc.hpp>

namespace Dynamo::Graphics::Vulkan {
    constexpr char INSTANCE_VAR_PREFIX[] = "instance";
    constexpr VkShaderStageFlags SUPPORTED_SHADER_STAGES =
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_COMPUTE_BIT |
        VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT | VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;

    ShaderRegistry::ShaderRegistry(VkDevice device) : _device(device) {}

    ShaderRegistry::~ShaderRegistry() {
        // Destroy descriptor set layouts
        for (const auto &[key, layout] : _layouts) {
            vkDestroyDescriptorSetLayout(_device, layout, nullptr);
        }
        _layouts.clear();

        // Destroy shader modules
        _modules.foreach ([&](ShaderModule &module) { vkDestroyShaderModule(_device, module.handle, nullptr); });
        _modules.clear();
    }

    std::vector<uint32_t> ShaderRegistry::compile(const std::string &name,
                                                  const std::string &code,
                                                  VkShaderStageFlagBits stage,
                                                  bool optimized) {
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

    void ShaderRegistry::reflect_vertex_input(ShaderModule &module, SpvReflectShaderModule reflection) {
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

    void ShaderRegistry::reflect_descriptor_sets(ShaderModule &module, SpvReflectShaderModule reflection) {
        uint32_t count = 0;
        SpvReflectResult result = spvReflectEnumerateDescriptorSets(&reflection, &count, NULL);
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        std::vector<SpvReflectDescriptorSet *> sets(count);
        result = spvReflectEnumerateDescriptorSets(&reflection, &count, sets.data());
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        for (unsigned i = 0; i < count; i++) {
            const SpvReflectDescriptorSet &refl_set = *sets[i];
            std::sort(refl_set.bindings, refl_set.bindings + refl_set.binding_count, [](auto *a, auto *b) {
                return a->binding < b->binding;
            });

            DescriptorSetLayout layout;
            DescriptorSetLayoutKey key;
            for (unsigned j = 0; j < refl_set.binding_count; j++) {
                // Update descriptor layout key
                const SpvReflectDescriptorBinding &refl_binding = *refl_set.bindings[j];
                VkDescriptorSetLayoutBinding layout_binding;
                layout_binding.binding = refl_binding.binding;
                layout_binding.pImmutableSamplers = nullptr;
                layout_binding.stageFlags = SUPPORTED_SHADER_STAGES;
                layout_binding.descriptorType = static_cast<VkDescriptorType>(refl_binding.descriptor_type);
                layout_binding.descriptorCount = 1;
                for (unsigned k = 0; k < refl_binding.array.dims_count; k++) {
                    layout_binding.descriptorCount *= refl_binding.array.dims[k];
                }
                key.bindings.push_back(layout_binding);

                // Add descriptor metadata to reflection
                DescriptorBinding descriptor_binding;
                descriptor_binding.name = refl_binding.name;
                descriptor_binding.type = layout_binding.descriptorType;
                descriptor_binding.binding = refl_binding.binding;
                descriptor_binding.count = layout_binding.descriptorCount;
                descriptor_binding.size = refl_binding.block.size;
                layout.bindings.push_back(descriptor_binding);

                Log::info("* Descriptor (name: {}, set: {}, binding: {}, size: {}, dim: {}, type: {})",
                          descriptor_binding.name,
                          refl_binding.set,
                          descriptor_binding.binding,
                          descriptor_binding.size,
                          descriptor_binding.count,
                          VkDescriptorType_string(layout_binding.descriptorType));
            }

            // Build descriptor set layout
            auto layout_it = _layouts.find(key);
            if (layout_it != _layouts.end()) {
                layout.handle = layout_it->second;
            } else {
                VkDescriptorSetLayoutBinding *bindings = key.bindings.data();
                unsigned binding_count = key.bindings.size();
                layout.handle = VkDescriptorSetLayout_create(_device, bindings, binding_count);
                _layouts.emplace(key, layout.handle);
            }

            // Register the set
            module.descriptor_set_layouts.push_back(layout);
        }
    }

    void ShaderRegistry::reflect_push_constants(ShaderModule &module, SpvReflectShaderModule reflection) {
        uint32_t count = 0;
        SpvReflectResult result = spvReflectEnumeratePushConstantBlocks(&reflection, &count, NULL);
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        std::vector<SpvReflectBlockVariable *> push_constants(count);
        result = spvReflectEnumeratePushConstantBlocks(&reflection, &count, push_constants.data());
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        for (unsigned i = 0; i < count; i++) {
            SpvReflectBlockVariable &block = *push_constants[i];
            PushConstantRange range;
            range.name = block.name;
            range.block.offset = block.offset;
            range.block.size = block.size;
            range.block.stageFlags = SUPPORTED_SHADER_STAGES;

            module.push_constant_ranges.push_back(range);
            Log::info("* Push Constant Range (name: {}, offset: {}, size: {})",
                      range.name,
                      range.block.offset,
                      range.block.size);
        }
    }

    Shader ShaderRegistry::build(const ShaderDescriptor &descriptor) {
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
        SpvReflectResult result = spvReflectCreateShaderModule(bytecode.size() * 4, bytecode.data(), &reflection);
        DYN_ASSERT(result == SPV_REFLECT_RESULT_SUCCESS);

        Log::info("Shader '{}' reflection:", descriptor.name);
        reflect_vertex_input(module, reflection);
        reflect_descriptor_sets(module, reflection);
        reflect_push_constants(module, reflection);
        spvReflectDestroyShaderModule(&reflection);

        // Register the shader module
        return _modules.insert(module);
    }

    const ShaderModule &ShaderRegistry::get(Shader shader) const { return _modules.get(shader); }

    void ShaderRegistry::destroy(Shader shader) {
        ShaderModule &module = _modules.get(shader);
        vkDestroyShaderModule(_device, module.handle, nullptr);
        _modules.remove(shader);
    }
} // namespace Dynamo::Graphics::Vulkan