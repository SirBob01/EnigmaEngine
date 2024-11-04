// Playground source file for testing new features.
#include <string>
#include <tiny_obj_loader.h>

#include <Dynamo.hpp>

Dynamo::Graphics::MeshDescriptor load_mesh(const std::string &filename) {
    struct ObjVertex {
        Dynamo::Vec3 position;
        Dynamo::Vec2 uv;

        inline bool operator==(const ObjVertex &other) const { return position == other.position && uv == other.uv; }

        struct Hash {
            inline size_t operator()(const ObjVertex &vertex) const {
                size_t hash0 = std::hash<Dynamo::Vec3>{}(vertex.position);
                size_t hash1 = std::hash<Dynamo::Vec2>{}(vertex.uv);

                return hash0 ^ (hash1 << 1);
            }
        };
    };

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filename.c_str())) {
        if (warn.length()) {
            Dynamo::Log::warn("{}", warn);
        }
        if (err.length()) {
            Dynamo::Log::error("{}", err);
        }
    }

    std::unordered_map<ObjVertex, unsigned, ObjVertex::Hash> unique;

    std::vector<Dynamo::Vec3> positions;
    std::vector<Dynamo::Vec2> uvs;
    std::vector<unsigned> indices;
    for (const auto &shape : shapes) {
        for (const auto &index : shape.mesh.indices) {
            ObjVertex vertex;
            vertex.position = Dynamo::Vec3(attrib.vertices[3 * index.vertex_index + 0],
                                           attrib.vertices[3 * index.vertex_index + 1],
                                           attrib.vertices[3 * index.vertex_index + 2]);
            vertex.uv = Dynamo::Vec2(attrib.texcoords[2 * index.texcoord_index + 0],
                                     1 - attrib.texcoords[2 * index.texcoord_index + 1]);

            if (unique.count(vertex) == 0) {
                unique[vertex] = positions.size();

                positions.push_back(vertex.position);
                uvs.push_back(vertex.uv);
            }

            indices.push_back(unique[vertex]);
        }
    }

    Dynamo::Vec3 instance;
    Dynamo::Graphics::MeshDescriptor descriptor(positions.size(), 1, Dynamo::Graphics::IndexType::U32);
    descriptor.add_instance_attribute(&instance);
    descriptor.add_vertex_attribute(positions.data());
    descriptor.add_vertex_attribute(uvs.data());
    descriptor.indices = indices;
    return descriptor;
}

struct Transform {
    alignas(16) Dynamo::Mat4 mvp;
};

struct SkyboxTransform {
    alignas(16) Dynamo::Mat4 vp;
};

struct Timer {
    alignas(16) float time;
};

struct Texture {
    alignas(16) unsigned index;
};

static const char *CUBEMAP_VERTEX_SHADER = R"(
#version 450

layout(set = 0, binding = 0) uniform CubemapTransform {
    mat4 vp;
} transform;

layout(location = 0) in vec3 position;
layout(location = 0) out vec3 tex_coord;

void main() {
    tex_coord = position;
    gl_Position = transform.vp * vec4(position, 1.0);
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

layout(location = 0) in vec3 instance_offset;
layout(location = 1) in vec3 position;
layout(location = 2) in vec2 uv;

layout(location = 0) out vec2 tex_coord;

void main() {
    gl_Position = transform.mvp * vec4(position + instance_offset, 1.0);
    tex_coord = uv;
}
)";

static const char *MODEL_FRAGMENT_SHADER = R"(
#version 450

layout(push_constant) uniform Timer {
    float time;
} timer;

layout(set = 1, binding = 0) uniform sampler2D tex_sampler[2];
layout(set = 1, binding = 1) uniform Texture {
    uint index;
} utexture;

layout(location = 0) in vec2 tex_coord;

layout(location = 0) out vec4 out_color;

void main() {
    out_color = texture(tex_sampler[utexture.index], tex_coord);
    // out_color *= ((sin(timer.time) + 1) * 0.5);
}
)";

static const Dynamo::Vec3 GEOM_OFFSETS[2] = {
    Dynamo::Vec3(-0.5, 0, 0),
    Dynamo::Vec3(0.5, 0, 0),
};
static const Dynamo::Vec3 GEOM_POSITIONS[4] = {
    Dynamo::Vec3(-0.5, -0.5, 0.0),
    Dynamo::Vec3(0.5, -0.5, 0.0),
    Dynamo::Vec3(0.5, 0.5, 0.0),
    Dynamo::Vec3(-0.5, 0.5, 0.0),
};
static const Dynamo::Vec2 GEOM_UVS[4] = {
    Dynamo::Vec2(0.0, 0.0),
    Dynamo::Vec2(1.0, 0.0),
    Dynamo::Vec2(1.0, 1.0),
    Dynamo::Vec2(0.0, 1.0),
};

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

