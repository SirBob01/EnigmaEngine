#include <Asset/Font.hpp>
#include <Math/Vec2.hpp>
#include <Utils/Log.hpp>

namespace Dynamo::Asset {
    static FT_Library library;
    static bool font_ready = false;

    void init_fonts() {
        FT_Error error = FT_Init_FreeType(&library);
        if (error) {
            Log::error("Could not initialize freetype library.");
        }
        font_ready = true;
    }

    Vec2 scale_point(FT_Vector &vector, float em_scale) {
        Vec2 point(vector.x, vector.y);
        return point / em_scale;
    }

    void build_contour(Font &font, FT_Outline &outline, unsigned i0, unsigned i1, float em_scale) {
        if (i0 == i1) return;

        // Reverse the direction of the contour if necessary
        unsigned direction = 1;
        if (outline.flags & FT_OUTLINE_REVERSE_FILL) {
            std::swap(i0, i1);
            direction = -1;
        }

        // Find first point in contour and remove it from processing
        Vec2 first;
        if (outline.tags[i0] & FT_CURVE_TAG_ON) {
            first = scale_point(outline.points[i0], em_scale);
            i0 += direction;
        } else if (outline.tags[i1] & FT_CURVE_TAG_ON) {
            first = scale_point(outline.points[i1], em_scale);
            i1 -= direction;
        } else {
            Vec2 p0 = scale_point(outline.points[i0], em_scale);
            Vec2 p1 = scale_point(outline.points[i1], em_scale);
            first = (p0 + p1) * 0.5;
        }

        Vec2 start = first;
        Vec2 control = first;
        Vec2 previous = first;
        unsigned char prev_tag = FT_CURVE_TAG_ON;
        for (unsigned v = i0; v != i1 + direction; v += direction) {
            Vec2 current = scale_point(outline.points[v], em_scale);
            unsigned char tag = FT_CURVE_TAG(outline.tags[v]);
            if (tag == FT_CURVE_TAG_CUBIC) {
                control = previous;
            } else if (tag == FT_CURVE_TAG_ON) {
                if (prev_tag == FT_CURVE_TAG_CUBIC) {
                    Vec2 &b0 = start;
                    Vec2 &b1 = control;
                    Vec2 &b2 = previous;
                    Vec2 &b3 = current;

                    Vec2 c0 = b0 + (b1 - b0) * 0.75;
                    Vec2 c1 = b3 + (b2 - b3) * 0.75;

                    Vec2 d = (c0 + c1) * 0.5;

                    font.contours.push_back({b0, c0, d});
                    font.contours.push_back({d, c1, b3});
                } else if (prev_tag == FT_CURVE_TAG_ON) {
                    Vec2 midpoint = (previous + current) * 0.5;
                    font.contours.push_back({previous, midpoint, current});
                } else {
                    font.contours.push_back({start, previous, current});
                }
                start = current;
                control = current;
            } else {
                if (prev_tag == FT_CURVE_TAG_ON) {
                    // no-op, wait for next point
                } else {
                    Vec2 midpoint = (previous + current) * 0.5;
                    font.contours.push_back({start, previous, midpoint});
                    start = midpoint;
                    control = midpoint;
                }
            }
            previous = current;
            prev_tag = tag;
        }

        // Close the contour
        if (prev_tag == FT_CURVE_TAG_CUBIC) {
            Vec2 &b0 = start;
            Vec2 &b1 = control;
            Vec2 &b2 = previous;
            Vec2 &b3 = first;

            Vec2 c0 = b0 + (b1 - b0) * 0.75;
            Vec2 c1 = b3 + (b2 - b3) * 0.75;

            Vec2 d = (c0 + c1) * 0.5;

            font.contours.push_back({b0, c0, d});
            font.contours.push_back({d, c1, b3});
        } else if (prev_tag == FT_CURVE_TAG_ON) {
            Vec2 midpoint = (previous + first) * 0.5;
            font.contours.push_back({previous, midpoint, first});
        } else {
            font.contours.push_back({start, previous, first});
        }
    }

    void build_font_glyph(Font &font, FT_Face &face, unsigned charcode, unsigned glyph_index) {
        FT_Outline &outline = face->glyph->outline;
        unsigned contour_index = font.contours.size();
        unsigned start = 0;
        for (unsigned i = 0; i < outline.n_contours; i++) {
            unsigned short end = outline.contours[i];
            build_contour(font, outline, start, end, face->units_per_EM);
            start = face->glyph->outline.contours[i] + 1;
        }
        unsigned contour_count = font.contours.size() - contour_index;
        font.glyph_buffer.push_back({contour_index, contour_count});

        Glyph glyph;
        glyph.index = glyph_index;
        glyph.buffer_index = font.glyph_buffer.size() - 1;
        glyph.contour_count = contour_count;
        glyph.width = face->glyph->metrics.width;
        glyph.height = face->glyph->metrics.height;
        glyph.x_bearing = face->glyph->metrics.horiBearingX;
        glyph.y_bearing = face->glyph->metrics.horiBearingY;
        glyph.advance = face->glyph->metrics.horiAdvance;

        font.glyphs.emplace(glyph_index, glyph);
    }

    Font load_font(const std::string filepath) {
        if (!font_ready) {
            init_fonts();
        }

        Font font;
        FT_Error error = FT_New_Face(library, filepath.c_str(), 0, &font.face);
        if (error == FT_Err_Unknown_File_Format) {
            Log::error("Font format unsupported: {}", filepath);
        } else if (error) {
            Log::error("Could not load font: {}", filepath);
        }

        // Null glyph
        int flags = FT_LOAD_NO_SCALE | FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
        error = FT_Load_Glyph(font.face, 0, flags);
        if (error) {
            Log::info("Font succesfully built null glyph.");
        }
        build_font_glyph(font, font.face, 0, 0);

        // ASCII glyphs
        for (unsigned c = 32; c < 128; c++) {
            unsigned glyph_index = FT_Get_Char_Index(font.face, c);
            error = FT_Load_Glyph(font.face, glyph_index, flags);
            build_font_glyph(font, font.face, c, glyph_index);
        }

        return font;
    }
} // namespace Dynamo::Asset