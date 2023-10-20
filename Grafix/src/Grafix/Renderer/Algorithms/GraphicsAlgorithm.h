#pragma once

#include <glm/glm.hpp>

#include "Grafix/Scene/Components.h"
#include "Grafix/Utils/ColorConvert.hpp"

namespace Grafix
{
    class GraphicsAlgorithm
    {
    public:
        static void UpdatePixelData(uint32_t* pixelData, int* idMap, uint32_t width, uint32_t height);

        static void SetLineProperties(float lineWidth, LineStyleType lineStyle);
        static void SetID(int id);

        static void DrawSquare(glm::vec2 center, float length, const glm::vec3& color);
        static void UpdateLinePatternIndex();
    protected:
        static uint32_t GetWidth();
        static uint32_t GetHeight();

        static void SetPixel(int x, int y, uint32_t color);
        static void SetSinglePixel(int x, int y, uint32_t color);

        static uint32_t GetPixelValue(int x, int y);
    };
}
