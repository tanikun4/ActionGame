#pragma once
#include <random>

template <typename T>
inline static T RandomRange(T min, T max)
{
    static_assert(std::is_arithmetic_v<T>,
        "Random::Range ‚Í”’lŒ^‚Ì‚İ‘Î‰");

    static std::mt19937 rng{ std::random_device{}() };

    if constexpr (std::is_integral_v<T>)
    {
        std::uniform_int_distribution<T> dist(min, max);
        return dist(rng);
    }
    else
    {
        std::uniform_real_distribution<T> dist(min, max);
        return dist(rng);
    }
}