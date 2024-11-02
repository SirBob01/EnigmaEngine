#pragma once

#include <vector>

#include <Utils/SparseArray.hpp>

namespace Dynamo::Graphics {
    /**
     * @brief Texture resource handle.
     *
     */
    DYN_DEFINE_ID_TYPE(Texture);

    /**
     * @brief Texture formats.
     *
     */
    enum class TextureFormat {
        F32_R_Norm,
        U8_RGB_Norm,
        U8_RGBA_Norm,
    };

    /**
     * @brief Texture filter modes.
     *
     */
    enum class TextureFilter {
        Nearest,
        Linear,
    };

    /**
     * @brief Texture addressing modes.
     *
     */
    enum class TextureAddressMode {
        Repeat,
        RepeatMirror,
        Clamp,
        ClampMirror,
        ClampBorder,
    };

    /**
     * @brief Texture descriptor.
     *
     */
    struct TextureDescriptor {
        /**
         * @brief Texture unit byte buffer.
         *
         */
        std::vector<unsigned char> texels;

        /**
         * @brief Width of the texture in texture units.
         *
         */
        unsigned width = 0;

        /**
         * @brief Height of the texture in texture units.
         *
         */
        unsigned height = 0;

        /**
         * @brief Format of the texture determines how the byte buffer is interpreted.
         *
         */
        TextureFormat format = TextureFormat::U8_RGBA_Norm;

        /**
         * @brief Minification filter.
         *
         */
        TextureFilter min_filter = TextureFilter::Nearest;

        /**
         * @brief Magnification filter.
         *
         */
        TextureFilter mag_filter = TextureFilter::Nearest;

        /**
         * @brief How U coordinates are addressed outside [0, 1).
         *
         */
        TextureAddressMode u_address_mode = TextureAddressMode::Repeat;

        /**
         * @brief How V coordinates are addressed outside [0, 1).
         *
         */
        TextureAddressMode v_address_mode = TextureAddressMode::Repeat;

        /**
         * @brief How W coordinates are addressed outside [0, 1).
         *
         */
        TextureAddressMode w_address_mode = TextureAddressMode::Repeat;
    };
} // namespace Dynamo::Graphics