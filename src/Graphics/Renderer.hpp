#pragma once

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/Context.hpp>
#include <Graphics/Vulkan/DescriptorPool.hpp>
#include <Graphics/Vulkan/FrameContext.hpp>
#include <Graphics/Vulkan/MeshRegistry.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Graphics/Vulkan/PipelineRegistry.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/Swapchain.hpp>
#include <Graphics/Vulkan/TextureRegistry.hpp>
#include <Graphics/Vulkan/UniformRegistry.hpp>
#include <Math/Color.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief Vulkan-powered 3D Renderer.
     *
     */
    class Renderer {
        const Display &_display;
        Vulkan::Context _context;
        Vulkan::Swapchain _swapchain;

        Vulkan::MemoryPool _memory;
        Vulkan::DescriptorPool _descriptors;
        Vulkan::BufferRegistry _buffers;
        Vulkan::TextureRegistry _textures;
        Vulkan::MeshRegistry _meshes;
        Vulkan::ShaderRegistry _shaders;
        Vulkan::PipelineRegistry _pipelines;
        Vulkan::UniformRegistry _uniforms;
        Vulkan::FrameContextList _frame_contexts;

        Texture _color_texture;
        Texture _depth_stencil_texture;

        VkRenderPass _forwardpass;
        std::vector<VkFramebuffer> _framebuffers;
        std::array<VkClearValue, 2> _clear;

        std::vector<Model> _models;

        // Important TODO:
        // * Better Uniform system that supports other descriptor types...
        //      - How do we double buffer objects for frames in flight??
        //      - When binding a uniform buffer to descriptor, create separate buffers as the 'working' buffer?
        //          - Not very good, could lead to heavy fragmentation
        // * Proper render pass system to implement algorithms like Forward+
        //      - Allow creating render passes at the user level
        //      - Users can specify number of color attachments and types
        //      - Users should have control over the render target area
        //      - Users should have control over which textures to use as attachments
        //      - Users should have control over attachment clear values
        //      - Revamp render() workflow, need to arbitrarily begin and end renderpasses
        // * Draw-to-texture?
        // * Customizable color blending (do we really need this?)
        // * Customizable stencil operations
        // * Memory defragmentation stategy
        // * Effects Algorithms
        //    * Ambient occlussion (GTAO?)
        //    * Shadow maps
        //    * Volumetric particles

        void rebuild_framebuffers();

        void rebuild_swapchain();

      public:
        /**
         * @brief Initialize the renderer.
         *
         * @param display
         * @param root_asset_directory
         */
        Renderer(const Display &display, const std::string &root_asset_directory);

        /**
         * @brief Cleanup and destroy the renderer.
         *
         */
        ~Renderer();

        /**
         * @brief Set the clear values.
         *
         * @param color
         * @param depth
         * @param stencil
         */
        void set_clear(Color color, float depth = 1, unsigned stencil = 0);

        /**
         * @brief Build a mesh and upload to VRAM.
         *
         * @param descriptor
         * @return Mesh
         */
        Mesh build_mesh(const MeshDescriptor &descriptor);

        /**
         * @brief Free mesh resources.
         *
         * @param mesh
         */
        void destroy_mesh(Mesh mesh);

        /**
         * @brief Build a shader module.
         *
         * @param descriptor
         * @return Shader
         */
        Shader build_shader(const ShaderDescriptor &descriptor);

        /**
         * @brief Free shader resources.
         *
         * @param shader
         */
        void destroy_shader(Shader shader);

        /**
         * @brief Build a buffer.
         *
         * @param descriptor
         * @return Buffer
         */
        Buffer build_buffer(const BufferDescriptor &descriptor);

        /**
         * @brief Destroy a buffer.
         *
         * @param buffer
         */
        void destroy_buffer(Buffer buffer);

        /**
         * @brief Write data to a (host-visible) buffer.
         *
         * @param src
         * @param dst
         * @param dst_offset
         * @param length
         */
        void write_buffer(const void *src, Buffer dst, unsigned dst_offset, unsigned length);

        /**
         * @brief Copy data between buffers.
         *
         * @param src
         * @param dst
         * @param src_offset
         * @param dst_offset
         * @param length
         */
        void copy_buffer(Buffer src, Buffer dst, unsigned src_offset, unsigned dst_offset, unsigned length);

        /**
         * @brief Build a texture.
         *
         * @param descriptor
         * @return Texture
         */
        Texture build_texture(const TextureDescriptor &descriptor);

        /**
         * @brief Free texture resources.
         *
         * @param texture
         */
        void destroy_texture(Texture texture);

        /**
         * @brief Build a graphics pipeline.
         *
         * @param descriptor
         * @return Pipeline
         */
        Pipeline build_pipeline(const PipelineDescriptor &descriptor);

        /**
         * @brief Destroy a graphics pipeline.
         *
         * @param pipeline
         */
        void destroy_pipeline(Pipeline pipeline);

        /**
         * @brief Build a uniform group from a pipeline.
         *
         * @param pipeline
         * @return UniformGroup
         */
        UniformGroup build_uniforms(Pipeline pipeline);

        /**
         * @brief Destroy a uniform group.
         *
         */
        void destroy_uniforms(UniformGroup group);

        /**
         * @brief Get a reference to a uniform from a uniform group.
         *
         * @param group
         * @param name
         * @return std::optional<Uniform>
         */
        std::optional<Uniform> get_uniform(UniformGroup group, const std::string &name);

        /**
         * @brief Write to a uniform.
         *
         * If the uniform is an array, an index offset and count can be provided.
         *
         * @param uniform
         * @param data
         * @param index
         * @param count
         */
        void write_uniform(Uniform uniform, void *data, unsigned index = 0, unsigned count = 1);

        /**
         * @brief Bind a texture to a uniform variable.
         *
         * If the uniform is an array, an index offset can be provided.
         *
         * @param uniform
         * @param texture
         * @param index
         */
        void bind_texture(Uniform uniform, Texture texture, unsigned index = 0);

        /**
         * @brief Draw a model in the current frame.
         *
         * @param model
         */
        void draw(const Model &model);

        /**
         * @brief Render to the display.
         *
         */
        void render();
    };
} // namespace Dynamo::Graphics