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
    enum class TextureFormat : uint8_t {
        R16_SFLOAT,
        R32_SFLOAT,
        R8G8B8_UNORM,
        R8G8B8A8_UNORM,
        R8G8B8A8_SRGB,
        ColorSurface,
        DepthSurface,
    };

    /**
     * @brief Texture usage.
     *
     */
    enum class TextureUsage : uint8_t {
        Static,
        Cubemap,
        ColorTarget,
        DepthStencilTarget,
    };

    /**
     * @brief Texture filter modes.
     *
     */
    enum class TextureFilter : uint8_t {
        Nearest,
        Linear,
    };

    /**
     * @brief Texture addressing modes.
     *
     */
    enum class TextureAddressMode : uint8_t {
        Repeat,
        RepeatMirror,
        Clamp,
        ClampMirror,
        ClampBorder,
    };

    /**
     * @brief Texture samples.
     *
     */
    enum class TextureSamples : uint8_t {
        S1,
        S2,
        S4,
        S8,
        S16,
        S32,
        S64,
    };

    /**
     * @brief Texture descriptor.
     *
     */
    struct TextureDescriptor {
        /**
         * @brief Texture unit byte buffer.
         *
         * If this buffer is empty, the texture will be default initialized.
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
         * @brief Depth of the texture in texture units (defaults to 1, for 2D texture).
         *
         */
        unsigned depth = 1;

        /**
         * @brief Mipmap levels.
         *
         * The texel buffer must hold the LODs in contiguous memory.
         *
         * Dimension_n = max(Dimension_n-1 / 2, 1)
         *
         */
        unsigned mip_levels = 1;

        /**
         * @brief Format of the texture determines how the byte buffer is interpreted.
         *
         * This may be overriden depending on the selected TextureUsage.
         *
         */
        TextureFormat format = TextureFormat::R8G8B8A8_SRGB;

        /**
         * @brief Usage of the texture.
         *
         * If usage is Cubemap, the texel buffer must contain 6 sub-images of uniform dimensions.
         *
         */
        TextureUsage usage = TextureUsage::Static;

        /**
         * @brief Minification filter.
         *
         */
        TextureFilter min_filter = TextureFilter::Linear;

        /**
         * @brief Magnification filter.
         *
         */
        TextureFilter mag_filter = TextureFilter::Linear;

        /**
         * @brief Mipmap filter.
         *
         */
        TextureFilter mipmap_filter = TextureFilter::Linear;

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

        /**
         * @brief Desired sample count.
         *
         * The actual sample count will be clamped depending on device availability.
         *
         */
        TextureSamples samples = TextureSamples::S1;
    };
} // namespace Dynamo::Graphics