#pragma once

#include <string>

#include <Graphics/Texture.hpp>

namespace Dynamo::Asset {
    /**
     * @brief Load a 2D texture from an image file.
     *
     * @param filepath
     * @param mip_levels
     * @return Graphics::TextureDescriptor
     */
    Graphics::TextureDescriptor load_texture(const std::string filepath, unsigned mip_levels = 1);

    /**
     * @brief Load a cubemap texture from image face files.
     *
     * The face images must all have the same size (e.g., 256 x 256).
     *
     * @param right
     * @param left
     * @param top
     * @param bottom
     * @param front
     * @param back
     * @return Graphics::TextureDescriptor
     */
    Graphics::TextureDescriptor load_texture_cubemap(const std::string right,
                                                     const std::string left,
                                                     const std::string top,
                                                     const std::string bottom,
                                                     const std::string front,
                                                     const std::string back);

    /**
     * @brief Generate texture mip levels.
     *
     * @param descriptor
     * @param mip_levels
     */
    void generate_texture_mipmap(Graphics::TextureDescriptor &descriptor, unsigned mip_levels);
} // namespace Dynamo::Asset