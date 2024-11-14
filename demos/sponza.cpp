// Sponza model viewer for testing the rendering engine
#include <Dynamo.hpp>

static const char *CUBEMAP_VERTEX_SHADER = R"(
#version 450

layout(set = 0, binding = 0) uniform CubemapTransform {
    mat4 vp;
} transform;

layout(location = 0) in vec3 position;
layout(location = 0) out vec3 tex_coord;

void main() {
    vec4 transformed = (transform.vp * vec4(position, 1.0));
    gl_Position = transformed.xyww;
    tex_coord = position;
}
)";

static const char *CUBEMAP_FRAGMENT_SHADER = R"(
#version 450

layout(set = 1, binding = 0) uniform samplerCube cubemap;

layout(location = 0) in vec3 tex_coord;
layout(location = 0) out vec4 out_color;

void main() {
    out_color = texture(cubemap, tex_coord);
}
)";

static const char *MODEL_VERTEX_SHADER = R"(
#version 450

layout(set = 0, binding = 0) uniform Transform {
    mat4 mvp;
} transform;

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 uv;

layout(location = 0) out vec2 out_uv;

void main() {
    gl_Position = transform.mvp * vec4(position, 1.0);
    out_uv = uv;
}
)";

static const char *MODEL_FRAGMENT_SHADER = R"(
#version 450

layout(set = 1, binding = 0) uniform sampler2D texsampler;

layout(location = 0) in vec2 uv;
layout(location = 0) out vec4 color;

void main() {
    color = texture(texsampler, uv);
}
)";

static const Dynamo::Vec3 SKYBOX_GEOM_POSITIONS[] = {
    Dynamo::Vec3(-1.0f, 1.0f, -1.0f),  Dynamo::Vec3(-1.0f, -1.0f, -1.0f), Dynamo::Vec3(1.0f, -1.0f, -1.0f),
    Dynamo::Vec3(1.0f, -1.0f, -1.0f),  Dynamo::Vec3(1.0f, 1.0f, -1.0f),   Dynamo::Vec3(-1.0f, 1.0f, -1.0f),

    Dynamo::Vec3(-1.0f, -1.0f, 1.0f),  Dynamo::Vec3(-1.0f, -1.0f, -1.0f), Dynamo::Vec3(-1.0f, 1.0f, -1.0f),
    Dynamo::Vec3(-1.0f, 1.0f, -1.0f),  Dynamo::Vec3(-1.0f, 1.0f, 1.0f),   Dynamo::Vec3(-1.0f, -1.0f, 1.0f),

    Dynamo::Vec3(1.0f, -1.0f, -1.0f),  Dynamo::Vec3(1.0f, -1.0f, 1.0f),   Dynamo::Vec3(1.0f, 1.0f, 1.0f),
    Dynamo::Vec3(1.0f, 1.0f, 1.0f),    Dynamo::Vec3(1.0f, 1.0f, -1.0f),   Dynamo::Vec3(1.0f, -1.0f, -1.0f),

    Dynamo::Vec3(-1.0f, -1.0f, 1.0f),  Dynamo::Vec3(-1.0f, 1.0f, 1.0f),   Dynamo::Vec3(1.0f, 1.0f, 1.0f),
    Dynamo::Vec3(1.0f, 1.0f, 1.0f),    Dynamo::Vec3(1.0f, -1.0f, 1.0f),   Dynamo::Vec3(-1.0f, -1.0f, 1.0f),

    Dynamo::Vec3(-1.0f, 1.0f, -1.0f),  Dynamo::Vec3(1.0f, 1.0f, -1.0f),   Dynamo::Vec3(1.0f, 1.0f, 1.0f),
    Dynamo::Vec3(1.0f, 1.0f, 1.0f),    Dynamo::Vec3(-1.0f, 1.0f, 1.0f),   Dynamo::Vec3(-1.0f, 1.0f, -1.0f),

    Dynamo::Vec3(-1.0f, -1.0f, -1.0f), Dynamo::Vec3(-1.0f, -1.0f, 1.0f),  Dynamo::Vec3(1.0f, -1.0f, -1.0f),
    Dynamo::Vec3(1.0f, -1.0f, -1.0f),  Dynamo::Vec3(-1.0f, -1.0f, 1.0f),  Dynamo::Vec3(1.0f, -1.0f, 1.0f),
};

