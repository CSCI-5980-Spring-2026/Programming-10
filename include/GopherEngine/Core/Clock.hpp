#pragma once

#include <chrono>

namespace GopherEngine
{
    class Clock
    {
        public:
            Clock();
            ~Clock();
            void reset();
            float delta_time();
            float get_elapsed_time() const;

        private:
            std::chrono::time_point<std::chrono::high_resolution_clock> start_ticks;
            std::chrono::time_point<std::chrono::high_resolution_clock> last_ticks;
    };
}