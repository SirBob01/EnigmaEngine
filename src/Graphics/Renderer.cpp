#include <Graphics/Renderer.hpp>

namespace Dynamo::Graphics {
    using namespace Vulkan;

    Renderer::Renderer(const Display &display, const std::string &root_asset_directory) :
        _display(display),
        _context(_display),
        _swapchain(_context.device, _context.physical, _display),
        _memory(_context.device, _context.physical),
        _descriptors(_context.device),
        _meshes(_context.device, _context.physical, _memory, _context.transfer_pool),
        _shaders(_context.device),
        _renderpasses(_context.device, _context.physical, _swapchain),
        _pipelines(_context.device, _context.physical, root_asset_directory + "/vulkan_cache.bin"),
        _uniforms(_context.device, _context.physical, _memory, _descriptors, _context.transfer_pool),
        _textures(_context.device, _context.physical, _memory, _context.transfer_pool),
        _frame_contexts(_context.device, _context.graphics_pool),
        _depth_framebuffer(VK_NULL_HANDLE) {
        // Setup the color buffer
        TextureDescriptor color_descriptor;
        color_descriptor.width = _swapchain.extent.width;
        color_descriptor.height = _swapchain.extent.height;
        color_descriptor.usage = TextureUsage::ColorTarget;
        color_descriptor.format = TextureFormat::ColorSurface;
        color_descriptor.samples = TextureSamples::S64;
        _color_texture = build_texture(color_descriptor);

        // Setup the depth buffer
        TextureDescriptor depth_stencil_descriptor;
        depth_stencil_descriptor.width = _swapchain.extent.width;
        depth_stencil_descriptor.height = _swapchain.extent.height;
        depth_stencil_descriptor.usage = TextureUsage::DepthStencilTarget;
        depth_stencil_descriptor.format = TextureFormat::DepthSurface;
        depth_stencil_descriptor.samples = TextureSamples::S64;
        _depth_stencil_texture = build_texture(depth_stencil_descriptor);

        // Build framebuffers
        rebuild_framebuffers();

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
        vkDeviceWaitIdle(_context.device);

        // Cache built pipelines
        _pipelines.write_to_disk();

        // Destroy swapchain and framebuffers
        _swapchain.destroy();
        vkDestroyFramebuffer(_context.device, _depth_framebuffer, nullptr);
        for (VkFramebuffer framebuffer : _shading_framebuffers) {
            vkDestroyFramebuffer(_context.device, framebuffer, nullptr);
        }
    }

    void Renderer::rebuild_framebuffers() {
        if (_depth_framebuffer != VK_NULL_HANDLE) {
            vkDestroyFramebuffer(_context.device, _depth_framebuffer, nullptr);
        }
        for (VkFramebuffer framebuffer : _shading_framebuffers) {
            vkDestroyFramebuffer(_context.device, framebuffer, nullptr);
        }
        _shading_framebuffers.clear();

        VkImageView color_view = _textures.get(_color_texture).view;
        VkImageView depth_stencil_view = _textures.get(_depth_stencil_texture).view;

        // Build depth framebuffer
        _depth_framebuffer = VkFramebuffer_create(_context.device,
                                                  _renderpasses.depth.handle,
                                                  _swapchain.extent,
                                                  &depth_stencil_view,
                                                  1,
                                                  _swapchain.array_layers);

        // Build shading framebuffers
        for (VkImageView view : _swapchain.views) {
            std::array<VkImageView, 3> views;
            unsigned count = 0;
            if (_context.physical.samples != VK_SAMPLE_COUNT_1_BIT) {
                views[count++] = color_view;
                views[count++] = depth_stencil_view;
                views[count++] = view;
            } else {
                views[count++] = view;
                views[count++] = depth_stencil_view;
            }
            _shading_framebuffers.push_back(VkFramebuffer_create(_context.device,
                                                                 _renderpasses.shading.handle,
                                                                 _swapchain.extent,
                                                                 views.begin(),
                                                                 count,
                                                                 _swapchain.array_layers));
        }
    }

