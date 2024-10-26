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

        // Main buffers
        std::array<VkCommandBuffer, 3> transfer_buffers;
        VkCommandBuffer_allocate(_device, _transfer_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, transfer_buffers.data(), 3);
        _vertex_buffer = Buffer(_device,
                                _physical,
                                transfer_buffers[0],
                                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _index_buffer = Buffer(_device,
                               _physical,
                               transfer_buffers[1],
                               VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                               VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        _staging_buffer = Buffer(_device,
                                 _physical,
                                 transfer_buffers[2],
                                 VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT |
                                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        // Vulkan object caches
        _shaders = ShaderSet(_device);
        _materials = MaterialRegistry(_device, root_asset_directory + "/vulkan_cache.bin");
        _framebuffers = FramebufferCache(_device);

        // Frame contexts
        _frame_contexts = FrameContextList<3>(_device, _graphics_pool);

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
        _vertex_buffer.destroy();
        _index_buffer.destroy();
        _staging_buffer.destroy();
        _framebuffers.destroy();
        _materials.destroy();
        _shaders.destroy();
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

    Mesh Renderer::build_mesh(const MeshDescriptor &descriptor) {
        return _meshes.build(descriptor, _vertex_buffer, _index_buffer, _staging_buffer);
    }

    void Renderer::destroy_mesh(Mesh mesh) { _meshes.free(mesh, _vertex_buffer, _index_buffer); }

    void Renderer::draw(const Model &model) { _models.push_back(model); }

    Shader Renderer::build_shader(const ShaderDescriptor &descriptor) { return _shaders.build(descriptor); }

    void Renderer::destroy_shader(Shader shader) { _shaders.destroy(shader); }

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

        // Iterate over models and draw
        for (Model model : _models) {
            MaterialInstance pipeline_pass = _materials.get(model.material, _swapchain, _shaders);

            FramebufferSettings framebuffer_settings;
            framebuffer_settings.view = _swapchain.views[image_index];
            framebuffer_settings.extent = _swapchain.extent;
            framebuffer_settings.renderpass = pipeline_pass.renderpass;
            VkFramebuffer framebuffer = _framebuffers.get(framebuffer_settings);

            VkRenderPassBeginInfo renderpass_begin_info = {};
            renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderpass_begin_info.renderPass = pipeline_pass.renderpass;
            renderpass_begin_info.renderArea.extent = _swapchain.extent;
            renderpass_begin_info.renderArea.offset.x = 0;
            renderpass_begin_info.renderArea.offset.y = 0;
            renderpass_begin_info.clearValueCount = 1;
            renderpass_begin_info.pClearValues = &_clear;
            renderpass_begin_info.framebuffer = framebuffer;

            vkCmdBeginRenderPass(frame.command_buffer, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
            vkCmdBindPipeline(frame.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_pass.pipeline);

            MeshAllocation &mesh = _meshes.get(model.mesh);
            vkCmdBindVertexBuffers(frame.command_buffer,
                                   0,
                                   mesh.attribute_offsets.size(),
                                   mesh.buffers.data(),
                                   mesh.attribute_offsets.data());
            if (mesh.index_type != VK_INDEX_TYPE_NONE_KHR) {
                vkCmdBindIndexBuffer(frame.command_buffer, _index_buffer.handle(), mesh.index_offset, mesh.index_type);
                vkCmdDrawIndexed(frame.command_buffer, mesh.index_count, mesh.instance_count, 0, 0, 0);
            } else {
                vkCmdDraw(frame.command_buffer, mesh.vertex_count, mesh.instance_count, 0, 0);
            }
            vkCmdEndRenderPass(frame.command_buffer);
        }
        _models.clear();

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