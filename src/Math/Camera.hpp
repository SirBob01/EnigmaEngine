#pragma once

#include <Math/Box2.hpp>
#include <Math/Mat4.hpp>

namespace Dynamo {
    /**
     * @brief Perspective camera parameters.
     *
     */
    struct Perspective {
        /**
         * @brief Vertical field-of-view in radians.
         *
         */
        float fovy;

        /**
         * @brief Aspect ratio of the viewport.
         *
         */
        float aspect;

        /**
         * @brief Near plane on the z-axis
         *
         */
        float znear;

        /**
         * @brief Far plane on the z-axis.
         *
         */
        float zfar;
    };

    /**
     * @brief Orthographic camera parameters.
     *
     */
    struct Orthographic {
        /**
         * @brief Extents of the viewport.
         *
         */
        Box2 viewport;

        /**
         * @brief Near plane on the z-axis
         *
         */
        float znear;

        /**
         * @brief Far plane on the z-axis.
         *
         */
        float zfar;
    };

    /**
     * @brief 3D Camera.
     *
     */
    struct Camera {
        Mat4 projection;
        Mat4 view;

        /**
         * @brief Default constructor.
         *
         */
        constexpr Camera() = default;

        /**
         * @brief Initialize a perspective projection camera.
         *
         * @param params
         */
        constexpr Camera(const Perspective &params) { make(params); }

        /**
         * @brief Initialize an orthographic projection camera.
         *
         * @param params
         */
        constexpr Camera(const Orthographic &params) { make(params); }

        /**
         * @brief Set a perspective projection matrix.
         *
         * @param params
         * @return Camera&
         */
        inline Camera &make(const Perspective &params) {
            DYN_ASSERT(params.aspect != 0);
            DYN_ASSERT(params.zfar != params.znear);

            float f = 1.0 / std::tan(0.5 * params.fovy);
            float dz = params.znear - params.zfar;

            projection.values[0] = f / params.aspect;
            projection.values[1] = 0;
            projection.values[2] = 0;
            projection.values[3] = 0;

            projection.values[4] = 0;
            projection.values[5] = -f;
            projection.values[6] = 0;
            projection.values[7] = 0;

            projection.values[8] = 0;
            projection.values[9] = 0;
            projection.values[10] = (params.zfar + params.znear) / dz;
            projection.values[11] = -1;

            projection.values[12] = 0;
            projection.values[13] = 0;
            projection.values[14] = (2 * params.zfar * params.znear) / dz;
            projection.values[15] = 0;

            return *this;
        }

        /**
         * @brief Set an orthographic projection matrix.
         *
         * @param params
         * @return Camera&
         */
        inline Camera &make(const Orthographic &params) {
            DYN_ASSERT(params.viewport.min.x != params.viewport.max.x);
            DYN_ASSERT(params.viewport.min.y != params.viewport.max.y);
            DYN_ASSERT(params.zfar != params.znear);

            float l = params.viewport.min.x;
            float r = params.viewport.max.x;

            float b = params.viewport.min.y;
            float t = params.viewport.max.y;

            float dx = r - l;
            float dy = b - t;
            float dz = params.znear - params.zfar;

            projection.values[0] = 2 / dx;
            projection.values[1] = 0;
            projection.values[2] = 0;
            projection.values[3] = 0;

            projection.values[4] = 0;
            projection.values[5] = 2 / dy;
            projection.values[6] = 0;
            projection.values[7] = 0;

            projection.values[8] = 0;
            projection.values[9] = 0;
            projection.values[10] = 1 / dz;
            projection.values[11] = 0;

            projection.values[12] = -(r + l) / dx;
            projection.values[13] = -(b + t) / dy;
            projection.values[14] = params.znear / dz;
            projection.values[15] = 1;

            return *this;
        }

        /**
         * @brief Orient the camera given its position, forward vector, and up vector.
         *
         * The forward and up vectors must be normalized.
         *
         * @param position
         * @param forward
         * @param up
         * @return Camera&
         */
        inline Camera &orient(const Vec3 &position, const Vec3 &forward, const Vec3 &up) {
            const Dynamo::Vec3 z = -forward;
            const Dynamo::Vec3 x = up.cross(z).normalize();
            const Dynamo::Vec3 y = z.cross(x);

            view.values[0] = x.x;
            view.values[1] = y.x;
            view.values[2] = z.x;
            view.values[3] = 0;

            view.values[4] = x.y;
            view.values[5] = y.y;
            view.values[6] = z.y;
            view.values[7] = 0;

            view.values[8] = x.z;
            view.values[9] = y.z;
            view.values[10] = z.z;
            view.values[11] = 0;

            view.values[12] = -x * position;
            view.values[13] = -y * position;
            view.values[14] = -z * position;
            view.values[15] = 1;

            return *this;
        }

        /**
         * @brief Orient the camera towards a target.
         *
         * @param position
         * @param target
         * @param up
         */
        inline Camera &look_at(const Vec3 &position, const Vec3 &target, const Vec3 &up) {
            const Dynamo::Vec3 z = (target - position).normalize();
            return orient(position, z, up);
        }
    };
} // namespace Dynamo