#pragma once

#include <Sound/Buffer.hpp>

namespace Dynamo::Asset {
    /**
     * @brief Load a sound file.
     *
     * @param filepath
     * @return Sound::Buffer
     */
    Sound::Buffer load_sound(const std::string filepath);
} // namespace Dynamo::Asset