#pragma once

#pragma once
#include <random>

namespace GopherEngine
{
    class Random
    {
        public:
            static float value() { return float_dist_(rng_); }
            static int range(int min, int max) { return std::uniform_int_distribution<int>(min, max)(rng_); } 

        private:
            inline static std::mt19937 rng_{ std::random_device{}() };
            inline static std::uniform_real_distribution<float> float_dist_{ 0.f, 1.f };
    };
}