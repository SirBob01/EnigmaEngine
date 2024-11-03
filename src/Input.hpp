#pragma once

#include <array>

#include <GLFW/glfw3.h>

#include <Math/Vec2.hpp>

namespace Dynamo {
    /**
     * @brief Enumerates all possible keycodes
     *
     * Taken from `GLFW/glfw3.h`
     *
     */
    enum class KeyCode {
        Space = 32,
        Apostrophe = 39, /* ' */
        Comma = 44,      /* , */
        Minus = 45,      /* - */
        Period = 46,     /* . */
        Slash = 47,      /* / */
        K0 = 48,
        K1 = 49,
        K2 = 50,
        K3 = 51,
        K4 = 52,
        K5 = 53,
        K6 = 54,
        K7 = 55,
        K8 = 56,
        K9 = 57,
        Semicolon = 59, /* ; */
        Equal = 61,     /* = */
        A = 65,
        B = 66,
        C = 67,
        D = 68,
        E = 69,
        F = 70,
        G = 71,
        H = 72,
        I = 73,
        J = 74,
        K = 75,
        L = 76,
        M = 77,
        N = 78,
        O = 79,
        P = 80,
        Q = 81,
        R = 82,
        S = 83,
        T = 84,
        U = 85,
        V = 86,
        W = 87,
        X = 88,
        Y = 89,
        Z = 90,
        LeftBracket = 91,  /* [ */
        BackSlash = 92,    /* \ */
        RightBracket = 93, /* ] */
        GraveAccent = 96,  /* ` */
        Escape = 256,
        Enter = 257,
        Tab = 258,
        Backspace = 259,
        Insert = 260,
        Delete = 261,
        Right = 262,
        Left = 263,
        Down = 264,
        Up = 265,
        PageUp = 266,
        PageDown = 267,
        Home = 268,
        End = 269,
        CapsLock = 280,
        ScrollLock = 281,
        NumLock = 282,
        PrintScreen = 283,
        Pause = 284,
        F1 = 290,
        F2 = 291,
        F3 = 292,
        F4 = 293,
        F5 = 294,
        F6 = 295,
        F7 = 296,
        F8 = 297,
        F9 = 298,
        F10 = 299,
        F11 = 300,
        F12 = 301,
        F13 = 302,
        F14 = 303,
        F15 = 304,
        F16 = 305,
        F17 = 306,
        F18 = 307,
        F19 = 308,
        F20 = 309,
        F21 = 310,
        F22 = 311,
        F23 = 312,
        F24 = 313,
        F25 = 314,
        KP0 = 320,
        KP1 = 321,
        KP2 = 322,
        KP3 = 323,
        KP4 = 324,
        KP5 = 325,
        KP6 = 326,
        KP7 = 327,
        KP8 = 328,
        KP9 = 329,
        KPDecimal = 330,
        KPDivide = 331,
        KPMultiply = 332,
        KPSubtract = 333,
        KPAdd = 334,
        KPEnter = 335,
        KPEqual = 336,
        LeftShift = 340,
        LeftControl = 341,
        LeftAlt = 342,
        LeftSuper = 343,
        RightShift = 344,
        RightControl = 345,
        RightAlt = 346,
        RightSuper = 347,
        Menu = 348,
    };

    /**
     * @brief Enumerates all possible mouse button codes
     *
     */
    enum class MouseCode {
        Left = 0,
        Right = 1,
        Middle = 2,
    };

    /**
     * @brief Total number of keys.
     *
     */
    constexpr unsigned KEYCODE_COUNT = static_cast<unsigned>(KeyCode::Menu) + 1;

    /**
     * @brief Total number of mouse buttons.
     *
     */
    constexpr unsigned MOUSECODE_COUNT = static_cast<unsigned>(MouseCode::Middle) + 1;

    /**
     * @brief Input polling handler
     *
     */
    class Input {
        GLFWwindow *_window;

        struct State {
            Vec2 mouse_position;
            Vec2 scroll_offset;

            std::array<bool, KEYCODE_COUNT> key_pressed;
            std::array<bool, KEYCODE_COUNT> key_released;
            std::array<bool, KEYCODE_COUNT> key_down;

            std::array<bool, MOUSECODE_COUNT> mouse_pressed;
            std::array<bool, MOUSECODE_COUNT> mouse_released;
            std::array<bool, MOUSECODE_COUNT> mouse_down;
        };
        inline static State _state = {};

      public:
        Input(GLFWwindow *window);
        Input() = default;

        /**
         * @brief Get the current mouse position
         *
         * @return const Vec2&
         */
        const Vec2 &get_mouse_position() const;

        /**
         * @brief Get the scroll offset along the vertical and
         * horizontal axes
         *
         * @return const Vec2&
         */
        const Vec2 &get_scroll_offset() const;

        /**
         * @brief Is the key pressed?
         *
         * This checks for the instantaneous button press, which is immediately
         * false by the next frame
         *
         * @param code Key code
         * @return true
         * @return false
         */
        bool is_pressed(KeyCode code);

        /**
         * @brief Is the mouse button pressed?
         *
         * This checks for the instantaneous button press, which is immediately
         * false by the next frame
         *
         * @param code Mouse button code
         * @return true
         * @return false
         */
        bool is_pressed(MouseCode code);

        /**
         * @brief Is the key released?
         *
         * This checks for the instantaneous button release, which is
         * immediately false by the next frame
         *
         * @param code Key code
         * @return true
         * @return false
         */
        bool is_released(KeyCode code);

        /**
         * @brief Is the mouse button released?
         *
         * This checks for the instantaneous button release, which is
         * immediately false by the next frame
         *
         * @param code Mouse button code
         * @return true
         * @return false
         */
        bool is_released(MouseCode code);

        /**
         * @brief Is the key held down?
         *
         * @param code Key code
         * @return true
         * @return false
         */
        bool is_down(KeyCode code);

        /**
         * @brief Is the mouse button held down?
         *
         * @param code Mouse button code
         * @return true
         * @return false
         */
        bool is_down(MouseCode code);

        /**
         * @brief Poll input and update internal state
         *
         */
        void poll();
    };
} // namespace Dynamo