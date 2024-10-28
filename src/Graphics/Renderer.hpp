#pragma once

#include <vulkan/vulkan_core.h>

#include <Display.hpp>
#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/Vulkan/Buffer.hpp>
#include <Graphics/Vulkan/FrameContext.hpp>
#include <Graphics/Vulkan/FramebufferCache.hpp>
#include <Graphics/Vulkan/MaterialRegistry.hpp>
#include <Graphics/Vulkan/MeshRegistry.hpp>
#include <Graphics/Vulkan/PhysicalDevice.hpp>
#include <Graphics/Vulkan/ShaderRegistry.hpp>
#include <Graphics/Vulkan/Swapchain.hpp>
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

        MeshRegistry _meshes;
        ShaderRegistry _shaders;
        MaterialRegistry _materials;
        UniformRegistry _uniforms;
        FramebufferCache _framebuffers;

        FrameContextList _frame_contexts;

        std::vector<Model> _models;
        VkClearValue _clear;

        // TODO:
        // * Let Buffer take in fallback memory types, only throw when all options exhausted
        //    * Actually, maybe refactor the whole thing. Ugly internals
        // * Pre-defined render passes
        // * Depth-stencil buffer
        // * Texture system -> Similar to shaders / meshes, generate a handle and return
        // * Draw-to-texture -> overload render(), render(Texture texture)

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
         * @brief Set the clear color.
         *
         * @param color
         */
        void set_clear(Color color);

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
         * Data must match the size of the uniform variable.
         *
         * @param uniform
         * @param data
         */
        void write_uniform(Uniform uniform, void *data);

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