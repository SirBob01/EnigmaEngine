#pragma once

#include <algorithm>

#include <Math/Quaternion.hpp>
#include <Math/Vec3.hpp>
#include <Utils/Log.hpp>

namespace Dynamo {
    /**
     * @brief A colum-major 4x4 square matrix.
     *
     */
    struct Mat4 {
        static constexpr unsigned N = 4;
        static constexpr unsigned N2 = N * N;
        float values[N2];

        /**
         * @brief Construct an identity Mat4.
         *
         */
        constexpr Mat4() : values() {
            for (unsigned i = 0; i < N; i++) {
                for (unsigned j = 0; j < N; j++) {
                    values[i * N + j] = (i == j);
                }
            }
        }

        /**
         * @brief Initialze a Mat4 with a value.
         *
         */
        constexpr Mat4(float v) : values() {
            for (unsigned i = 0; i < N2; i++) {
                values[i] = v;
            }
        }

        /**
         * @brief Initialize a basis Mat4.
         *
         */
        constexpr Mat4(const Vec3 &x, const Vec3 &y, const Vec3 &z) : values() {
            values[0] = x.x;
            values[1] = x.y;
            values[2] = x.z;
            values[3] = 0;

            values[4] = y.x;
            values[5] = y.y;
            values[6] = y.z;
            values[7] = 0;

            values[8] = z.x;
            values[9] = z.y;
            values[10] = z.z;
            values[11] = 0;

            values[12] = 0;
            values[13] = 0;
            values[14] = 0;
            values[15] = 1;
        }

        /**
         * @brief Compose a Mat4 from a position, rotation, and scale.
         *
         */
        constexpr Mat4(const Vec3 &position, const Quaternion &rotation, const Vec3 &scale) : values() {
            float x = rotation.x;
            float y = rotation.y;
            float z = rotation.z;
            float w = rotation.w;

            float x2 = x + x;
            float y2 = y + y;
            float z2 = z + z;
            float xx = x * x2;
            float xy = x * y2;
            float xz = x * z2;
            float yy = y * y2;
            float yz = y * z2;
            float zz = z * z2;
            float wx = w * x2;
            float wy = w * y2;
            float wz = w * z2;

            float sx = scale.x;
            float sy = scale.y;
            float sz = scale.z;

            values[0] = (1 - (yy + zz)) * sx;
            values[1] = (xy + wz) * sx;
            values[2] = (xz - wy) * sx;
            values[3] = 0;

            values[4] = (xy - wz) * sy;
            values[5] = (1 - (xx + zz)) * sy;
            values[6] = (yz + wx) * sy;
            values[7] = 0;

            values[8] = (xz + wy) * sz;
            values[9] = (yz - wx) * sz;
            values[10] = (1 - (xx + yy)) * sz;
            values[11] = 0;

            values[12] = position.x;
            values[13] = position.y;
            values[14] = position.z;
            values[15] = 1;
        }

        /**
         * @brief Negation.
         *
         * @return Mat4
         */
        inline Mat4 operator-() const {
            Mat4 result;
            for (unsigned i = 0; i < N2; i++) {
                result.values[i] = -values[i];
            }
            return result;
        }

        /**
         * @brief Add another matrix.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 operator+(const Mat4 &rhs) const {
            Mat4 result;
            for (unsigned i = 0; i < N2; i++) {
                result.values[i] = values[i] + rhs.values[i];
            }
            return result;
        }

        /**
         * @brief Subtract another matrix.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 operator-(const Mat4 &rhs) const {
            Mat4 result;
            for (unsigned i = 0; i < N2; i++) {
                result.values[i] = values[i] - rhs.values[i];
            }
            return result;
        }

        /**
         * @brief Multiply with another matrix.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 operator*(const Mat4 &rhs) const {
            Mat4 result;
            for (unsigned i = 0; i < N; i++) {
                for (unsigned j = 0; j < N; j++) {
                    float dot = 0;
                    for (unsigned c = 0; c < N; c++) {
                        dot += values[c * N + j] * rhs.values[i * N + c];
                    }
                    result.values[i * N + j] = dot;
                }
            }
            return result;
        }

        /**
         * @brief Multiply by a scalar.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 operator*(float rhs) const {
            Mat4 result;
            for (unsigned i = 0; i < N2; i++) {
                result.values[i] = values[i] * rhs;
            }
            return result;
        }

        /**
         * @brief Apply to a Vec3.
         *
         * @param rhs
         * @return Vec3
         */
        Vec3 operator*(const Vec3 &rhs) const {
            Vec3 result;
            float w = 1.0 / (values[3] * rhs.x + values[7] * rhs.y + values[11] * rhs.z + values[15]);
            result.x = (values[0] * rhs.x + values[4] * rhs.y + values[8] * rhs.z + values[12]) * w;
            result.y = (values[1] * rhs.x + values[5] * rhs.y + values[9] * rhs.z + values[13]) * w;
            result.z = (values[2] * rhs.x + values[6] * rhs.y + values[10] * rhs.z + values[14]) * w;
            return result;
        }

        /**
         * @brief Divide by a scalar.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 operator/(float rhs) const {
            float inv = 1.0 / rhs;
            return (*this) * inv;
        }

        /**
         * @brief Add another matrix in-place.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 &operator+=(const Mat4 &rhs) {
            for (unsigned i = 0; i < N2; i++) {
                values[i] += rhs.values[i];
            }
            return *this;
        }

        /**
         * @brief Subtract another matrix in-place.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 &operator-=(const Mat4 &rhs) {
            for (unsigned i = 0; i < N2; i++) {
                values[i] -= rhs.values[i];
            }
            return *this;
        }

        /**
         * @brief Multiply with another matrix in-place.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 &operator*=(const Mat4 &rhs) {
            Mat4 result;
            for (unsigned i = 0; i < N; i++) {
                for (unsigned j = 0; j < N; j++) {
                    float dot = 0;
                    for (unsigned c = 0; c < N; c++) {
                        dot += values[c * N + j] * rhs.values[i * N + c];
                    }
                    result.values[i * N + j] = dot;
                }
            }
            std::copy(result.values, result.values + N2, values);
            return *this;
        }

        /**
         * @brief Multiply by a scalar in-place.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 operator*=(float rhs) {
            for (unsigned i = 0; i < N2; i++) {
                values[i] *= rhs;
            }
            return *this;
        }

        /**
         * @brief Divide by a scalar in-place.
         *
         * @param rhs
         * @return Mat4
         */
        inline Mat4 operator/=(float rhs) {
            float inv = 1.0 / rhs;
            return (*this) *= inv;
        }

        /**
         * @brief Swap rows and columns.
         *
         * @return Mat4&
         */
        inline Mat4 &transpose() {
            for (unsigned i = 0; i < N; i++) {
                for (unsigned j = 0; j < i; j++) {
                    std::swap(values[i + j * N], values[j + i * N]);
                }
            }
            return *this;
        }

        /**
         * @brief Equality operator.
         *
         * @param rhs
         * @return true
         * @return false
         */
        inline bool operator==(const Mat4 &rhs) const {
            for (unsigned i = 0; i < N2; i++) {
                if (values[i] != rhs.values[i]) {
                    return false;
                }
            }
            return true;
        }

        /**
         * @brief Inequality operator.
         *
         * @param rhs
         * @return true
         * @return false
         */
        inline bool operator!=(const Mat4 &rhs) const { return !(*this == rhs); }
    };
} // namespace Dynamo