#pragma once

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/Texture.hpp>
#include <Graphics/Vulkan/FrameContext.hpp>
#include <Graphics/Vulkan/FramebufferCache.hpp>
#include <Graphics/Vulkan/MaterialRegistry.hpp>
#include <Graphics/Vulkan/MeshRegistry.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/Swapchain.hpp>
#include <Graphics/Vulkan/TextureRegistry.hpp>
#include <Graphics/Vulkan/UniformRegistry.hpp>
#include <Math/Color.hpp>

namespace Dynamo::Graphics {
    using namespace Vulkan;

    /**
     * @brief Vulkan-powered 3D Renderer.
     *
     */
    class Renderer {
        const Display &_display;

        VkInstance _instance;
        VkDebugUtilsMessengerEXT _debugger;
        VkSurfaceKHR _surface;

        PhysicalDevice _physical;
        VkDevice _device;

        Swapchain _swapchain;

        VkCommandPool _graphics_pool;
        VkCommandPool _transfer_pool;

        MemoryPool _memory;
        MeshRegistry _meshes;
        ShaderRegistry _shaders;
        MaterialRegistry _materials;
        UniformRegistry _uniforms;
        TextureRegistry _textures;
        FramebufferCache _framebuffers;

        Texture _color_texture;
        Texture _depth_stencil_texture;

        FrameContextList _frame_contexts;

        std::vector<Model> _models;
        std::array<VkClearValue, 2> _clear;

        // TODO - Fixes:
        // * Decouple a renderpass from a material, make it a first class object
        // * Memory defragmentation stategy

        // TODO - Features:
        // * Cubemaps
        // * Mipmaps from file
        // * Live update texture? --- Support non-shader-optimal image layouts
        // * Draw-to-texture ---- overload render(), render(Texture texture)

        /**
         * @brief Rebuild the swapchain.
         *
         */
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
         * @brief Build a material.
         *
         * @param descriptor
         * @return Material
         */
        Material build_material(const MaterialDescriptor &descriptor);

        /**
         * @brief Free material instance resources.
         *
         * @param material
         */
        void destroy_material(Material material);

        /**
         * @brief Get a reference to a uniform from a material.
         *
         * @param material
         * @param name
         * @return std::optional<Uniform>
         */
        std::optional<Uniform> get_uniform(Material material, const std::string &name);

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