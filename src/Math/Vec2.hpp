#pragma once

#include <cmath>
#include <functional>

namespace Dynamo {
    /**
     * @brief 2D vector.
     *
     */
    struct Vec2 {
        float x, y;

        /**
         * @brief Construct a new Vec2 object.
         *
         * @param x
         * @param y
         */
        constexpr Vec2(float x = 0, float y = 0) : x(x), y(y) {}

        /**
         * @brief Calculate the squared length.
         *
         * @return float
         */
        inline float length_squared() const { return x * x + y * y; }

        /**
         * @brief Calculate the length.
         *
         * @return float
         */
        inline float length() const { return std::sqrt(length_squared()); }

        /**
         * @brief Get the left normal perpendicular to this vector.
         *
         * @return Vec2
         */
        inline Vec2 left_normal() const { return Vec2(-y, x); }

        /**
         * @brief Get the right normal perpendicular to this vector.
         *
         * @return Vec2
         */
        inline Vec2 right_normal() const { return Vec2(y, -x); }

        /**
         * @brief Add another vector.
         *
         * @param rhs
         * @return Vec2
         */
        inline Vec2 operator+(const Vec2 &rhs) const { return Vec2(x + rhs.x, y + rhs.y); }

        /**
         * @brief Subtract another vector.
         *
         * @param rhs
         * @return Vec2
         */
        inline Vec2 operator-(const Vec2 &rhs) const { return Vec2(x - rhs.x, y - rhs.y); }

        /**
         * @brief Multiply by a scalar.
         *
         * @param scalar
         * @return Vec2
         */
        inline Vec2 operator*(float scalar) const { return Vec2(x * scalar, y * scalar); }

        /**
         * @brief Divide by a scalar.
         *
         * @param scalar
         * @return Vec2
         */
        inline Vec2 operator/(float scalar) const {
            const float inv = 1.0f / scalar;
            return *this * inv;
        }

        /**
         * @brief Negation.
         *
         * @return Vec2
         */
        inline Vec2 operator-() const { return Vec2(-x, -y); }

        /**
         * @brief Add another vector in-place.
         *
         * @param rhs
         * @return Vec2&
         */
        inline Vec2 &operator+=(const Vec2 &rhs) {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }

        /**
         * @brief Subtract another vector in-place.
         *
         * @param rhs
         * @return Vec2&
         */
        inline Vec2 &operator-=(const Vec2 &rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }

        /**
         * @brief Multiply by a scalar in-place.
         *
         * @param scalar
         * @return Vec2&
         */
        inline Vec2 &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            return *this;
        }

        /**
         * @brief Divide by a scalar in-place.
         *
         * @param scalar
         * @return Vec2&
         */
        inline Vec2 &operator/=(float scalar) {
            float inv = 1.0 / scalar;
            return *this *= inv;
        }

        /**
         * @brief Normalize this vector in-place.
         *
         * @return Vec2&
         */
        inline Vec2 &normalize() { return *this /= length(); }

        /**
         * @brief Dot product.
         *
         * @param rhs
         * @return float
         */
        inline float operator*(const Vec2 &rhs) const { return x * rhs.x + y * rhs.y; }

        /**
         * @brief 2D cross product.
         *
         * @param rhs
         * @return float
         */
        inline float cross(const Vec2 &rhs) const { return x * rhs.y - y * rhs.x; }

        /**
         * @brief Equality operator.
         *
         * @param rhs
         * @return true
         * @return false
         */
        inline bool operator==(const Vec2 &rhs) const { return x == rhs.x && y == rhs.y; }

        /**
         * @brief Inequality operator.
         *
         * @param rhs
         * @return true
         * @return false
         */
        inline bool operator!=(const Vec2 &rhs) const { return !(*this == rhs); }
    };
} // namespace Dynamo

/**
 * @brief Hash function implementation for Vec2.
 *
 * @tparam
 */
template <>
struct std::hash<Dynamo::Vec2> {
    inline size_t operator()(const Dynamo::Vec2 &point) const {
        long long tx = point.x * 73856093;
        long long ty = point.y * 19349663;
        return tx ^ ty;
    }
};
