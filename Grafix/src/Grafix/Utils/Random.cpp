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

    int Random::GenerateInt()
    {
        return s_Distribution(s_RandomEngine);
    }

    int Random::GenerateInt(int min, int max)
    {
        return min + GenerateInt() % (max - min);
    }

    float Random::GenerateFloat()
    {
        return (float)GenerateInt() / (float)std::numeric_limits<uint32_t>::max();
    }

    float Random::GenerateFloat(float min, float max)
    {
        return min + GenerateFloat() * (max - min);
    }
}

