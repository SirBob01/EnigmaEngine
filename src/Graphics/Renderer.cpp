#include <Graphics/Renderer.hpp>

namespace Dynamo::Graphics {
    Renderer::Renderer(const Display &display, const std::string &root_asset_directory) : _display(display) {
        _instance = VkInstance_create(_display);
#ifdef DYN_DEBUG
        _debugger = VkDebugUtilsMessengerEXT_create(_instance);
#endif
        _surface = _display.create_vulkan_surface(_instance);

        // Create the logical device
        _physical = PhysicalDevice::select_best(_instance, _surface);
        _device = VkDevice_create(_physical);

        // Build the swapchain and its views
        _swapchain = Swapchain(_device, _physical, _display);

        // Command buffer pools for each queue family
        _graphics_pool = VkCommandPool_create(_device, _physical.graphics_queues);
        _transfer_pool = VkCommandPool_create(_device, _physical.transfer_queues);

        // Vulkan object registries
        _memory = MemoryPool(_device, _physical);
        _shaders = ShaderRegistry(_device);
        _meshes = MeshRegistry(_device, _physical, _transfer_pool);
        _uniforms = UniformRegistry(_device, _physical, _transfer_pool);
        _textures = TextureRegistry(_device, _physical, _transfer_pool);
        _pipelines = PipelineRegistry(_device, _physical, root_asset_directory + "/vulkan_cache.bin");

        // Setup the color buffer
        TextureDescriptor color_descriptor;
        color_descriptor.width = _swapchain.extent.width;
        color_descriptor.height = _swapchain.extent.height;
        color_descriptor.usage = TextureUsage::ColorTarget;
        color_descriptor.format = TextureFormat::ColorSurface;
        _color_texture = build_texture(color_descriptor);

        // Setup the depth buffer
        TextureDescriptor depth_stencil_descriptor;
        depth_stencil_descriptor.width = _swapchain.extent.width;
        depth_stencil_descriptor.height = _swapchain.extent.height;
        depth_stencil_descriptor.usage = TextureUsage::DepthStencilTarget;
        depth_stencil_descriptor.format = TextureFormat::DepthSurface;
        _depth_stencil_texture = build_texture(depth_stencil_descriptor);

        // Renderpass and framebuffers
        VkFormat surface_color_format = _swapchain.surface_format.format;
        VkFormat surface_depth_format = _physical.depth_format;
        _forwardpass = VkRenderPass_create(_device, _physical.samples, surface_color_format, surface_depth_format);
        rebuild_framebuffers();

        // Frame contexts
        _frame_contexts = FrameContextList(_device, _graphics_pool);

        // Color fill clear value
        _clear[0].color.float32[0] = 0;
        _clear[0].color.float32[1] = 0;
        _clear[0].color.float32[2] = 0;
        _clear[0].color.float32[3] = 1;

        // Depth-stencil clear value
        _clear[1].depthStencil.depth = 1;
        _clear[1].depthStencil.stencil = 0;
    }

    Renderer::~Renderer() {
        // Wait for device queues to finish processing
        vkDeviceWaitIdle(_device);

        // Cache built pipelines
        _pipelines.write_to_disk();

        // High-level objects
        _frame_contexts.destroy();
        _pipelines.destroy();
        _textures.destroy(_memory);
        _uniforms.destroy(_memory);
        _meshes.destroy(_memory);
        _shaders.destroy();
        _memory.destroy();
        _swapchain.destroy();

        // Vulkan core objects
        for (VkFramebuffer framebuffer : _framebuffers) {
            vkDestroyFramebuffer(_device, framebuffer, nullptr);
        }
        vkDestroyRenderPass(_device, _forwardpass, nullptr);
        vkDestroyCommandPool(_device, _graphics_pool, nullptr);
        vkDestroyCommandPool(_device, _transfer_pool, nullptr);
        vkDestroyDevice(_device, nullptr);
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
#ifdef DYN_DEBUG
        vkDestroyDebugUtilsMessengerEXT(_instance, _debugger, nullptr);
#endif
        vkDestroyInstance(_instance, nullptr);
    }

    void Renderer::rebuild_framebuffers() {
        for (VkFramebuffer framebuffer : _framebuffers) {
            vkDestroyFramebuffer(_device, framebuffer, nullptr);
        }
        _framebuffers.clear();

        for (VkImageView view : _swapchain.views) {
            std::array<VkImageView, 3> views;
            unsigned count = 0;
            if (_physical.samples != VK_SAMPLE_COUNT_1_BIT) {
                views[count++] = _textures.get(_color_texture).view;
                views[count++] = _textures.get(_depth_stencil_texture).view;
                views[count++] = view;
            } else {
                views[count++] = view;
                views[count++] = _textures.get(_depth_stencil_texture).view;
            }
            _framebuffers.push_back(VkFramebuffer_create(_device,
                                                         _forwardpass,
                                                         _swapchain.extent,
                                                         views.begin(),
                                                         count,
                                                         _swapchain.array_layers));
        }
    }

