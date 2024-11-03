#include <Input.hpp>

namespace Dynamo {
    Input::Input(GLFWwindow *window) : _window(window) {
        // Register input handling callbacks to update internal state
        glfwSetWindowUserPointer(_window, &_state);

        // On cursor motion
        auto cursor_cb = [](GLFWwindow *window, double x, double y) {
            void *userptr = glfwGetWindowUserPointer(window);
            State *state = static_cast<State *>(userptr);
            state->mouse_position.x = x;
            state->mouse_position.y = y;
        };
        glfwSetCursorPosCallback(_window, cursor_cb);

        // On scroll
        auto scroll_cb = [](GLFWwindow *window, double x, double y) {
            void *userptr = glfwGetWindowUserPointer(window);
            State *state = static_cast<State *>(userptr);
            state->scroll_offset.x = x;
            state->scroll_offset.y = y;
        };
        glfwSetScrollCallback(_window, scroll_cb);

        // On key state change
        auto key_cb = [](GLFWwindow *window, int key, int scancode, int action, int mods) {
            void *userptr = glfwGetWindowUserPointer(window);
            State *state = static_cast<State *>(userptr);
            switch (action) {
            case GLFW_PRESS:
                state->key_pressed[key] = true;
                state->key_down[key] = true;
                break;
            case GLFW_RELEASE:
                state->key_released[key] = true;
                state->key_down[key] = false;
                break;
            default:
                break;
            }
        };
        glfwSetKeyCallback(_window, key_cb);

        // On mouse button state change
        auto click_cb = [](GLFWwindow *window, int button, int action, int mods) {
            void *userptr = glfwGetWindowUserPointer(window);
            State *state = static_cast<State *>(userptr);
            switch (action) {
            case GLFW_PRESS:
                state->mouse_pressed[button] = true;
                state->mouse_down[button] = true;
                break;
            case GLFW_RELEASE:
                state->mouse_released[button] = true;
                state->mouse_down[button] = false;
                break;
            default:
                break;
            }
        };
        glfwSetMouseButtonCallback(_window, click_cb);
    }

    const Vec2 &Input::get_mouse_position() const { return _state.mouse_position; }

    const Vec2 &Input::get_scroll_offset() const { return _state.scroll_offset; }

    bool Input::is_pressed(KeyCode code) { return _state.key_pressed[static_cast<unsigned>(code)]; }

    bool Input::is_pressed(MouseCode code) { return _state.mouse_pressed[static_cast<unsigned>(code)]; }

    bool Input::is_released(KeyCode code) { return _state.key_released[static_cast<unsigned>(code)]; }

    bool Input::is_released(MouseCode code) { return _state.mouse_released[static_cast<unsigned>(code)]; }

    bool Input::is_down(KeyCode code) { return _state.key_down[static_cast<unsigned>(code)]; }

    bool Input::is_down(MouseCode code) { return _state.mouse_down[static_cast<unsigned>(code)]; }

    void Input::poll() {
        // Reset scroll state
        _state.scroll_offset.x = 0;
        _state.scroll_offset.y = 0;

        // Reset key states
        std::fill(_state.key_pressed.begin(), _state.key_pressed.end(), false);
        std::fill(_state.key_released.begin(), _state.key_released.end(), false);

        // Reset mouse button states
        std::fill(_state.mouse_pressed.begin(), _state.mouse_pressed.end(), false);
        std::fill(_state.mouse_released.begin(), _state.mouse_released.end(), false);

        glfwPollEvents();
    }
} // namespace Dynamo