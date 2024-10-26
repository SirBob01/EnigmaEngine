#pragma once
#define GLFW_INCLUDE_VULKAN

#include <string>

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <Input.hpp>
#include <Math/Vec2.hpp>

namespace Dynamo {
    /**
     * @brief Display window.
     *
     */
    class Display {
        GLFWmonitor *_monitor;
        GLFWwindow *_window;
        Input _input;
        std::string _title;
        bool _vsync;

      public:
        Display(const std::string &title, unsigned window_width, unsigned window_height);

        /**
         * @brief Clean up window resources.
         *
         */
        ~Display();

        /**
         * @brief Get the supported Vulkan extensions.
         *
         * @return std::vector<const char *>
         */
        std::vector<const char *> get_vulkan_extensions() const;

        /**
         * @brief Create a Vulkan surface.
         *
         * @param instance
         * @return VkSurfaceKHR
         */
        VkSurfaceKHR create_vulkan_surface(VkInstance instance) const;

        /**
         * @brief Get the display title.
         *
         * @return const std::string&
         */
        const std::string &get_title() const;

        /**
         * @brief Get the Input subsystem.
         *
         * @return Input&
         */
        Input &input();

        /**
         * @brief Get the window size.
         *
         * @return Vec2
         */
        Vec2 get_window_size() const;

        /**
         * @brief Get the framebuffer size.
         *
         * @return Vec2
         */
        Vec2 get_framebuffer_size() const;

        /**
         * @brief Check if the display is open.
         *
         * @return true
         * @return false
         */
        bool is_open() const;

        /**
         * @brief Check if fullscreen mode is enabled.
         *
         * @return true
         * @return false
         */
        bool is_fullscreen() const;

        /**
         * @brief Check if vsync is enabled.
         *
         * @return true
         * @return false
         */
        bool is_vsync() const;

        /**
         * @brief Set the window size.
         *
         * @param size
         */
        void set_window_size(Vec2 size);

        /**
         * @brief Set fullscreen mode.
         *
         * @param flag
         */
        void set_fullscreen(bool flag);

        /**
         * @brief Set vsync.
         *
         * @param flag
         */
        void set_vsync(bool flag);

        /**
         * @brief Set the title of the window.
         *
         * @param title
         */
        void set_title(const std::string &title);

        /**
         * @brief Set the icon of the window.
         *
         * @param filepath
         */
        void set_icon(const std::string &filepath);
    };
} // namespace Dynamo