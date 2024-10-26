#pragma once

#include <chrono>

using namespace std::chrono_literals;

namespace Dynamo {

    /**
     * @brief Standard unit of time.
     *
     */
    using Seconds = std::chrono::duration<float>;

    /**
     * @brief Time point.
     *
     */
    using TimePoint = std::chrono::system_clock::time_point;

    /**
     * @brief Application time-keeper.
     *
     */
    class Clock {
        Seconds _start;
        Seconds _delta;
        Seconds _prev;

        unsigned long long _frames;

      public:
        Clock();

        /**
         * @brief Get the current system time.
         *
         * @return TimePoint
         */
        static TimePoint time();

        /**
         * @brief Get the total elapsed time since initialization.
         *
         * @return Seconds
         */
        Seconds elapsed() const;

        /**
         * @brief Get the delta time since the previous frame.
         *
         * @return Seconds
         */
        Seconds delta() const;

        /**
         * @brief Get the total number of frames since initialization.
         *
         * @return unsigned long long
         */
        unsigned long long frames() const;

        /**
         * @brief Tick the clock.
         *
         * @return Seconds
         */
        void tick();
    };
} // namespace Dynamo