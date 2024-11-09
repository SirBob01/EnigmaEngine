#pragma once

#include <Graphics/Renderer.hpp>

namespace Dynamo::UI {
    /**
     * @brief GUI Context.
     *
     */
    class Context {
        Graphics::Renderer &_renderer;
        Input &_input;

      public:
        Context(Graphics::Renderer &renderer, Input &input);
    };
} // namespace Dynamo::UI