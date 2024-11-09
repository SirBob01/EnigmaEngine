#pragma once

#include <string>

namespace Dynamo::Asset {
    struct Font {};

    /**
     * @brief Load font data
     *
     * @param filepath
     * @return Font
     */
    Font load_font(const std::string filepath);
} // namespace Dynamo::Asset