#pragma once

#include <glm/glm.hpp>

#include "Grafix/Scene/Components.h"
#include "Grafix/Utils/ColorConvert.hpp"
#include "Grafix/Math/Math.h"

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
        static int GetCanvasWidth();
        static int GetCanvasHeight();

        static bool IsInCanvas(int x, int y);

        static void SetPoint(int x, int y, uint32_t color);
        static void SetPixel(int x, int y, uint32_t color);
        static void DrawSquare(int centerX, int centerY, int length, uint32_t color);

        static uint32_t GetPixel(int x, int y);
    };
}
