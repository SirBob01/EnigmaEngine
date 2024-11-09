#include <UI/Context.hpp>

namespace Dynamo::UI {
    Context::Context(Graphics::Renderer &renderer, Input &input) : _renderer(renderer), _input(input) {}
} // namespace Dynamo::UI