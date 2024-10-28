#define STB_IMAGE_IMPLEMENTATION
#include <Display.hpp>
#include <Utils/Log.hpp>

namespace Dynamo {
    Display::Display(const std::string &title, unsigned window_width, unsigned window_height) {
        if (!glfwInit()) {
            Log::error("GLFW failed to initialize.");
        }

        _monitor = glfwGetPrimaryMonitor();
        if (!_monitor) {
            Log::error("GLFW could not find the primary monitor.");
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        _window = glfwCreateWindow(window_width, window_height, title.c_str(), nullptr, nullptr);
        if (!_window) {
            Log::error("GLFW failed to create a display window.");
        }

        glfwMakeContextCurrent(_window);
        glfwSwapInterval(1);

        _input = Input(_window);
        _title = title;
        _vsync = true;
    }

    Display::~Display() {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    std::vector<const char *> Display::get_vulkan_extensions() const {
        unsigned count = 0;
        const char **ptr = glfwGetRequiredInstanceExtensions(&count);

        std::vector<const char *> extensions;
        for (unsigned i = 0; i < count; i++) {
            extensions.emplace_back(ptr[i]);
        }
        return extensions;
    }

    VkSurfaceKHR Display::create_vulkan_surface(VkInstance instance) const {
        VkSurfaceKHR surface;
        VkResult result = glfwCreateWindowSurface(instance, _window, nullptr, &surface);
        if (result != VK_SUCCESS) {
            Log::error("Unable to create Vulkan surface from Display.");
        }
        return surface;
    }

    const std::string &Display::get_title() const { return _title; }

    Input &Display::input() { return _input; }

    Vec2 Display::get_window_size() const {
        int width, height;
        glfwGetWindowSize(_window, &width, &height);
        return Vec2(width, height);
    }

    Vec2 Display::get_framebuffer_size() const {
        int width, height;
        glfwGetFramebufferSize(_window, &width, &height);
        return Vec2(width, height);
    }

    bool Display::is_open() const { return !glfwWindowShouldClose(_window); }

    bool Display::is_fullscreen() const { return glfwGetWindowMonitor(_window) != nullptr; }

    bool Display::is_vsync() const { return _vsync; }

    void Display::set_window_size(Vec2 size) { glfwSetWindowSize(_window, size.x, size.y); }

    void Display::set_fullscreen(bool flag) {
        Vec2 size = get_window_size();
        const GLFWvidmode *mode = glfwGetVideoMode(_monitor);
        if (flag) {
            glfwSetWindowMonitor(_window,
                                 _monitor,
                                 mode->width / 2,
                                 mode->height / 2,
                                 mode->width,
                                 mode->height,
                                 mode->refreshRate);
        } else {
            glfwSetWindowMonitor(_window,
                                 nullptr,
                                 (mode->width - size.x) / 2,
                                 (mode->height - size.y) / 2,
                                 size.x,
                                 size.y,
                                 0);
        }
    }

    void Display::set_vsync(bool flag) { _vsync = flag; }

    void Display::set_title(const std::string &title) {
        _title = title;
        glfwSetWindowTitle(_window, _title.c_str());
    }

    void Display::set_icon(const std::string &filepath) {
        int width, height, channels;
        stbi_uc *pixels = stbi_load(filepath.c_str(), &width, &height, &channels, STBI_rgb_alpha);
        if (!pixels) {
            Log::warn("Could not load window icon file `{}`", filepath);
            return;
        }
        GLFWimage image = {
            width,
            height,
            pixels,
        };
        glfwSetWindowIcon(_window, 1, &image);
    }
} // namespace Dynamo