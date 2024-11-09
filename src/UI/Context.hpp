#pragma once

#include <Graphics/Mesh.hpp>
#include <Graphics/Model.hpp>
#include <Graphics/Pipeline.hpp>
#include <Graphics/Renderer.hpp>
#include <Graphics/Shader.hpp>
#include <UI/Widget.hpp>

namespace Dynamo::UI {
    /**
     * @brief GUI Context.
     *
     */
    class Context {
        using MaterialPool = std::vector<WidgetMaterial>;

        Display &_display;
        Graphics::Renderer &_renderer;

        Graphics::Mesh _quad;

        Graphics::Shader _container_vertex;
        Graphics::Shader _container_fragment;

        std::vector<MaterialPool> _material_pools;

        std::vector<Widget> _widgets;

        WidgetMaterial build_material(WidgetType type);

        Widget build_widget(WidgetType type);

      public:
        Context(Display &display, Graphics::Renderer &renderer);

        /**
         * @brief Begin a container subcontext.
         *
         * @param title
         * @param color
         * @param radius
         */
        void begin(const std::string &title, const Color &color = Color(0.1, 0.07, 0.03), float radius = 5);

        /**
         * @brief End the container subcontext.
         *
         */
        void end();

        /**
         * @brief Draw submitted GUI elements.
         *
         */
        void draw();
    };
} // namespace Dynamo::UI