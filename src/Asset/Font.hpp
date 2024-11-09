#pragma once

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <Math/Vec2.hpp>

namespace Dynamo::Asset {
    /**
     * @brief Bezier curve control points.
     *
     */
    struct Contour {
        Vec2 p0;
        Vec2 p1;
        Vec2 p2;
    };

    /**
     * @brief Buffer glyph object.
     *
     */
    struct BufferGlyph {
        uint32_t contour_index;
        uint32_t contour_count;
    };

    /**
     * @brief A glyph is a sequence of Bezier contours.
     *
     */
    struct Glyph {
        unsigned index;
        unsigned buffer_index;
        unsigned contour_count;
        long width;
        long height;
        long x_bearing;
        long y_bearing;
        long advance;
    };

    /**
     * @brief Font atlas.
     *
     */
    struct Font {
        FT_Face face;
        std::vector<Contour> contours;
        std::vector<BufferGlyph> glyph_buffer;
        std::unordered_map<unsigned, Glyph> glyphs;
    };

    /**
     * @brief Load font data
     *
     * @param filepath
     * @return Font
     */
    Font load_font(const std::string filepath);
} // namespace Dynamo::Asset