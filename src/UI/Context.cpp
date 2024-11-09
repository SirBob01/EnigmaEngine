#include <UI/Context.hpp>
#include <UI/Shaders.hpp>
#include <limits>

namespace Dynamo::UI {
    Context::Context(Display &display, Graphics::Renderer &renderer) :
        _display(display), _renderer(renderer), _material_pools(static_cast<unsigned>(WidgetType::MaxWidgetType)) {
        Graphics::MeshDescriptor quad_descriptor(4, Dynamo::Graphics::IndexType::U16);
        quad_descriptor.add_vertex_attribute(QUAD_VERTICES);
        for (unsigned i = 0; i < sizeof(QUAD_INDICES) / sizeof(QUAD_INDICES[0]); i++) {
            quad_descriptor.indices.push_back(QUAD_INDICES[i]);
        }
        _quad = _renderer.build_mesh(quad_descriptor);

        Graphics::ShaderDescriptor container_vertex_descriptor;
        container_vertex_descriptor.name = "ContainerVertex";
        container_vertex_descriptor.code = CONTAINER_VERTEX_SHADER;
        container_vertex_descriptor.stage = Graphics::ShaderStage::Vertex;
        _container_vertex = _renderer.build_shader(container_vertex_descriptor);

        Graphics::ShaderDescriptor container_fragment_descriptor;
        container_fragment_descriptor.name = "ContainerFragment";
        container_fragment_descriptor.code = CONTAINER_FRAGMENT_SHADER;
        container_fragment_descriptor.stage = Graphics::ShaderStage::Fragment;
        _container_fragment = _renderer.build_shader(container_fragment_descriptor);
    }

    WidgetMaterial Context::build_material(WidgetType type) {
        Graphics::PipelineDescriptor descriptor;
        switch (type) {
        case WidgetType::Container:
            descriptor.vertex = _container_vertex;
            descriptor.fragment = _container_fragment;
        case WidgetType::MaxWidgetType:
            break;
        }

        WidgetMaterial material;
        material.pipeline = _renderer.build_pipeline(descriptor);
        material.uniform = _renderer.get_uniform(material.pipeline, "settings").value();

        return material;
    }

    Widget Context::build_widget(WidgetType type) {
        Widget widget;
        widget.type = type;
        widget.mesh = _quad;

        unsigned pool_index = static_cast<unsigned>(widget.type);
        MaterialPool &pool = _material_pools[pool_index];
        if (pool.size()) {
            widget.material = pool.back();
            pool.pop_back();
        } else {
            widget.material = build_material(type);
        }

        return widget;
    }

    void Context::begin(const std::string &title, const Color &color, float radius) {
        // Write the uniforms for this widget.
        struct {
            Vec2 viewport;
            Vec2 offset;
            Vec2 scale;
            alignas(16) Color color;
            alignas(16) float radius;
        } uniform_data;
        uniform_data.viewport = _display.get_window_size();
        uniform_data.offset = {0, 0};
        uniform_data.scale = {100, 100};
        uniform_data.color = Color(0.05, 0.05, 0.07);
        uniform_data.radius = radius;

        Widget widget = build_widget(WidgetType::Container);
        _renderer.write_uniform(widget.material.uniform, &uniform_data);
        _widgets.push_back(widget);
    }

    void Context::draw() {
        for (const Widget &widget : _widgets) {
            // Draw widget
            Dynamo::Graphics::Model model;
            model.mesh = widget.mesh;
            model.pipeline = widget.material.pipeline;
            model.group = std::numeric_limits<int>::max();
            _renderer.draw(model);

            // Add material back in the pool for reuse
            unsigned pool_index = static_cast<unsigned>(widget.type);
            _material_pools[pool_index].push_back(widget.material);
        }
        _widgets.clear();
    }
} // namespace Dynamo::UI