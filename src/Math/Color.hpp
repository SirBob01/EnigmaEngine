#pragma once

#include <array>
#include <cstdint>
#include <functional>

#include <Math/Common.hpp>

namespace Dynamo {
    /**
     * @brief RGBA color value.
     *
     * Each channel accepts a floating point number from [0 - 1]. Values outside
     * this range are invalid.
     *
     */
    struct Color {
        float r;
        float g;
        float b;
        float a;

        /**
         * @brief Construct a new Color object.
         *
         * @param r Red channel.
         * @param g Green channel.
         * @param b Blue channel.
         * @param a Alpha channel.
         */
        constexpr Color(float r = 0, float g = 0, float b = 0, float a = 1) : r(r), g(g), b(b), a(a) {}

        /**
         * @brief Linearly interpolate between two colors.
         *
         * @param rhs Target color.
         * @param t   Interpolation factor.
         * @return Color
         */
        Color lerp(const Color &rhs, float t) const {
            return Color(Dynamo::lerp(r, rhs.r, t),
                         Dynamo::lerp(g, rhs.g, t),
                         Dynamo::lerp(b, rhs.b, t),
                         Dynamo::lerp(a, rhs.a, t));
        }

        /**
         * @brief Invert the RGB channels, maintaining the alpha channel.
         *
         * @return Color
         */
        Color invert() { return Color(1 - r, 1 - g, 1 - b, a); }

        /**
         * @brief Convert the color to a single 32-bit unsigned integer which
         * can be represented as a hex value.
         *
         * @return constexpr unsigned
         */
        constexpr unsigned to_hex() const {
            unsigned r_v = UINT8_MAX * r;
            unsigned g_v = UINT8_MAX * g;
            unsigned b_v = UINT8_MAX * b;
            unsigned a_v = UINT8_MAX * a;
            return (r_v << 24) | (g_v << 16) | (b_v << 8) | (a_v);
        }

        /**
         * @brief Convert the Color into an array of floats.
         *
         * @return std::array<float, 4>
         */
        std::array<float, 4> to_array() const { return {r, g, b, a}; }

        /**
         * @brief Equality operator.
         *
         * @param rhs
         * @return true
         * @return false
         */
        inline bool operator==(const Color &rhs) const { return r == rhs.r && g == rhs.g && b == rhs.b && a == rhs.a; }

        /**
         * @brief Inequality operator.
         *
         * @param rhs
         * @return true
         * @return false
         */
        inline bool operator!=(const Color &rhs) const { return !(*this == rhs); }
    };
} // namespace Dynamo

/**
 * @brief Hash function implementation for Color.
 *
 * @tparam
 */
template <>
struct std::hash<Dynamo::Color> {
    inline size_t operator()(const Dynamo::Color &color) const {
        long long tr = color.r * 73856093;
        long long tg = color.g * 19349663;
        long long tb = color.b * 83492791;
        long long ta = color.a * 52477425;
        return tr ^ tg ^ tb ^ ta;
    }
};
