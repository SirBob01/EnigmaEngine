#include <Graphics/Draw2D.hpp>
#include <Math/Vec2.hpp>

namespace Dynamo::Graphics {
    const Vec2 QUAD_VERTICES[4] = {
        Vec2(0, 0),
        Vec2(1, 0),
        Vec2(1, 1),
        Vec2(0, 1),
    };

    static const unsigned QUAD_INDICES[6] = {
        2,
        1,
        0,
        0,
        3,
        2,
    };

    const std::string QUAD_VERTEX_SHADER = R"(
    #version 450
    layout(location = 0) in vec2 position;

    layout(set = 0, binding = 0) uniform QuadUniform {
        vec2 canvas;
        vec2 offset;
        vec2 scale;
        vec4 color;
        float radius;
    } settings;

    layout(location = 0) out vec4 color;
    layout(location = 1) out vec2 scale;
    layout(location = 2) out vec2 uv;
    layout(location = 3) out float radius;

    void main() {
        vec2 canvas_space = position * settings.scale + settings.offset;
        vec2 clip_space = (canvas_space / settings.canvas) * 2 - 1;
        gl_Position = vec4(clip_space, 0.0, 1.0);

        color = settings.color;
        scale = settings.scale;
        uv = position;
        radius = settings.radius;
    }
    )";

    const std::string QUAD_FRAGMENT_SHADER = R"(
    #version 450
    layout(location = 0) in vec4 color;
    layout(location = 1) in vec2 scale;
    layout(location = 2) in vec2 uv;
    layout(location = 3) flat in float radius;

    layout(location = 0) out vec4 frag;

    float rect_distance() {
        vec2 extents = scale * 0.5;
        vec2 coords = abs(2 * (uv - vec2(0.5))) * (extents + radius);
        vec2 delta = max(coords - extents, 0);
        return length(delta);
    }

    void main() {
        if (rect_distance() > radius) {
            discard;
        }
        frag = color;
    }
    )";

    const std::string ELLIPSE_VERTEX_SHADER = R"(
    #version 450
    layout(location = 0) in vec2 position;

    layout(set = 0, binding = 0) uniform EllipseUniform {
        vec2 canvas;
        vec2 center;
        vec2 radii;
        vec4 color;
    } settings;

    layout(location = 0) out vec4 color;
    layout(location = 1) out vec2 uv;
    layout(location = 2) out vec2 radii;

    void main() {
        vec2 diameters = 2 * settings.radii;
        vec2 canvas_space = position * diameters - settings.radii + settings.center;
        vec2 clip_space = (canvas_space / settings.canvas) * 2 - 1;
        gl_Position = vec4(clip_space, 0.0, 1.0);

        color = settings.color;
        uv = position;
        radii = settings.radii;
    }
    )";

    const std::string ELLIPSE_FRAGMENT_SHADER = R"(
    #version 450
    layout(location = 0) in vec4 color;
    layout(location = 1) in vec2 uv;
    layout(location = 2) in vec2 radii;

    layout(location = 0) out vec4 frag;


    void main() {
        vec2 diameters = 2 * radii;
        vec2 point = uv * diameters - radii;
        vec2 elem = (point * point) / (radii * radii);
        if (elem.x + elem.y > 1) {
            discard;
        }
        frag = color;
    }
    )";

    Draw2D::Draw2D(Display &display, Renderer &renderer) :
        _display(display), _renderer(renderer), _shapes_inactive(static_cast<unsigned>(ShapeType::MaxShapeType)),
        _shapes_active(static_cast<unsigned>(ShapeType::MaxShapeType)) {
        MeshDescriptor mesh_descriptor(4, IndexType::U16);
        mesh_descriptor.add_vertex_attribute(QUAD_VERTICES);
        for (unsigned i = 0; i < sizeof(QUAD_INDICES) / sizeof(QUAD_INDICES[0]); i++) {
            mesh_descriptor.indices.push_back(QUAD_INDICES[i]);
        }
        _quad_mesh = _renderer.build_mesh(mesh_descriptor);

        // Rect
        ShaderDescriptor shader_descriptor;
        shader_descriptor.name = "QuadVertex";
        shader_descriptor.code = QUAD_VERTEX_SHADER;
        shader_descriptor.stage = ShaderStage::Vertex;
        _rect_vertex = _renderer.build_shader(shader_descriptor);

        shader_descriptor.name = "QuadFragment";
        shader_descriptor.code = QUAD_FRAGMENT_SHADER;
        shader_descriptor.stage = ShaderStage::Fragment;
        _rect_fragment = _renderer.build_shader(shader_descriptor);

        // Ellipse
        shader_descriptor.name = "EllipseVertex";
        shader_descriptor.code = ELLIPSE_VERTEX_SHADER;
        shader_descriptor.stage = ShaderStage::Vertex;
        _ellipse_vertex = _renderer.build_shader(shader_descriptor);

        shader_descriptor.name = "EllipseFragment";
        shader_descriptor.code = ELLIPSE_FRAGMENT_SHADER;
        shader_descriptor.stage = ShaderStage::Fragment;
        _ellipse_fragment = _renderer.build_shader(shader_descriptor);
    }

    Draw2D::ShapeMaterial Draw2D::build_material(ShapeType type) {
        ShapeMaterial material;
        material.type = type;

        // Reuse from the inactive pool if available
        ShapePool &inactive = _shapes_inactive[static_cast<unsigned>(type)];
        if (inactive.size()) {
            material = inactive.back();
            inactive.pop_back();
            return material;
        }

        PipelineDescriptor pipeline_descriptor;
        switch (type) {
        case ShapeType::Rect:
            pipeline_descriptor.vertex = _rect_vertex;
            pipeline_descriptor.fragment = _rect_fragment;
            break;
        case ShapeType::Ellipse:
            pipeline_descriptor.vertex = _ellipse_vertex;
            pipeline_descriptor.fragment = _ellipse_fragment;
            break;
        case ShapeType::MaxShapeType:
            break;
        }
        material.pipeline = _renderer.build_pipeline(pipeline_descriptor);
        material.uniform = _renderer.get_uniform(material.pipeline, "settings").value();
        material.z_order = _shapes_active[static_cast<unsigned>(type)].size();
        return material;
    }

    void Draw2D::rect(const Box2 &box, const Color &color, float radius) {
        struct {
            Vec2 canvas;
            Vec2 offset;
            Vec2 scale;
            alignas(16) Color color;
            alignas(16) float radius;
        } uniform_data;
        uniform_data.canvas = _display.get_window_size();
        uniform_data.offset = box.min;
        uniform_data.scale = box.max - box.min;
        uniform_data.color = color;
        uniform_data.radius = radius;

        ShapeMaterial material = build_material(ShapeType::Rect);
        _renderer.write_uniform(material.uniform, &uniform_data);
        _shapes_active[static_cast<unsigned>(material.type)].push_back(material);
    }

    void Draw2D::circle(const Vec2 &center, unsigned radius, const Color &color) {
        struct {
            Vec2 canvas;
            Vec2 center;
            alignas(16) Vec2 radii;
            alignas(16) Color color;
        } uniform_data;
        uniform_data.canvas = _display.get_window_size();
        uniform_data.center = center;
        uniform_data.radii.x = radius;
        uniform_data.radii.y = radius;
        uniform_data.color = color;

        ShapeMaterial material = build_material(ShapeType::Ellipse);
        _renderer.write_uniform(material.uniform, &uniform_data);
        _shapes_active[static_cast<unsigned>(material.type)].push_back(material);
    }

    void Draw2D::ellipse(const Vec2 &center, const Vec2 &radii, const Color &color) {
        struct {
            Vec2 canvas;
            Vec2 center;
            alignas(16) Vec2 radii;
            alignas(16) Color color;
        } uniform_data;
        uniform_data.canvas = _display.get_window_size();
        uniform_data.center = center;
        uniform_data.radii = radii;
        uniform_data.color = color;

        ShapeMaterial material = build_material(ShapeType::Ellipse);
        _renderer.write_uniform(material.uniform, &uniform_data);
        _shapes_active[static_cast<unsigned>(material.type)].push_back(material);
    }

    void Draw2D::draw() {
        for (unsigned i = 0; i < _shapes_active.size(); i++) {
            // Sort to preserve draw order
            std::sort(_shapes_active[i].begin(), _shapes_active[i].end(), [](ShapeMaterial &a, ShapeMaterial &b) {
                return a.z_order > b.z_order;
            });
            for (const ShapeMaterial &material : _shapes_active[i]) {
                _renderer.draw({_quad_mesh, material.pipeline, std::numeric_limits<int>::max()});
                _shapes_inactive[i].push_back(material);
            }
            _shapes_active[i].clear();
        }
    }
} // namespace Dynamo::Graphics