#include <Graphics/Renderer.hpp>

namespace Dynamo::Graphics {
    Renderer::Renderer(const Display &display, const std::string &root_asset_directory) : _display(display) {
        _instance = VkInstance_create(_display);
#ifdef DYN_DEBUG
        _debugger = VkDebugUtilsMessengerEXT_create(_instance);
#endif
        _surface = _display.create_vulkan_surface(_instance);

        // Create the logical device
        _physical = PhysicalDevice::select(_instance, _surface);
        _device = VkDevice_create(_physical);

        // Build the swapchain and its views
        _swapchain = Swapchain(_device, _physical, _display);

        // Command buffer pools for each queue family
        _graphics_pool = VkCommandPool_create(_device, _physical.graphics_queues);
        _transfer_pool = VkCommandPool_create(_device, _physical.transfer_queues);

        // Vulkan object registries
        _meshes = MeshRegistry(_device, _physical, _transfer_pool);
        _shaders = ShaderRegistry(_device);
        _materials = MaterialRegistry(_device, root_asset_directory + "/vulkan_cache.bin");
        _uniforms = UniformRegistry(_device, _physical, _transfer_pool);
        _framebuffers = FramebufferCache(_device);

        // Frame contexts
        _frame_contexts = FrameContextList(_device, _graphics_pool);

        // Color fill clear value
        _clear.color.float32[0] = 0;
        _clear.color.float32[1] = 0;
        _clear.color.float32[2] = 0;
        _clear.color.float32[3] = 1;
    }

    Renderer::~Renderer() {
        // Wait for device queues to finish processing
        vkDeviceWaitIdle(_device);

        // Cache built pipelines
        _materials.write_to_disk();

        // High-level objects
        _frame_contexts.destroy();
        _framebuffers.destroy();
        _uniforms.destroy();
        _materials.destroy();
        _shaders.destroy();
        _meshes.destroy();
        _swapchain.destroy();

        // Vulkan core objects
        vkDestroyCommandPool(_device, _graphics_pool, nullptr);
        vkDestroyCommandPool(_device, _transfer_pool, nullptr);
        vkDestroyDevice(_device, nullptr);
        vkDestroySurfaceKHR(_instance, _surface, nullptr);
#ifdef DYN_DEBUG
        vkDestroyDebugUtilsMessengerEXT(_instance, _debugger, nullptr);
#endif
        vkDestroyInstance(_instance, nullptr);
    }

    void Renderer::rebuild_swapchain() {
        // Destroy old swapchain resources
        vkDeviceWaitIdle(_device);
        _framebuffers.destroy();

        // Rebuild the swapchain
        _swapchain = Swapchain(_device, _physical, _display, _swapchain);
    }

    void Renderer::set_clear(Color color) {
        _clear.color.float32[0] = color.r;
        _clear.color.float32[1] = color.g;
        _clear.color.float32[2] = color.b;
        _clear.color.float32[3] = color.a;
    }

    Mesh Renderer::build_mesh(const MeshDescriptor &descriptor) { return _meshes.build(descriptor); }

    void Renderer::destroy_mesh(Mesh mesh) { _meshes.destroy(mesh); }

    Shader Renderer::build_shader(const ShaderDescriptor &descriptor) { return _shaders.build(descriptor); }

    void Renderer::destroy_shader(Shader shader) { _shaders.destroy(shader); }

    Material Renderer::build_material(const MaterialDescriptor &descriptor) {
        return _materials.build(descriptor, _swapchain, _shaders, _uniforms);
    }

    void Renderer::destroy_material(Material material) { _materials.destroy(material, _uniforms); }

    std::optional<Uniform> Renderer::get_uniform(Material material, const std::string &name) {
        MaterialInstance &instance = _materials.get(material);
        for (Uniform uniform : instance.uniforms) {
            const UniformVariable &var = _uniforms.get(uniform);
            if (var.name == name) {
                return uniform;
            }
        }
        return {};
    }

    void Renderer::write_uniform(Uniform uniform, void *data) { _uniforms.write(uniform, data); }

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
            VkResult_log("Acquire Image", acquire_result);
        }

        VkResult_log("Reset Fence", vkResetFences(_device, 1, &frame.sync_fence));
        VkResult_log("Reset Command Buffer", vkResetCommandBuffer(frame.command_buffer, 0));

        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags = 0;
        begin_info.pInheritanceInfo = nullptr;

        VkResult_log("Begin Command Recording", vkBeginCommandBuffer(frame.command_buffer, &begin_info));

        // Group models by material and geometry
        std::sort(_models.begin(), _models.end(), [](const Model &a, const Model &b) {
            return (a.material < b.material) || (a.mesh < b.mesh);
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

        VkRenderPass prev_renderpass = VK_NULL_HANDLE;
        VkPipeline prev_pipeline = VK_NULL_HANDLE;
        Mesh prev_mesh = reinterpret_cast<Mesh>(-1);
        for (Model model : _models) {
            const MeshAllocation &mesh = _meshes.get(model.mesh);
            const MaterialInstance &material = _materials.get(model.material);

            // Rebind renderpass if changed
            if (prev_renderpass != material.renderpass) {
                FramebufferSettings framebuffer_settings;
                framebuffer_settings.view = _swapchain.views[image_index];
                framebuffer_settings.extent = _swapchain.extent;
                framebuffer_settings.renderpass = material.renderpass;
                VkFramebuffer framebuffer = _framebuffers.get(framebuffer_settings);

                VkRenderPassBeginInfo renderpass_begin_info = {};
                renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                renderpass_begin_info.renderPass = material.renderpass;
                renderpass_begin_info.renderArea.extent = _swapchain.extent;
                renderpass_begin_info.renderArea.offset.x = 0;
                renderpass_begin_info.renderArea.offset.y = 0;
                renderpass_begin_info.clearValueCount = 1;
                renderpass_begin_info.pClearValues = &_clear;
                renderpass_begin_info.framebuffer = framebuffer;

                // End the previous renderpass if active
                if (prev_renderpass != VK_NULL_HANDLE) {
                    vkCmdEndRenderPass(frame.command_buffer);
                }
                prev_renderpass = material.renderpass;
                vkCmdBeginRenderPass(frame.command_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            }

            // Rebind pipeline if changed
            if (prev_pipeline != material.pipeline) {
                prev_pipeline = material.pipeline;
                vkCmdBindPipeline(frame.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, material.pipeline);
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
            if (material.descriptor_sets.size()) {
                vkCmdBindDescriptorSets(frame.command_buffer,
                                        VK_PIPELINE_BIND_POINT_GRAPHICS,
                                        material.layout,
                                        0,
                                        material.descriptor_sets.size(),
                                        material.descriptor_sets.data(),
                                        0,
                                        nullptr);
            }

            // Push constants
            for (unsigned i = 0; i < material.push_constant_ranges.size(); i++) {
                VkPushConstantRange range = material.push_constant_ranges[i];
                unsigned offset = material.push_constant_offsets[i];
                void *data = _uniforms.get_push_constant_data(offset);
                vkCmdPushConstants(frame.command_buffer,
                                   material.layout,
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

        // End last renderpass
        if (prev_renderpass != VK_NULL_HANDLE) {
            vkCmdEndRenderPass(frame.command_buffer);
        }
        VkResult_log("End Command Buffer", vkEndCommandBuffer(frame.command_buffer));

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

        VkResult_log("Graphics Submit", vkQueueSubmit(queue, 1, &submit_info, frame.sync_fence));

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
            VkResult_log("Present Render", present_result);
        }
    }
} // namespace Dynamo::Graphics