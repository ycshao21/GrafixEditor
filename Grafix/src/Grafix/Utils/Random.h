#pragma once

namespace Grafix
{
    class Random
    {
    public:
        static void Init();

        // Generate a random uint32_t
        static uint32_t GenerateUint32();

        // Generate a random uint32_t between min and max
        static uint32_t GenerateUint32(uint32_t min, uint32_t max);

        // Generate a random float between 0 and 1
        static float GenerateFloat();

        // Generate a random float between min and max
        static float GenerateFloat(float min, float max);
    };
}
