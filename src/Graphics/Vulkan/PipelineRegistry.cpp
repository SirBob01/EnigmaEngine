#include <Graphics/Vulkan/PipelineRegistry.hpp>
#include <Graphics/Vulkan/Utils.hpp>

namespace Dynamo::Graphics::Vulkan {
    PipelineRegistry::PipelineRegistry(VkDevice device, const PhysicalDevice &physical, const std::string &filename) :
        _device(device),
        _physical(physical),
        _pipeline_cache(VkPipelineCache_create(_device, filename)) {
        _ofstream.open(filename, std::ios::trunc | std::ios::binary);
    }

    PipelineRegistry::~PipelineRegistry() {
        // Clean up pipelines
        vkDestroyPipelineCache(_device, _pipeline_cache, nullptr);
        for (const auto &[key, pipeline] : _pipelines) {
            vkDestroyPipeline(_device, pipeline, nullptr);
        }
        _pipelines.clear();

        // Clean up pipeline layouts
        for (const auto &[key, layout] : _layouts) {
            vkDestroyPipelineLayout(_device, layout, nullptr);
        }
        _layouts.clear();

        // Clear all instances
        _instances.clear();
    }

    Pipeline PipelineRegistry::build(const PipelineDescriptor &descriptor,
                                     VkRenderPass renderpass,
                                     const Swapchain &swapchain,
                                     const ShaderRegistry &shaders,
                                     UniformRegistry &uniforms,
                                     MemoryPool &memory) {
        PipelineInstance instance;

        const ShaderModule &vertex_module = shaders.get(descriptor.vertex);
        const ShaderModule &fragment_module = shaders.get(descriptor.fragment);
        std::array<std::reference_wrapper<const ShaderModule>, 2> shader_modules = {
            vertex_module,
            fragment_module,
        };

        // Aggregate unique descriptor set layouts and push constant ranges
        PipelineLayoutSettings layout_settings;
        for (const ShaderModule &module : shader_modules) {
            for (const DescriptorSetLayout &layout : module.descriptor_set_layouts) {
                auto dup_it = std::find_if(layout_settings.descriptor_set_layouts.begin(),
                                           layout_settings.descriptor_set_layouts.end(),
                                           [&](VkDescriptorSetLayout query) { return query == layout.handle; });
                if (dup_it == layout_settings.descriptor_set_layouts.end()) {
                    layout_settings.descriptor_set_layouts.push_back(layout.handle);
                    instance.descriptor_set_layouts.push_back(&layout);
                }
            }
            for (const PushConstantRange &range : module.push_constant_ranges) {
                auto dup_it =
                    std::find_if(layout_settings.push_constant_ranges.begin(),
                                 layout_settings.push_constant_ranges.end(),
                                 [&](VkPushConstantRange &query) { return query.offset == range.block.offset; });
                if (dup_it == layout_settings.push_constant_ranges.end()) {
                    layout_settings.push_constant_ranges.push_back(range.block);
                    instance.push_constant_ranges.push_back(&range);
                }
            }
        }

        // Build pipeline layout
        auto layout_it = _layouts.find(layout_settings);
        if (layout_it != _layouts.end()) {
            instance.layout = layout_it->second;
        } else {
            instance.layout = VkPipelineLayout_create(_device,
                                                      layout_settings.descriptor_set_layouts.data(),
                                                      layout_settings.descriptor_set_layouts.size(),
                                                      layout_settings.push_constant_ranges.data(),
                                                      layout_settings.push_constant_ranges.size());
            _layouts.emplace(layout_settings, instance.layout);
        }

        // Build pipeline
        GraphicsPipelineSettings pipeline_settings;
        pipeline_settings.layout = instance.layout;
        pipeline_settings.vertex = vertex_module.handle;
        pipeline_settings.fragment = fragment_module.handle;
        pipeline_settings.topology = convert_topology(descriptor.topology);
        pipeline_settings.fill = convert_fill(descriptor.fill);
        pipeline_settings.cull = convert_cull(descriptor.cull);
        pipeline_settings.samples = _physical.samples;
        pipeline_settings.color_mask = 0;
        if (descriptor.color_mask.r) {
            pipeline_settings.color_mask |= VK_COLOR_COMPONENT_R_BIT;
        }
        if (descriptor.color_mask.g) {
            pipeline_settings.color_mask |= VK_COLOR_COMPONENT_G_BIT;
        }
        if (descriptor.color_mask.b) {
            pipeline_settings.color_mask |= VK_COLOR_COMPONENT_B_BIT;
        }
        if (descriptor.color_mask.a) {
            pipeline_settings.color_mask |= VK_COLOR_COMPONENT_A_BIT;
        }
        pipeline_settings.depth_test = descriptor.depth_test;
        pipeline_settings.depth_write = descriptor.depth_write;
        pipeline_settings.depth_test_op = convert_compare_op(descriptor.depth_test_op);
        auto pipeline_it = _pipelines.find(pipeline_settings);
        if (pipeline_it != _pipelines.end()) {
            instance.handle = pipeline_it->second;
        } else {
            instance.handle = VkPipeline_create(_device,
                                                _pipeline_cache,
                                                pipeline_settings.layout,
                                                renderpass,
                                                pipeline_settings.vertex,
                                                pipeline_settings.fragment,
                                                pipeline_settings.topology,
                                                pipeline_settings.fill,
                                                pipeline_settings.cull,
                                                pipeline_settings.samples,
                                                pipeline_settings.color_mask,
                                                pipeline_settings.depth_test,
                                                pipeline_settings.depth_write,
                                                pipeline_settings.depth_test_op,
                                                vertex_module.bindings.data(),
                                                vertex_module.bindings.size(),
                                                vertex_module.attributes.data(),
                                                vertex_module.attributes.size());
            _pipelines.emplace(pipeline_settings, instance.handle);
        }

        return _instances.insert(instance);
    }

    PipelineInstance &PipelineRegistry::get(Pipeline pipeline) { return _instances.get(pipeline); }

    void PipelineRegistry::write_to_disk() {
        size_t size;
        vkGetPipelineCacheData(_device, _pipeline_cache, &size, nullptr);
        std::vector<char> buffer(size);
        vkGetPipelineCacheData(_device, _pipeline_cache, &size, buffer.data());

        _ofstream.write(buffer.data(), buffer.size());
    }
} // namespace Dynamo::Graphics::Vulkan