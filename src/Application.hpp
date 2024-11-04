#pragma once

#include <string>
#include <thread>

#include <Clock.hpp>
#include <Display.hpp>
#include <Graphics/Renderer.hpp>
#include <Sound/Jukebox.hpp>

namespace Dynamo {
    /**
     * @brief Application setup options.
     *
     */
    struct ApplicationSettings {
        /**
         * @brief Application title.
         *
         */
        std::string title;

        /**
         * @brief Width of the display window.
         *
         */
        unsigned window_width;

        /**
         * @brief Height of the display window.
         *
         */
        unsigned window_height;

        /**
         * @brief Root asset directory.
         *
         */
        std::string root_asset_directory;
    };

    /**
     * @brief Dynamo Application.
     *
     */
    class Application {
        Display _display;
        Clock _clock;
        Graphics::Renderer _renderer;
        Sound::Jukebox _jukebox;

        std::thread _audio_thread;

        bool _running;

      public:
        /**
         * @brief Initialize a new Application.
         *
         * @param settings
         */
        Application(const ApplicationSettings &settings);
        ~Application();

        /**
         * @brief Check if the application is running.
         *
         * @return true
         * @return false
         */
        bool is_running() const;

        /**
         * @brief Get the display.
         *
         * @return Display&
         */
        Display &display();

        /**
         * @brief Get the input handler.
         *
         * @return Input&
         */
        Input &input();

        /**
         * @brief Get the clock.
         *
         * @return Clock&
         */
        Clock &clock();

        /**
         * @brief Get the renderer.
         *
         * @return Graphics::Renderer&
         */
        Graphics::Renderer &renderer();

        /**
         * @brief Get the audio engine.
         *
         * @return Sound::Jukebox&
         */
        Sound::Jukebox &jukebox();

        /**
         * @brief Quit the application.
         *
         */
        void quit();

        /**
         * @brief Update the application state.
         *
         */
        void update();
    };
} // namespace Dynamo