int main() {
    Dynamo::ApplicationSettings config;
    config.title = "Jukebox!";
    config.window_width = 640;
    config.window_height = 480;
    config.root_asset_directory = "../assets/";

    Dynamo::Application app(config);
    Dynamo::Sound::Buffer music = Dynamo::load_sound("../assets/audio/mono.ogg");
    Dynamo::Sound::Buffer fx = Dynamo::load_sound("../assets/audio/fusion_boom.ogg");

    Dynamo::Sound::Listener &listener = app.jukebox().listener();

    // Filters
    Dynamo::Sound::Distance distance;
    Dynamo::Sound::Binaural binaural;
    Dynamo::Sound::Stereo stereo;
    Dynamo::Sound::Amplify amplify;
    Dynamo::Sound::FilterSequence sequence;
    sequence.push(binaural);
    sequence.push(distance);
    sequence.push(amplify);

    amplify.gain = 0.5;

    // Load the model

    // Build the model geometry
    Dynamo::Graphics::MeshDescriptor model_mesh_descriptor = load_mesh("../assets/models/viking_room.obj");
    Dynamo::Graphics::Mesh model_mesh = app.renderer().build_mesh(model_mesh_descriptor);

    // Build the flat mesh
    Dynamo::Graphics::MeshDescriptor flat_mesh_descriptor(4, 2, Dynamo::Graphics::IndexType::U16);
    flat_mesh_descriptor.add_instance_attribute(GEOM_OFFSETS);
    flat_mesh_descriptor.add_vertex_attribute(GEOM_POSITIONS);
    flat_mesh_descriptor.add_vertex_attribute(GEOM_UVS);
    flat_mesh_descriptor.indices = {0, 1, 2, 2, 3, 0};
    Dynamo::Graphics::Mesh flat_mesh = app.renderer().build_mesh(flat_mesh_descriptor);

    // Build the skybox mesh
    Dynamo::Graphics::MeshDescriptor skybox_mesh_descriptor(36, Dynamo::Graphics::IndexType::None);
    skybox_mesh_descriptor.add_vertex_attribute(SKYBOX_GEOM_POSITIONS);
    Dynamo::Graphics::Mesh skybox_mesh = app.renderer().build_mesh(skybox_mesh_descriptor);

    // Build model vertex shader
    Dynamo::Graphics::ShaderDescriptor vertex_shader_descriptor;
    vertex_shader_descriptor.name = "Vertex";
    vertex_shader_descriptor.code = MODEL_VERTEX_SHADER;
    vertex_shader_descriptor.stage = Dynamo::Graphics::ShaderStage::Vertex;
    vertex_shader_descriptor.shared_uniforms.push_back("pos");
    Dynamo::Graphics::Shader vertex = app.renderer().build_shader(vertex_shader_descriptor);

    // Build model fragment shader
    Dynamo::Graphics::ShaderDescriptor fragment_shader_descriptor;
    fragment_shader_descriptor.name = "Fragment";
    fragment_shader_descriptor.code = MODEL_FRAGMENT_SHADER;
    fragment_shader_descriptor.stage = Dynamo::Graphics::ShaderStage::Fragment;
    fragment_shader_descriptor.shared_uniforms.push_back("time");
    Dynamo::Graphics::Shader fragment = app.renderer().build_shader(fragment_shader_descriptor);

    // Build cubemap vertex shader
    Dynamo::Graphics::ShaderDescriptor skybox_vertex_shader_descriptor;
    skybox_vertex_shader_descriptor.name = "Cubemap Vertex";
    skybox_vertex_shader_descriptor.code = CUBEMAP_VERTEX_SHADER;
    skybox_vertex_shader_descriptor.stage = Dynamo::Graphics::ShaderStage::Vertex;
    Dynamo::Graphics::Shader skybox_vertex = app.renderer().build_shader(skybox_vertex_shader_descriptor);

    // Build cubemap fragment shader
    Dynamo::Graphics::ShaderDescriptor skybox_fragment_shader_descriptor;
    skybox_fragment_shader_descriptor.name = "Cubemap Fragment";
    skybox_fragment_shader_descriptor.code = CUBEMAP_FRAGMENT_SHADER;
    skybox_fragment_shader_descriptor.stage = Dynamo::Graphics::ShaderStage::Fragment;
    Dynamo::Graphics::Shader skybox_fragment = app.renderer().build_shader(skybox_fragment_shader_descriptor);

    // Build model materials
    Dynamo::Graphics::MaterialDescriptor material_descriptor;
    material_descriptor.vertex = vertex;
    material_descriptor.fragment = fragment;
    material_descriptor.topology = Dynamo::Graphics::Topology::Triangle;
    material_descriptor.fill = Dynamo::Graphics::Fill::Face;
    material_descriptor.cull = Dynamo::Graphics::Cull::None;
    Dynamo::Graphics::Material material0 = app.renderer().build_material(material_descriptor);
    Dynamo::Graphics::Material material1 = app.renderer().build_material(material_descriptor);

    // Build cubemap material
    Dynamo::Graphics::MaterialDescriptor skybox_material_descriptor;
    skybox_material_descriptor.vertex = skybox_vertex;
    skybox_material_descriptor.fragment = skybox_fragment;
    skybox_material_descriptor.topology = Dynamo::Graphics::Topology::Triangle;
    skybox_material_descriptor.fill = Dynamo::Graphics::Fill::Face;
    skybox_material_descriptor.cull = Dynamo::Graphics::Cull::None;
    Dynamo::Graphics::Material skybox_material = app.renderer().build_material(skybox_material_descriptor);

    // Shared uniforms are constant across all materials, so changing either uniform will write to the same memory.

    Dynamo::Graphics::Uniform transform_uniform0 = app.renderer().get_uniform(material0, "transform").value();
    Dynamo::Graphics::Uniform transform_uniform1 = app.renderer().get_uniform(material1, "transform").value();

    Dynamo::Graphics::Uniform time_uniform0 = app.renderer().get_uniform(material0, "timer").value();
    Dynamo::Graphics::Uniform time_uniform1 = app.renderer().get_uniform(material1, "timer").value();

    Dynamo::Graphics::Uniform sampler_uniform0 = app.renderer().get_uniform(material0, "tex_sampler").value();
    Dynamo::Graphics::Uniform sampler_uniform1 = app.renderer().get_uniform(material1, "tex_sampler").value();

    Dynamo::Graphics::Uniform texture_index0 = app.renderer().get_uniform(material0, "utexture").value();
    Dynamo::Graphics::Uniform texture_index1 = app.renderer().get_uniform(material1, "utexture").value();

    Dynamo::Graphics::Uniform skytransform_uniform = app.renderer().get_uniform(skybox_material, "transform").value();
    Dynamo::Graphics::Uniform cubemap_uniform = app.renderer().get_uniform(skybox_material, "cubemap").value();

    // Build texture
    Dynamo::Graphics::TextureDescriptor texture_descriptor;

    int w, h, channels;
    unsigned char *buffer = stbi_load("../assets/models/viking_room.png", &w, &h, &channels, 4);
    texture_descriptor.texels.resize(w * h * 4);
    texture_descriptor.width = w;
    texture_descriptor.height = h;
    std::memcpy(texture_descriptor.texels.data(), buffer, w * h * 4);
    stbi_image_free(buffer);

    Dynamo::Graphics::Texture texture0 = app.renderer().build_texture(texture_descriptor);
    app.renderer().bind_texture(sampler_uniform0, texture0, 0);
    app.renderer().bind_texture(sampler_uniform1, texture0, 0);

    std::array<Dynamo::Color, 4> colors = {
        Dynamo::Color(1, 0, 0), // TL
        Dynamo::Color(0, 1, 0), // TR
        Dynamo::Color(0, 0, 1), // BL
        Dynamo::Color(1, 1, 1), // BR
    };
    texture_descriptor.texels.clear();
    for (const Dynamo::Color &color : colors) {
        texture_descriptor.texels.push_back(255 * color.r);
        texture_descriptor.texels.push_back(255 * color.g);
        texture_descriptor.texels.push_back(255 * color.b);
        texture_descriptor.texels.push_back(255 * color.a);
    }
    texture_descriptor.width = 2;
    texture_descriptor.height = 2;

    Dynamo::Graphics::Texture texture1 = app.renderer().build_texture(texture_descriptor);
    app.renderer().bind_texture(sampler_uniform0, texture1, 1);
    app.renderer().bind_texture(sampler_uniform1, texture1, 1);

    std::array<Dynamo::Color, 6> cubemap_colors = {
        Dynamo::Color(1, 0, 0),
        Dynamo::Color(0, 1, 0),
        Dynamo::Color(0, 0, 1),
        Dynamo::Color(1, 1, 0),
        Dynamo::Color(0, 1, 1),
        Dynamo::Color(1, 0, 1),
    };
    texture_descriptor.texels.clear();
    for (const Dynamo::Color &color : cubemap_colors) {
        texture_descriptor.texels.push_back(255 * color.r);
        texture_descriptor.texels.push_back(255 * color.g);
        texture_descriptor.texels.push_back(255 * color.b);
        texture_descriptor.texels.push_back(255 * color.a);
    }
    texture_descriptor.width = 1;
    texture_descriptor.height = 1;
    // texture_descriptor.usage = Dynamo::Graphics::TextureUsage::Cubemap;
    // Dynamo::Graphics::Texture cubemap_texture = app.renderer().build_texture(texture_descriptor);
    // app.renderer().bind_texture(cubemap_uniform, cubemap_texture, 0);

    // Build the models
    Dynamo::Graphics::Model model0;
    model0.mesh = model_mesh;
    model0.material = material0;

    Dynamo::Graphics::Model model1;
    model1.mesh = flat_mesh;
    model1.material = material1;

    Dynamo::Graphics::Model skybox;
    skybox.mesh = skybox_mesh;
    skybox.material = skybox_material;

    // Sound source
    Dynamo::Sound::Source source(music, sequence);
    source.set_on_finish([&]() {
        source.set_start(3s);
        source.set_duration(3s);
        source.seek(0s);
        app.jukebox().play(source);
    });

    Dynamo::Color a(0, 0, 0);
    Dynamo::Color b(0, 0, 0);

    float dt_ave = 0;
    bool projection_toggle = true;

    Dynamo::Vec3 camera_position(2, 2, 2);
    Dynamo::Vec3 camera_direction = Dynamo::Vec3(-2, -2, -2).normalize();
    Dynamo::Vec3 camera_up(0, 0, 1);

    float yaw = -135;
    float pitch = -30;

    Dynamo::Vec2 prev_mouse = app.display().get_window_size() / 2;
    while (app.is_running()) {
        if (app.input().is_pressed(Dynamo::KeyCode::Space)) {
            app.jukebox().play(source);
        }
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

        // Set the listener position to the camera position
        listener.position = camera_position;

        // Set the source position to the position of the model
        source.position = {0, 0, 0};

        Dynamo::Vec2 mouse_position = app.input().get_mouse_position();
        float xoffset = (mouse_position.x - prev_mouse.x) * 0.1;
        float yoffset = (mouse_position.y - prev_mouse.y) * 0.1;
        prev_mouse = mouse_position;
        yaw -= xoffset;
        pitch -= yoffset;

        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        camera_direction.x = cos(Dynamo::to_radians(yaw)) * cos(Dynamo::to_radians(pitch));
        camera_direction.z = sin(Dynamo::to_radians(pitch));
        camera_direction.y = sin(Dynamo::to_radians(yaw)) * cos(Dynamo::to_radians(pitch));
        camera_direction.normalize();

        unsigned long long frames = app.clock().frames();
        unsigned update_rate = 32;
        dt_ave += app.clock().delta().count();
        if (frames % update_rate == 0) {
            dt_ave = dt_ave / update_rate;
            unsigned fps = 1.0 / dt_ave;
            app.display().set_title(std::to_string(fps) + " fps");
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

        Dynamo::Camera camera;
        if (projection_toggle) {
            camera.make(perspective).orient(camera_position, camera_direction, camera_up);
        } else {
            camera.make(orthographic).orient(camera_position, camera_direction, camera_up);
        }

        // Update uniforms
        Timer timer;
        Transform transform;
        Texture texture;

        timer.time = Dynamo::to_radians(frames);
        transform.mvp =
            camera.projection * camera.view * Dynamo::Mat4({0, 0, 0}, Dynamo::Quaternion({0, 0, 1}, 0), {1, 1, 1});
        texture.index = 0;
        app.renderer().write_uniform(transform_uniform0, &transform);
        app.renderer().write_uniform(time_uniform0, &timer);
        app.renderer().write_uniform(texture_index0, &texture);

        timer.time = -timer.time;
        transform.mvp = camera.projection * camera.view *
                        Dynamo::Mat4({0, 0, -0.5}, Dynamo::Quaternion({0, 0, 1}, timer.time), {1, 1, 1});
        texture.index = 1;
        app.renderer().write_uniform(transform_uniform1, &transform);
        app.renderer().write_uniform(time_uniform1, &timer);
        app.renderer().write_uniform(texture_index1, &texture);

        SkyboxTransform skybox_transform;
        Dynamo::Mat4 skybox_view = camera.view;
        skybox_view.values[12] = 0;
        skybox_view.values[13] = 0;
        skybox_view.values[14] = 0;
        skybox_view.values[15] = 1;

        skybox_transform.vp = camera.projection * skybox_view;
        app.renderer().write_uniform(skytransform_uniform, &skybox_transform);

        // Draw the model
        app.renderer().draw(model0);
        app.renderer().draw(model1);
        // app.renderer().draw(skybox);

        float x_t = mouse_position.x / display_size.x;
        app.renderer().set_clear(a.lerp(b, x_t));

        app.update();
    }
    return 0;
}