    void Renderer::rebuild_swapchain() {
        vkDeviceWaitIdle(_device);

        // Rebuild the swapchain
        _swapchain = Swapchain(_device, _physical, _display, _swapchain);

        // Rebuild the color texture
        TextureDescriptor color_descriptor;
        color_descriptor.width = _swapchain.extent.width;
        color_descriptor.height = _swapchain.extent.height;
        color_descriptor.usage = TextureUsage::ColorTarget;
        color_descriptor.format = TextureFormat::ColorSurface;
        destroy_texture(_color_texture);
        _color_texture = build_texture(color_descriptor);

        // Rebuild the depth-stencil texture
        TextureDescriptor depth_stencil_descriptor;
        depth_stencil_descriptor.width = _swapchain.extent.width;
        depth_stencil_descriptor.height = _swapchain.extent.height;
        depth_stencil_descriptor.usage = TextureUsage::DepthStencilTarget;
        depth_stencil_descriptor.format = TextureFormat::DepthSurface;
        destroy_texture(_depth_stencil_texture);
        _depth_stencil_texture = build_texture(depth_stencil_descriptor);

        // Rebuild framebuffers
        rebuild_framebuffers();
    }

    void Renderer::set_clear(Color color, float depth, unsigned stencil) {
        _clear[0].color.float32[0] = color.r;
        _clear[0].color.float32[1] = color.g;
        _clear[0].color.float32[2] = color.b;
        _clear[0].color.float32[3] = color.a;

        _clear[1].depthStencil.depth = depth;
        _clear[1].depthStencil.stencil = stencil;
    }

    Mesh Renderer::build_mesh(const MeshDescriptor &descriptor) { return _meshes.build(descriptor, _memory); }

    void Renderer::destroy_mesh(Mesh mesh) { _meshes.destroy(mesh, _memory); }

    Shader Renderer::build_shader(const ShaderDescriptor &descriptor) { return _shaders.build(descriptor); }

    void Renderer::destroy_shader(Shader shader) { _shaders.destroy(shader); }

    Texture Renderer::build_texture(const TextureDescriptor &descriptor) {
        return _textures.build(descriptor, _swapchain, _memory);
    }

    void Renderer::destroy_texture(Texture texture) { _textures.destroy(texture, _memory); }

    Pipeline Renderer::build_pipeline(const PipelineDescriptor &descriptor) {
        return _pipelines.build(descriptor, _forwardpass, _swapchain, _shaders, _uniforms, _memory);
    }

    void Renderer::destroy_pipeline(Pipeline pipeline) {
        // Free allocated descriptor / push constant uniforms
        PipelineInstance &instance = _pipelines.get(pipeline);
        for (Uniform uniform : instance.uniforms) {
            _uniforms.destroy(uniform, _memory);
        }
    }

    std::optional<Uniform> Renderer::get_uniform(Pipeline pipeline, const std::string &name) {
        PipelineInstance &instance = _pipelines.get(pipeline);
        for (Uniform uniform : instance.uniforms) {
            const UniformVariable &var = _uniforms.get(uniform);
            if (var.name == name) {
                return uniform;
            }
        }
        return {};
    }

    void Renderer::write_uniform(Uniform uniform, void *data, unsigned index, unsigned count) {
        _uniforms.write(uniform, data, index, count);
    }

    void Renderer::bind_texture(Uniform uniform, Texture texture, unsigned index) {
        const TextureInstance &instance = _textures.get(texture);
        _uniforms.bind(uniform, instance, index);
    }

    void Renderer::draw(const Model &model) { _models.push_back(model); }