struct Model {
    std::vector<Dynamo::Graphics::Model> groups;
    std::vector<Dynamo::Graphics::Uniform> uniforms;
};

Model build_obj_model(Dynamo::Graphics::Renderer &renderer, const std::string filepath) {
    // Vertex shader
    Dynamo::Graphics::ShaderDescriptor vertex_shader_descriptor;
    vertex_shader_descriptor.name = "Vertex";
    vertex_shader_descriptor.code = MODEL_VERTEX_SHADER;
    vertex_shader_descriptor.stage = Dynamo::Graphics::ShaderStage::Vertex;
    Dynamo::Graphics::Shader vertex = renderer.build_shader(vertex_shader_descriptor);

    // Fragment shader
    Dynamo::Graphics::ShaderDescriptor fragment_shader_descriptor;
    fragment_shader_descriptor.name = "Fragment";
    fragment_shader_descriptor.code = MODEL_FRAGMENT_SHADER;
    fragment_shader_descriptor.stage = Dynamo::Graphics::ShaderStage::Fragment;
    Dynamo::Graphics::Shader fragment = renderer.build_shader(fragment_shader_descriptor);

    // Pipeline
    Dynamo::Graphics::PipelineDescriptor pipeline_descriptor;
    pipeline_descriptor.vertex = vertex;
    pipeline_descriptor.fragment = fragment;
    Dynamo::Graphics::Pipeline pipeline = renderer.build_pipeline(pipeline_descriptor);

    // Build submodels
    Model model;
    for (Dynamo::Asset::Obj::MeshGroup &group : Dynamo::Asset::Obj(filepath).groups) {
        // Skip empty groups or groups with no diffuse texture
        if (group.positions.empty() || group.material.diffuse_filepath.empty()) continue;

        Dynamo::Graphics::MeshDescriptor mesh_descriptor(group.positions.size(), Dynamo::Graphics::IndexType::U32);
        mesh_descriptor.add_vertex_attribute(group.positions.data());
        mesh_descriptor.add_vertex_attribute(group.uvs.data());
        mesh_descriptor.indices = group.indices;

        Dynamo::Graphics::Mesh mesh = renderer.build_mesh(mesh_descriptor);
        Dynamo::Graphics::UniformGroup uniforms = renderer.build_uniforms(pipeline);
        Dynamo::Graphics::Uniform texture_uniform = renderer.get_uniform(uniforms, "texsampler").value();

        // Bind the texture
        std::filesystem::path folder = std::filesystem::relative(filepath).parent_path();
        std::string texture_path = folder.string() + "/" + group.material.diffuse_filepath;
        Dynamo::Graphics::TextureDescriptor texture_descriptor = Dynamo::Asset::load_texture(texture_path);
        Dynamo::Asset::generate_texture_mipmap(texture_descriptor, 10);
        Dynamo::Graphics::Texture texture = renderer.build_texture(texture_descriptor);
        renderer.bind_texture(texture_uniform, texture);

        // Register the transform uniform to update per-frame
        Dynamo::Graphics::Uniform transform_uniform = renderer.get_uniform(uniforms, "transform").value();
        model.uniforms.push_back(transform_uniform);

        // Register mesh group
        model.groups.push_back({mesh, pipeline, uniforms, 0});
    }
    return model;
}

