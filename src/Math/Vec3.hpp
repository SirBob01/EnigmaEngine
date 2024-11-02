#pragma once

#include <cmath>
#include <functional>

namespace Dynamo {
    /**
     * @brief 3D vector.
     *
     */
    struct Vec3 {
        float x, y, z;

        /**
         * @brief Construct a new Vec3 object.
         *
         * @param x
         * @param y
         * @param z
         */
        constexpr Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}

        /**
         * @brief Calculate the squared length.
         *
         * @return float
         */
        inline float length_squared() const { return x * x + y * y + z * z; }

        /**
         * @brief Calculate the length.
         *
         * @return float
         */
        inline float length() const { return std::sqrt(length_squared()); }

        /**
         * @brief Add another vector.
         *
         * @param rhs
         * @return Vec3
         */
        inline Vec3 operator+(const Vec3 &rhs) const { return Vec3(x + rhs.x, y + rhs.y, z + rhs.z); }

        /**
         * @brief Subtract another vector.
         *
         * @param rhs
         * @return Vec3
         */
        inline Vec3 operator-(const Vec3 &rhs) const { return Vec3(x - rhs.x, y - rhs.y, z - rhs.z); }

        /**
         * @brief Multiply by a scalar.
         *
         * @param scalar
         * @return Vec3
         */
        inline Vec3 operator*(float scalar) const { return Vec3(x * scalar, y * scalar, z * scalar); }

        /**
         * @brief Divide by a scalar.
         *
         * @param scalar
         * @return Vec3
         */
        inline Vec3 operator/(float scalar) const {
            float inv = 1.0f / scalar;
            return *this * inv;
        }

        /**
         * @brief Negation.
         *
         * @return Vec3
         */
        inline Vec3 operator-() const { return Vec3(-x, -y, -z); }

        /**
         * @brief Add another vector in-place.
         *
         * @param rhs
         * @return Vec3&
         */
        inline Vec3 &operator+=(const Vec3 &rhs) {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }

        /**
         * @brief Subtract another vector in-place.
         *
         * @param rhs
         * @return Vec3&
         */
        inline Vec3 &operator-=(const Vec3 &rhs) {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }

        /**
         * @brief Multiply by a scalar in-place.
         *
         * @param scalar
         * @return Vec3&
         */
        inline Vec3 &operator*=(float scalar) {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            return *this;
        }

        /**
         * @brief Divide by a scalar in-place.
         *
         * @param scalar
         * @return Vec3&
         */
        inline Vec3 &operator/=(float scalar) {
            float inv = 1.0f / scalar;
            return *this *= inv;
        }

        /**
         * @brief Normalize this vector in-place.
         *
         * @return Vec3&
         */
        inline Vec3 &normalize() { return *this /= length(); }

        /**
         * @brief Dot product.
         *
         * @param rhs
         * @return float
         */
        inline float operator*(const Vec3 &rhs) const { return x * rhs.x + y * rhs.y + z * rhs.z; }

        /**
         * @brief 3D cross product.
         *
         * @param rhs
         * @return Vec3
         */
        inline Vec3 cross(const Vec3 &rhs) const {
            float n_x = y * rhs.z - z * rhs.y;
            float n_y = z * rhs.x - x * rhs.z;
            float n_z = x * rhs.y - y * rhs.x;
            return Vec3(n_x, n_y, n_z);
        }

        /**
         * @brief Equality operator.
         *
         * @param rhs
         * @return true
         * @return false
         */
        inline bool operator==(const Vec3 &rhs) const { return x == rhs.x && y == rhs.y && z == rhs.z; }

        /**
         * @brief Inequality operator.
         *
         * @param rhs
         * @return true
         * @return false
         */
        inline bool operator!=(const Vec3 &rhs) const { return !(*this == rhs); }
    };
} // namespace Dynamo

/**
 * @brief Hash function implementation for Vec3.
 *
 * @tparam
 */
template <>
struct std::hash<Dynamo::Vec3> {
    inline size_t operator()(const Dynamo::Vec3 &point) const {
        long long tx = point.x * 73856093;
        long long ty = point.y * 19349663;
        long long tz = point.z * 83492791;
        return tx ^ ty ^ tz;
    }
};