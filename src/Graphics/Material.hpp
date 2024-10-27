#pragma once

#include <Graphics/Shader.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief Topology modes.
     *
     */
    enum class Topology {
        Point,
        Triangle,
        Line,
    };

    /**
     * @brief Fill modes.
     *
     */
    enum class Fill {
        Point,
        Line,
        Face,
    };

    /**
     * @brief Cull modes.
     *
     */
    enum class Cull {
        None,
        Back,
        Front,
    };

    /**
     * @brief Material descriptor.
     *
     */
    struct Material {
        /**
         * @brief Vertex shader handle.
         *
         */
        Shader vertex;

        /**
         * @brief Fragment shader handle.
         *
         */
        Shader fragment;

        /**
         * @brief Topology of the vertex input.
         *
         */
        Topology topology = Topology::Triangle;

        /**
         * @brief Fill mode on draw.
         *
         */
        Fill fill = Fill::Face;

        /**
         * @brief Cull mode.
         *
         */
        Cull cull = Cull::Back;

        /**
         * @brief Equality operator.
         *
         * @param other
         * @return true
         * @return false
         */
        inline bool operator==(const Material &other) const {
            return vertex == other.vertex && fragment == other.fragment && topology == other.topology &&
                   fill == other.fill && cull == other.cull;
        }
    };
} // namespace Dynamo::Graphics

/**
 * @brief Hash function implementation for Graphics Material.
 *
 * @tparam
 */
template <>
struct std::hash<Dynamo::Graphics::Material> {
    inline size_t operator()(const Dynamo::Graphics::Material &material) const {
        size_t hash0 = std::hash<Dynamo::Graphics::Shader>{}(material.vertex);
        size_t hash1 = std::hash<Dynamo::Graphics::Shader>{}(material.fragment);
        size_t hash2 = std::hash<unsigned>{}(static_cast<unsigned>(material.topology));
        size_t hash3 = std::hash<unsigned>{}(static_cast<unsigned>(material.fill));
        size_t hash4 = std::hash<unsigned>{}(static_cast<unsigned>(material.cull));
        return hash0 ^ (hash1 << 1) ^ (hash2 << 2) ^ (hash3 << 3) ^ (hash4 << 4);
    }
};
