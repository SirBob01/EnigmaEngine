#include <Asset/Obj.hpp>
#include <rapidobj/rapidobj.hpp>

namespace Dynamo::Asset {
    Obj::Obj(const std::string filepath) {
        rapidobj::Result result = rapidobj::ParseFile(filepath);
        rapidobj::Triangulate(result);
        if (result.error) {
            Log::error("Obj: {} on line {} '{}'",
                       result.error.code.message(),
                       result.error.line_num,
                       result.error.line);
        }

        std::vector<std::unordered_map<Vertex, unsigned>> index_maps;

        // No-material group
        groups.push_back({});
        index_maps.push_back({});

        // Material groups
        for (const rapidobj::Material &material : result.materials) {
            MeshGroup group;
            group.material.ambient_color = Color(material.ambient[0], material.ambient[1], material.ambient[2]);
            group.material.ambient_filepath = material.ambient_texname;

            group.material.diffuse_color = Color(material.diffuse[0], material.diffuse[1], material.diffuse[2]);
            group.material.diffuse_filepath = material.diffuse_texname;

            group.material.specular_color = Color(material.specular[0], material.specular[1], material.specular[2]);
            group.material.specular_filepath = material.specular_texname;

            group.material.bump_filepath = material.bump_texname;

            groups.emplace_back(group);
            index_maps.push_back({});
        }

        for (const rapidobj::Shape &shape : result.shapes) {
            for (unsigned i = 0; i < shape.mesh.indices.size(); i += 3) {
                for (unsigned v = 0; v < 3; v++) {
                    Vertex vertex;
                    rapidobj::Index index = shape.mesh.indices[i + v];
                    if (index.position_index >= 0) {
                        vertex.position.x = result.attributes.positions[3 * index.position_index + 0];
                        vertex.position.y = result.attributes.positions[3 * index.position_index + 1];
                        vertex.position.z = result.attributes.positions[3 * index.position_index + 2];

                        // Optional vertex color, defaults to #FFFFFF if not available
                        if (result.attributes.colors.size()) {
                            vertex.color.r = result.attributes.colors[3 * index.position_index + 0];
                            vertex.color.g = result.attributes.colors[3 * index.position_index + 1];
                            vertex.color.b = result.attributes.colors[3 * index.position_index + 2];
                        } else {
                            vertex.color.r = 1.0f;
                            vertex.color.g = 1.0f;
                            vertex.color.b = 1.0f;
                        }
                    }
                    if (index.normal_index >= 0) {
                        vertex.normal.x = result.attributes.normals[3 * index.normal_index + 0];
                        vertex.normal.y = result.attributes.normals[3 * index.normal_index + 1];
                        vertex.normal.z = result.attributes.normals[3 * index.normal_index + 2];
                    }
                    if (index.texcoord_index >= 0) {
                        vertex.uv.x = result.attributes.texcoords[2 * index.texcoord_index + 0],
                        vertex.uv.y = 1 - result.attributes.texcoords[2 * index.texcoord_index + 1];
                    }

                    int material_id = shape.mesh.material_ids[i / 3];
                    MeshGroup &group = groups[material_id + 1];
                    auto &index_map = index_maps[material_id + 1];

                    if (!index_map.count(vertex)) {
                        index_map.emplace(vertex, group.positions.size());

                        // Update vertex arrays
                        group.positions.push_back(vertex.position);
                        group.normals.push_back(vertex.normal);
                        group.uvs.push_back(vertex.uv);
                        group.colors.push_back(vertex.color);
                    }
                    group.indices.push_back(index_map.at(vertex));
                }
            }
        }
    }
} // namespace Dynamo::Asset