Model build_skybox(Dynamo::Graphics::Renderer &renderer) {
    // Skybox vertex shader
    Dynamo::Graphics::ShaderDescriptor vertex_shader_descriptor;
    vertex_shader_descriptor.name = "Cubemap Vertex";
    vertex_shader_descriptor.code = CUBEMAP_VERTEX_SHADER;
    vertex_shader_descriptor.stage = Dynamo::Graphics::ShaderStage::Vertex;
    Dynamo::Graphics::Shader vertex = renderer.build_shader(vertex_shader_descriptor);

    // Skybox fragment shader
    Dynamo::Graphics::ShaderDescriptor fragment_shader_descriptor;
    fragment_shader_descriptor.name = "Cubemap Fragment";
    fragment_shader_descriptor.code = CUBEMAP_FRAGMENT_SHADER;
    fragment_shader_descriptor.stage = Dynamo::Graphics::ShaderStage::Fragment;
    Dynamo::Graphics::Shader fragment = renderer.build_shader(fragment_shader_descriptor);

    // Skybox pipeline
    Dynamo::Graphics::PipelineDescriptor pipeline_descriptor;
    pipeline_descriptor.vertex = vertex;
    pipeline_descriptor.fragment = fragment;
    pipeline_descriptor.depth_test_op = Dynamo::Graphics::CompareOp::LessEqual;
    Dynamo::Graphics::Pipeline pipeline = renderer.build_pipeline(pipeline_descriptor);
    Dynamo::Graphics::UniformGroup uniforms = renderer.build_uniforms(pipeline);

    // Skybox mesh
    Dynamo::Graphics::MeshDescriptor mesh_descriptor(36, Dynamo::Graphics::IndexType::None);
    mesh_descriptor.add_vertex_attribute(SKYBOX_GEOM_POSITIONS);
    Dynamo::Graphics::Mesh mesh = renderer.build_mesh(mesh_descriptor);

    // Bind cubemap texture
    Dynamo::Graphics::TextureDescriptor texture_descriptors =
        Dynamo::Asset::load_texture_cubemap("../assets/textures/skybox/right.jpg",
                                            "../assets/textures/skybox/left.jpg",
                                            "../assets/textures/skybox/top.jpg",
                                            "../assets/textures/skybox/bottom.jpg",
                                            "../assets/textures/skybox/front.jpg",
                                            "../assets/textures/skybox/back.jpg");
    Dynamo::Graphics::Texture texture = renderer.build_texture(texture_descriptors);
    Dynamo::Graphics::Uniform cubemap_uniform = renderer.get_uniform(uniforms, "cubemap").value();
    renderer.bind_texture(cubemap_uniform, texture, 0);

    // Register the model and per-frame uniform
    Model model;
    model.groups.push_back({mesh, pipeline, uniforms, 1});
    model.uniforms.push_back(renderer.get_uniform(uniforms, "transform").value());

    return model;
}

