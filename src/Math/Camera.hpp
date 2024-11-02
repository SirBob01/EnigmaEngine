#pragma once

#include <Math/Mat4.hpp>
#include <Math/Vec2.hpp>

namespace Dynamo {
    /**
     * @brief 3D Camera.
     *
     */
    struct Camera {
        Mat4 projection;
        Mat4 view;

        /**
         * @brief Create a perspective projection camera.
         *
         * @param fovy
         * @param aspect
         * @param znear
         * @param zfar
         */
        Camera(float fovy, float aspect, float znear, float zfar) {
            DYN_ASSERT(aspect != 0);
            DYN_ASSERT(zfar != znear);

            float f = 1.0 / std::tan(0.5 * fovy);
            float dz = znear - zfar;
            projection.values[0] = f / aspect;
            projection.values[1] = 0;
            projection.values[2] = 0;
            projection.values[3] = 0;

            projection.values[4] = 0;
            projection.values[5] = -f;
            projection.values[6] = 0;
            projection.values[7] = 0;

            projection.values[8] = 0;
            projection.values[9] = 0;
            projection.values[10] = (zfar + znear) / dz;
            projection.values[11] = -1;

            projection.values[12] = 0;
            projection.values[13] = 0;
            projection.values[14] = (2 * zfar * znear) / dz;
            projection.values[15] = 0;
        }

        /**
         * @brief Define the view matrix.
         *
         * @param eye
         * @param target
         * @param up
         */
        inline void look_at(const Vec3 &eye, const Vec3 &target, const Vec3 &up) {
            Dynamo::Vec3 z = (eye - target).normalize();
            Dynamo::Vec3 x = up.cross(z).normalize();
            Dynamo::Vec3 y = z.cross(x);

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

            view.values[12] = -x * eye;
            view.values[13] = -y * eye;
            view.values[14] = -z * eye;
            view.values[15] = 1;
        }
    };
} // namespace Dynamo