#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class LineAlgorithm final : public GraphicsAlgorithm
    {
    public:
        static void Midpoint(glm::vec2 p0, glm::vec2 p1, const glm::vec3& color);
        static void Bresenham(glm::vec2 p0, glm::vec2 p1, const glm::vec3& color);
    };
}