    void Renderer::render() {
        const FrameContext &frame = _frame_contexts.next();
        vkWaitForFences(_device, 1, &frame.sync_fence, VK_TRUE, UINT64_MAX);

        unsigned image_index;
        VkResult acquire_result = vkAcquireNextImageKHR(_device,
                                                        _swapchain.handle,
                                                        UINT64_MAX,
                                                        frame.sync_render_start,
                                                        VK_NULL_HANDLE,
                                                        &image_index);
        if (acquire_result == VK_ERROR_OUT_OF_DATE_KHR) {
            rebuild_swapchain();
            return;
        } else if (acquire_result != VK_SUCCESS && acquire_result != VK_SUBOPTIMAL_KHR) {
            VkResult_check("Acquire Image", acquire_result);
        }

        VkResult_check("Reset Fence", vkResetFences(_device, 1, &frame.sync_fence));
        VkResult_check("Reset Command Buffer", vkResetCommandBuffer(frame.command_buffer, 0));

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        begin_info.pInheritanceInfo = nullptr;

        VkResult_check("Begin Command Recording", vkBeginCommandBuffer(frame.command_buffer, &begin_info));

        // Sort models by group, then pipeline, then geometry
        std::sort(_models.begin(), _models.end(), [](const Model &a, const Model &b) {
            return a.group < b.group ||
                   (a.group == b.group && (a.pipeline < b.pipeline || (a.pipeline == b.pipeline && a.mesh < b.mesh)));
        });

        // Submit commands
        VkViewport viewport;
        viewport.minDepth = 0;
        viewport.maxDepth = 1;
        viewport.width = _swapchain.extent.width;
        viewport.height = _swapchain.extent.height;
        viewport.x = 0;
        viewport.y = 0;
        vkCmdSetViewport(frame.command_buffer, 0, 1, &viewport);

        VkRect2D scissor;
        scissor.extent = _swapchain.extent;
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        vkCmdSetScissor(frame.command_buffer, 0, 1, &scissor);

        VkRenderPassBeginInfo renderpass_begin_info = {};
        renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderpass_begin_info.renderPass = _forwardpass;
        renderpass_begin_info.renderArea.extent = _swapchain.extent;
        renderpass_begin_info.renderArea.offset.x = 0;
        renderpass_begin_info.renderArea.offset.y = 0;
        renderpass_begin_info.clearValueCount = _clear.size();
        renderpass_begin_info.pClearValues = _clear.data();
        renderpass_begin_info.framebuffer = _framebuffers[image_index];
        vkCmdBeginRenderPass(frame.command_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        VkPipeline prev_pipeline = VK_NULL_HANDLE;
        Mesh prev_mesh = reinterpret_cast<Mesh>(-1);

        for (Model model : _models) {
            const MeshInstance &mesh = _meshes.get(model.mesh);
            const PipelineInstance &pipeline = _pipelines.get(model.pipeline);

            // Rebind pipeline if changed
            if (prev_pipeline != pipeline.handle) {
                prev_pipeline = pipeline.handle;
                vkCmdBindPipeline(frame.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.handle);
            }

            // Rebind mesh if changed
            if (prev_mesh != model.mesh) {
                prev_mesh = model.mesh;
                vkCmdBindVertexBuffers(frame.command_buffer,
                                       0,
                                       mesh.attribute_offsets.size(),
                                       mesh.buffers.data(),
                                       mesh.attribute_offsets.data());
                if (mesh.index_type != VK_INDEX_TYPE_NONE_KHR) {
                    vkCmdBindIndexBuffer(frame.command_buffer, mesh.index_buffer, mesh.index_offset, mesh.index_type);
                }
            }

            // Bind descriptor sets
            // TODO: Avoid costly descriptor set rebinding
            if (pipeline.descriptor_sets.size()) {
                vkCmdBindDescriptorSets(frame.command_buffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipeline.layout,
                                        0,
                                        pipeline.descriptor_sets.size(),
                                        pipeline.descriptor_sets.data(),
                                        0,
                                        nullptr);
            }

            // Push constants
            for (unsigned i = 0; i < pipeline.push_constant_ranges.size(); i++) {
                VkPushConstantRange range = pipeline.push_constant_ranges[i];
                unsigned offset = pipeline.push_constant_offsets[i];
                void *data = _uniforms.get_push_constant_data(offset);
                vkCmdPushConstants(frame.command_buffer,
                                   pipeline.layout,
                                   range.stageFlags,
                                   range.offset,
                                   range.size,
                                   data);
            }

            // Draw
            if (mesh.index_type != VK_INDEX_TYPE_NONE_KHR) {
                vkCmdDrawIndexed(frame.command_buffer, mesh.index_count, mesh.instance_count, 0, 0, 0);
            } else {
                vkCmdDraw(frame.command_buffer, mesh.vertex_count, mesh.instance_count, 0, 0);
            }
        }
        _models.clear();

        // End renderpass
        vkCmdEndRenderPass(frame.command_buffer);
        VkResult_check("End Command Buffer", vkEndCommandBuffer(frame.command_buffer));

        // Submit commands
        VkQueue queue;
        vkGetDeviceQueue(_device, _physical.graphics_queues.index, 0, &queue);

        VkSubmitInfo submit_info = {};
        VkPipelineStageFlags wait_stage_mask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submit_info.commandBufferCount = 1;
        submit_info.pCommandBuffers = &frame.command_buffer;
        submit_info.waitSemaphoreCount = 1;
        submit_info.pWaitSemaphores = &frame.sync_render_start;
        submit_info.signalSemaphoreCount = 1;
        submit_info.pSignalSemaphores = &frame.sync_render_done;
        submit_info.pWaitDstStageMask = &wait_stage_mask;

        VkResult_check("Graphics Submit", vkQueueSubmit(queue, 1, &submit_info, frame.sync_fence));

        // Present the render
        VkPresentInfoKHR present_info = {};
        present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        present_info.waitSemaphoreCount = 1;
        present_info.pWaitSemaphores = &frame.sync_render_done;
        present_info.swapchainCount = 1;
        present_info.pSwapchains = &_swapchain.handle;
        present_info.pImageIndices = &image_index;
        present_info.pResults = nullptr;

        VkResult present_result = vkQueuePresentKHR(queue, &present_info);

        if (present_result == VK_ERROR_OUT_OF_DATE_KHR || present_result == VK_SUBOPTIMAL_KHR) {
            rebuild_swapchain();
        } else if (present_result != VK_SUCCESS) {
            VkResult_check("Present Render", present_result);
        }
    }
} // namespace Dynamo::Graphics