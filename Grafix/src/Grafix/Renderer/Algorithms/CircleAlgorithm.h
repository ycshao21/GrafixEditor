#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class CircleAlgorithm final : public GraphicsAlgorithm
    {
    public:
        static void Midpoint(glm::vec2 center, float radius, const glm::vec3& color);
    private:
        static void SetCirclePixels(int centerX, int centerY, int x, int y, uint32_t colorValue);
    };
}
