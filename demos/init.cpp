#include <Dynamo.hpp>

int main() {
    Dynamo::Application app({
        .title = "Hello, world",
        .window_width = 640,
        .window_height = 480,
        .root_asset_directory = "../assets/",
    });
    while (app.is_running()) {
        app.update();
    }
    return 0;
}
