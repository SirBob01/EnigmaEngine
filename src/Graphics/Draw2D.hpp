#pragma once

#include <Graphics/Renderer.hpp>
#include <Math/Box2.hpp>
#include <Math/Vec2.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief 2D graphics submodule.
     *
     */
    class Draw2D {
        enum class ShapeType {
            Rect,
            Ellipse,
            MaxShapeType,
        };

        struct ShapeMaterial {
            ShapeType type;
            Pipeline pipeline;
            Uniform uniform;
            unsigned z_order;
        };

        using ShapePool = std::vector<ShapeMaterial>;

        Display &_display;
        Renderer &_renderer;

        Mesh _quad_mesh;

        Shader _rect_vertex;
        Shader _rect_fragment;

        Shader _ellipse_vertex;
        Shader _ellipse_fragment;

        std::vector<ShapePool> _shapes_inactive;
        std::vector<ShapePool> _shapes_active;

        ShapeMaterial build_material(ShapeType type);

      public:
        Draw2D(Display &display, Renderer &renderer);

        /**
         * @brief Draw a rectangle.
         *
         * @param box
         * @param color
         * @param radius
         */
        void rect(const Box2 &box, const Color &color, float radius = 0);

        /**
         * @brief Draw a circle.
         *
         * @param center
         * @param radius
         * @param color
         */
        void circle(const Vec2 &center, unsigned radius, const Color &color);

        /**
         * @brief Draw an ellipse.
         *
         * @param center
         * @param radii
         * @param color
         */
        void ellipse(const Vec2 &center, const Vec2 &radii, const Color &color);

        /**
         * @brief Submit draw calls to the renderer.
         *
         */
        void draw();
    };
} // namespace Dynamo::Graphics