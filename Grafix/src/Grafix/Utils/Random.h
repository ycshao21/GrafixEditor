#pragma once

namespace Grafix
{
    class Random
    {
    public:
        static void Init();

        static float GenerateFloat();
        static float GenerateFloat(float min, float max);
    };
}
