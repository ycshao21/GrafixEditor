#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class SeedFillAlgorithm final : public GraphicsAlgorithm
    {
    public:
        static void FloodFill(glm::vec2 seedPoint, uint32_t fillColor, uint32_t oldColor);
    };
}
