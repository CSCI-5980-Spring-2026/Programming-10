#include <GopherEngine/Core/Clock.hpp>

using namespace std::chrono;

namespace GopherEngine {
    Clock::Clock()
    {
        reset();
    }

    Clock::~Clock()
    {}

    void Clock::reset()
    {
        start_ticks = high_resolution_clock::now();
        last_ticks = start_ticks;
    }

    float Clock::delta_time()
    {
        auto current_ticks = high_resolution_clock::now();
        auto delta_time = duration_cast<duration<float>>(current_ticks - last_ticks).count();
        last_ticks = current_ticks;
        return delta_time;
    }

    float Clock::get_elapsed_time() const
    {
        auto current_ticks = high_resolution_clock::now();
        return duration_cast<duration<float>>(current_ticks - start_ticks).count();
    }  
}