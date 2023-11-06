#pragma once

namespace Grafix
{
    class Random
    {
    public:
        static void Init();

        // Generate a random float between 0 and 1
        static float GenerateFloat();

        // Generate a random float between min and max
        static float GenerateFloat(float min, float max);
    };
}
