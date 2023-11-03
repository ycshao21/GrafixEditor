#pragma once

namespace Grafix
{
    class Random
    {
    public:
        static void Init();

        static int GenerateInt();
        static int GenerateInt(int min, int max);

        static float GenerateFloat();
        static float GenerateFloat(float min, float max);
    };
}
