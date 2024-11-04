#include <Application.hpp>

namespace Dynamo {
    Application::Application(const ApplicationSettings &settings) :
        _display(settings.title, settings.window_width, settings.window_height),
        _renderer(_display, settings.root_asset_directory) {
        // Run audio on a separate thread
        _audio_thread = std::thread([&]() {
            while (is_running()) {
                _jukebox.update();
            }
        });
        _running = true;
    }

    Application::~Application() {
        if (_audio_thread.joinable()) {
            _audio_thread.join();
        }
    }

    bool Application::is_running() const { return _display.is_open() && _running; }

    Display &Application::display() { return _display; }

    Input &Application::input() { return _display.input(); }

    Clock &Application::clock() { return _clock; }

    Graphics::Renderer &Application::renderer() { return _renderer; }

    Sound::Jukebox &Application::jukebox() { return _jukebox; }

    void Application::quit() { _running = false; }

    void Application::update() {
        // Poll for input
        _display.input().poll();
        _renderer.render();

        // Tick
        _clock.tick();
    }
} // namespace Dynamo