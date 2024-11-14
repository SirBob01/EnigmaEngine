#pragma once

#include <Graphics/Mesh.hpp>
#include <Math/Color.hpp>
#include <Math/Vec2.hpp>
#include <Math/Vec3.hpp>

namespace Dynamo::Asset {
    /**
     * @brief Obj model asset.
     *
     */
    struct Obj {
        /**
         * @brief Material descriptor.
         *
         */
        struct Material {
            Color ambient_color;
            std::string ambient_filepath;

            Color diffuse_color;
            std::string diffuse_filepath;

            Color specular_color;
            std::string specular_filepath;

            std::string bump_filepath;
        };

        /**
         * @brief Mesh group by material.
         *
         */
        struct MeshGroup {
            Material material;
            std::vector<Vec3> positions;
            std::vector<Vec3> normals;
            std::vector<Vec2> uvs;
            std::vector<Color> colors;
            std::vector<unsigned> indices;
        };

        /**
         * @brief Standard Obj vertex format
         *
         */
        struct Vertex {
            Vec3 position;
            Vec3 normal;
            Vec2 uv;
            Color color;

            bool operator==(const Vertex &other) const {
                return position == other.position && normal == other.normal && uv == other.uv && color == other.color;
            }
        };

        /**
         * @brief Meshes grouped by material.
         *
         */
        std::vector<MeshGroup> groups;

        /**
         * @brief Load an Obj model file.
         *
         * @param filepath
         */
        Obj(const std::string filepath);
    };
} // namespace Dynamo::Asset

template <>
struct std::hash<Dynamo::Asset::Obj::Vertex> {
    inline size_t operator()(const Dynamo::Asset::Obj::Vertex &vertex) const {
        size_t hash0 = std::hash<Dynamo::Vec3>{}(vertex.position);
        size_t hash1 = std::hash<Dynamo::Vec3>{}(vertex.normal);
        size_t hash2 = std::hash<Dynamo::Vec2>{}(vertex.uv);
        size_t hash3 = std::hash<Dynamo::Color>{}(vertex.color);
        return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3);
    }
};