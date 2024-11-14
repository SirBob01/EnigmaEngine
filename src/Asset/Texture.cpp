#define STB_IMAGE_RESIZE_IMPLEMENTATION

#include <stb_image.h>
#include <stb_image_resize.h>

#include <Asset/Texture.hpp>

namespace Dynamo::Asset {
    Graphics::TextureDescriptor load_texture(const std::string filepath) {
        int w, h, channels;
        unsigned char *buffer = stbi_load(filepath.c_str(), &w, &h, &channels, 4);
        if (!buffer) {
            Dynamo::Log::error("Failed to load texture: {}", filepath);
        }

        Graphics::TextureDescriptor descriptor;
        descriptor.width = w;
        descriptor.height = h;
        descriptor.texels.resize(w * h * 4);

        std::memcpy(descriptor.texels.data(), buffer, w * h * 4);
        stbi_image_free(buffer);

        return descriptor;
    }

    Graphics::TextureDescriptor load_texture_cubemap(const std::string right,
                                                     const std::string left,
                                                     const std::string top,
                                                     const std::string bottom,
                                                     const std::string front,
                                                     const std::string back) {
        Graphics::TextureDescriptor descriptor;

        int w, h, channels;
        for (const std::string &filepath : {right, left, top, bottom, front, back}) {
            unsigned char *buffer = stbi_load(filepath.c_str(), &w, &h, &channels, 4);
            if (!buffer) {
                Dynamo::Log::error("Failed to load texture: {}", filepath);
            }

            unsigned buffer_offset = descriptor.texels.size();
            descriptor.texels.resize(buffer_offset + w * h * 4);
            std::memcpy(descriptor.texels.data() + buffer_offset, buffer, w * h * 4);

            stbi_image_free(buffer);
        }

        descriptor.width = w;
        descriptor.height = h;
        descriptor.mip_levels = 1;
        descriptor.usage = Dynamo::Graphics::TextureUsage::Cubemap;

        return descriptor;
    }

    void generate_texture_mipmap(Graphics::TextureDescriptor &descriptor, unsigned mip_levels) {
        unsigned lod_w = descriptor.width;
        unsigned lod_h = descriptor.height;

        descriptor.mip_levels = mip_levels;
        for (unsigned i = 1; i < descriptor.mip_levels; i++) {
            unsigned n = lod_w * lod_h * 4;
            unsigned dst_offset = descriptor.texels.size();
            unsigned src_offset = dst_offset - n;

            unsigned next_w = std::max(lod_w / 2, 1U);
            unsigned next_h = std::max(lod_h / 2, 1U);
            unsigned next_n = next_w * next_h * 4;

            descriptor.texels.resize(dst_offset + next_n);
            stbir_resize_uint8(descriptor.texels.data() + src_offset,
                               lod_w,
                               lod_h,
                               0,
                               descriptor.texels.data() + dst_offset,
                               next_w,
                               next_h,
                               0,
                               4);
            lod_w = next_w;
            lod_h = next_h;
        }
    }
} // namespace Dynamo::Asset