int main() {
    Dynamo::ApplicationSettings config;
    config.title = "Jukebox!";
    config.window_width = 640;
    config.window_height = 480;
    config.root_asset_directory = "../assets/";
    Dynamo::Application app(config);

    // --- Models ---
    Model model = build_obj_model(app.renderer(), "../assets/models/sponza/sponza.obj");
    Model skybox = build_skybox(app.renderer());

    // --- Controls ---
    float dt_ave = 0;
    bool projection_toggle = true;

    Dynamo::Vec3 camera_position(2, 2, -2);
    Dynamo::Vec3 camera_direction;
    Dynamo::Vec3 camera_up(0, 1, 0);

    float yaw = 135;
    float pitch = -35;

    Dynamo::Vec2 prev_mouse = app.display().get_window_size() / 2;
    while (app.is_running()) {
        if (app.input().is_pressed(Dynamo::KeyCode::Escape)) {
            app.quit();
        }

        if (app.input().is_pressed(Dynamo::MouseCode::Left)) {
            app.input().capture_mouse();
        }
        if (app.input().is_pressed(Dynamo::MouseCode::Right)) {
            app.input().release_mouse();
        }

        float dt = 0.05;
        if (app.input().is_pressed(Dynamo::KeyCode::P)) {
            projection_toggle = !projection_toggle;
        }
        if (app.input().is_down(Dynamo::KeyCode::W)) {
            camera_position += camera_direction * dt;
        }
        if (app.input().is_down(Dynamo::KeyCode::S)) {
            camera_position -= camera_direction * dt;
        }
        if (app.input().is_down(Dynamo::KeyCode::A)) {
            Dynamo::Vec3 camera_right = camera_direction.cross(camera_up);
            camera_position -= camera_right * dt;
        }
        if (app.input().is_down(Dynamo::KeyCode::D)) {
            Dynamo::Vec3 camera_right = camera_direction.cross(camera_up);
            camera_position += camera_right * dt;
        }
        if (app.input().is_down(Dynamo::KeyCode::Q)) {
            camera_position -= camera_up * dt;
        }
        if (app.input().is_down(Dynamo::KeyCode::E)) {
            camera_position += camera_up * dt;
        }

        Dynamo::Vec2 mouse_position = app.input().get_mouse_position();
        float xoffset = (mouse_position.x - prev_mouse.x) * 0.1;
        float yoffset = (mouse_position.y - prev_mouse.y) * 0.1;
        prev_mouse = mouse_position;
        yaw += xoffset;
        pitch -= yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        camera_direction.x = cos(Dynamo::to_radians(yaw)) * cos(Dynamo::to_radians(pitch));
        camera_direction.y = sin(Dynamo::to_radians(pitch));
        camera_direction.z = sin(Dynamo::to_radians(yaw)) * cos(Dynamo::to_radians(pitch));
        camera_direction.normalize();

        unsigned long long frames = app.clock().frames();
        unsigned update_rate = 64;
        dt_ave += app.clock().delta().count();
        if (frames % update_rate == 0) {
            dt_ave = dt_ave / update_rate;
            app.display().set_title(std::to_string(dt_ave * 1000) + " ms");
            dt_ave = 0;
        }

        Dynamo::Vec2 display_size = app.display().get_window_size();
        Dynamo::Perspective perspective;
        perspective.fovy = Dynamo::to_radians(45);
        perspective.aspect = display_size.x / display_size.y;
        perspective.znear = 0.1;
        perspective.zfar = 10000;

        Dynamo::Orthographic orthographic;
        orthographic.viewport.min = {-1, -1};
        orthographic.viewport.max = {1, 1};
        orthographic.znear = 0.1;
        orthographic.zfar = 10000;

        // Camera projection toggle
        Dynamo::Camera camera;
        if (projection_toggle) {
            camera.make(perspective).orient(camera_position, camera_direction, camera_up);
        } else {
            camera.make(orthographic).orient(camera_position, camera_direction, camera_up);
        }

        // Update skybox uniforms
        for (const Dynamo::Graphics::Uniform uniform : skybox.uniforms) {
            Dynamo::Mat4 view = camera.view;
            // Ignore translation component
            view.values[12] = 0;
            view.values[13] = 0;
            view.values[14] = 0;
            view.values[15] = 1;

            alignas(16) Dynamo::Mat4 vp = camera.projection * view;
            app.renderer().write_uniform(uniform, &vp);
        }

        // Update model uniforms
        for (const Dynamo::Graphics::Uniform uniform : model.uniforms) {
            Dynamo::Mat4 model = Dynamo::Mat4({0, 0, 0}, {0, 0, 0, 1}, {.01, .01, .01});
            alignas(16) Dynamo::Mat4 mvp = camera.projection * camera.view * model;
            app.renderer().write_uniform(uniform, &mvp);
        }

        // Draw the model groups
        for (const Dynamo::Graphics::Model &group : model.groups) {
            app.renderer().draw(group);
        }
        for (const Dynamo::Graphics::Model &group : skybox.groups) {
            app.renderer().draw(group);
        }

        app.update();
    }
    return 0;
}
