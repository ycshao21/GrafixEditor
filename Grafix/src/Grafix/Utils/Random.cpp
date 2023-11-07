#include "pch.h"

#include "Random.h"
#include <random>

namespace Grafix
{
    static std::mt19937 s_RandomEngine;
    static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;

    void Random::Init()
    {
        s_RandomEngine.seed(std::random_device()());
    }

    uint32_t Random::GenerateUint32()
    {
        return s_Distribution(s_RandomEngine);
    }

    uint32_t Random::GenerateUint32(uint32_t min, uint32_t max)
    {
        if(min == max)
            return min;

        if (min > max)
            std::swap(min, max);

        return min + s_Distribution(s_RandomEngine) % (max - min);
    }

    float Random::GenerateFloat()
    {
        return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<uint32_t>::max();
    }

    float Random::GenerateFloat(float min, float max)
    {
        return min + GenerateFloat() * (max - min);
    }
}