    void Renderer::rebuild_swapchain() {
        vkDeviceWaitIdle(_context.device);

        // Rebuild the swapchain
        _swapchain = Swapchain(_context.device, _context.physical, _display, _swapchain);

        // Rebuild the color texture
        TextureDescriptor color_descriptor;
        color_descriptor.width = _swapchain.extent.width;
        color_descriptor.height = _swapchain.extent.height;
        color_descriptor.usage = TextureUsage::ColorTarget;
        color_descriptor.format = TextureFormat::ColorSurface;
        color_descriptor.samples = TextureSamples::S64;
        destroy_texture(_color_texture);
        _color_texture = build_texture(color_descriptor);

        // Rebuild the depth-stencil texture
        TextureDescriptor depth_stencil_descriptor;
        depth_stencil_descriptor.width = _swapchain.extent.width;
        depth_stencil_descriptor.height = _swapchain.extent.height;
        depth_stencil_descriptor.usage = TextureUsage::DepthStencilTarget;
        depth_stencil_descriptor.format = TextureFormat::DepthSurface;
        depth_stencil_descriptor.samples = TextureSamples::S64;
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

    Mesh Renderer::build_mesh(const MeshDescriptor &descriptor) { return _meshes.build(descriptor); }

    void Renderer::destroy_mesh(Mesh mesh) { _meshes.destroy(mesh); }

    Shader Renderer::build_shader(const ShaderDescriptor &descriptor) { return _shaders.build(descriptor); }

    void Renderer::destroy_shader(Shader shader) { _shaders.destroy(shader); }

    Texture Renderer::build_texture(const TextureDescriptor &descriptor) {
        return _textures.build(descriptor, _swapchain);
    }

    void Renderer::destroy_texture(Texture texture) { _textures.destroy(texture); }

    Pipeline Renderer::build_pipeline(const PipelineDescriptor &descriptor) {
        return _pipelines.build(descriptor,
                                _renderpasses.depth.handle,
                                _renderpasses.shading.handle,
                                _swapchain,
                                _shaders,
                                _uniforms,
                                _memory);
    }

    void Renderer::destroy_pipeline(Pipeline pipeline) { _pipelines.destroy(pipeline); }

    UniformGroup Renderer::build_uniforms(Pipeline pipeline) {
        const PipelineInstance &instance = _pipelines.get(pipeline);
        return _uniforms.build(instance.descriptor_set_layouts, instance.push_constant_ranges);
    }

    void Renderer::destroy_uniforms(UniformGroup group) { _uniforms.destroy(group); }

    std::optional<Uniform> Renderer::get_uniform(UniformGroup group, const std::string &name) {
        return _uniforms.find(group, name);
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
        vkWaitForFences(_context.device, 1, &frame.sync_fence, VK_TRUE, UINT64_MAX);

        unsigned image_index;
        VkResult acquire_result = vkAcquireNextImageKHR(_context.device,
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

        VkResult_check("Reset Fence", vkResetFences(_context.device, 1, &frame.sync_fence));
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

        VkRenderPassBeginInfo depth_pass_begin_info = {};
        depth_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        depth_pass_begin_info.renderPass = _renderpasses.depth.handle;
        depth_pass_begin_info.renderArea.extent = _swapchain.extent;
        depth_pass_begin_info.renderArea.offset.x = 0;
        depth_pass_begin_info.renderArea.offset.y = 0;
        depth_pass_begin_info.clearValueCount = 1;
        depth_pass_begin_info.pClearValues = &_clear[1];
        depth_pass_begin_info.framebuffer = _depth_framebuffer;
        vkCmdBeginRenderPass(frame.command_buffer, &depth_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        VkPipeline prev_pipeline = VK_NULL_HANDLE;
        Mesh prev_mesh = reinterpret_cast<Mesh>(-1);

        for (Model model : _models) {
            const MeshInstance &mesh = _meshes.get(model.mesh);
            const PipelineInstance &pipeline = _pipelines.get(model.pipeline);
            const UniformGroupInstance &uniform_group = _uniforms.get(model.uniforms);

            // Rebind pipeline if changed
            if (prev_pipeline != pipeline.depth_pass_pipeline) {
                prev_pipeline = pipeline.depth_pass_pipeline;
                vkCmdBindPipeline(frame.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.depth_pass_pipeline);
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
            if (uniform_group.descriptor_sets.size()) {
                vkCmdBindDescriptorSets(frame.command_buffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipeline.layout,
                                        0,
                                        uniform_group.descriptor_sets.size(),
                                        uniform_group.descriptor_sets.data(),
                                        0,
                                        nullptr);
            }

            // Push constants
            for (unsigned i = 0; i < uniform_group.push_constant_ranges.size(); i++) {
                VkPushConstantRange range = uniform_group.push_constant_ranges[i];
                unsigned offset = uniform_group.push_constant_offsets[i];
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
        vkCmdEndRenderPass(frame.command_buffer);

        VkRenderPassBeginInfo shading_pass_begin_info = {};
        shading_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        shading_pass_begin_info.renderPass = _renderpasses.shading.handle;
        shading_pass_begin_info.renderArea.extent = _swapchain.extent;
        shading_pass_begin_info.renderArea.offset.x = 0;
        shading_pass_begin_info.renderArea.offset.y = 0;
        shading_pass_begin_info.clearValueCount = _clear.size();
        shading_pass_begin_info.pClearValues = _clear.data();
        shading_pass_begin_info.framebuffer = _shading_framebuffers[image_index];
        vkCmdBeginRenderPass(frame.command_buffer, &shading_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

        prev_pipeline = VK_NULL_HANDLE;
        prev_mesh = reinterpret_cast<Mesh>(-1);

        for (Model model : _models) {
            const MeshInstance &mesh = _meshes.get(model.mesh);
            const PipelineInstance &pipeline = _pipelines.get(model.pipeline);
            const UniformGroupInstance &uniform_group = _uniforms.get(model.uniforms);

            // Rebind pipeline if changed
            if (prev_pipeline != pipeline.shading_pass_pipeline) {
                prev_pipeline = pipeline.shading_pass_pipeline;
                vkCmdBindPipeline(frame.command_buffer,
                                  VK_PIPELINE_BIND_POINT_GRAPHICS,
                                  pipeline.shading_pass_pipeline);
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
            if (uniform_group.descriptor_sets.size()) {
                vkCmdBindDescriptorSets(frame.command_buffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        pipeline.layout,
                                        0,
                                        uniform_group.descriptor_sets.size(),
                                        uniform_group.descriptor_sets.data(),
                                        0,
                                        nullptr);
            }

            // Push constants
            for (unsigned i = 0; i < uniform_group.push_constant_ranges.size(); i++) {
                VkPushConstantRange range = uniform_group.push_constant_ranges[i];
                unsigned offset = uniform_group.push_constant_offsets[i];
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
        vkCmdEndRenderPass(frame.command_buffer);

        VkResult_check("End Command Buffer", vkEndCommandBuffer(frame.command_buffer));
        _models.clear();

        // Submit commands
        VkQueue queue;
        vkGetDeviceQueue(_context.device, _context.physical.graphics_queues.index, 0, &queue);

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