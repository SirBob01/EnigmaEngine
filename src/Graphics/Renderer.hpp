#pragma once

#include "Graphics/Vulkan/RenderPassRegistry.hpp"
#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/DescriptorPool.hpp>
#include <Graphics/Vulkan/FrameContext.hpp>
#include <Graphics/Vulkan/MeshRegistry.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Graphics/Vulkan/PipelineRegistry.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/Swapchain.hpp>
#include <Graphics/Vulkan/TextureRegistry.hpp>
#include <Graphics/Vulkan/UniformRegistry.hpp>
#include <Graphics/Vulkan/VulkanContext.hpp>
#include <Math/Color.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief Vulkan-powered 3D Renderer.
     *
     */
    class Renderer {
        const Display &_display;
        Vulkan::VulkanContext _context;
        Vulkan::Swapchain _swapchain;

        Vulkan::MemoryPool _memory;
        Vulkan::DescriptorPool _descriptors;
        Vulkan::MeshRegistry _meshes;
        Vulkan::ShaderRegistry _shaders;
        Vulkan::RenderPassRegistry _renderpasses;
        Vulkan::PipelineRegistry _pipelines;
        Vulkan::UniformRegistry _uniforms;
        Vulkan::TextureRegistry _textures;
        Vulkan::FrameContextList _frame_contexts;

        Texture _color_texture;
        Texture _depth_stencil_texture;

        VkFramebuffer _depth_framebuffer;
        std::vector<VkFramebuffer> _shading_framebuffers;
        std::array<VkClearValue, 2> _clear;

        std::vector<Model> _models;

        // Important TODO:
        // * Implement RenderPassRegistry that holds the main renderpasses
        //      - Forward (Scene)
        //      - Target (draw-to-texture)?
        //      - Overlay
        // * BufferRegistry? We will need to update all registries that work with buffers...
        // * Allow grouping `shared_uniforms` (i.e., which UniformGroups should share which variables?)
        //      - Holup, are these even useful now that we have UniformGroups???
        // * Customizable color blending (do we really need this?)
        // * Customizable stencil operations
        // * Memory defragmentation stategy? We need to recreate buffers/textures and rebind them....
        //      - Should we attach defrag listeners to the owners of allocated resources?
        //      - Affected modules: MeshRegistry, TextureRegistry, UniformRegistry

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