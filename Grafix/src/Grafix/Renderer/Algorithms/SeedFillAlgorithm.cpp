#include "pch.h"
#include "SeedFillAlgorithm.h"

namespace Grafix
{
    void SeedFillAlgorithm::FloodFill(glm::vec2 point, uint32_t fillColor, uint32_t oldColor)
    {
        if (fillColor == oldColor)
            return;

        glm::ivec2 roundedPoint{ Math::Round(point.x), Math::Round(point.y) };
        if (!IsInCanvas(roundedPoint.x, roundedPoint.y))
            return;

        SetPixel(roundedPoint.x, roundedPoint.y, fillColor);

        // BFS
        std::queue<glm::ivec2> pixelQueue;
        pixelQueue.push(roundedPoint);

        glm::ivec2 offset[4] = { { 1, 0 }, { 0, 1 }, { -1, 0 }, { 0, -1 } };

        while (!pixelQueue.empty())
        {
            glm::ivec2 current = pixelQueue.front();
            pixelQueue.pop();

            for (int i = 0; i < 4; ++i)
            {
                glm::ivec2 next = current + offset[i];
                if (IsInCanvas(next.x, next.y) && GetPixel(next.x, next.y) == oldColor)
                {
                    SetPixel(next.x, next.y, fillColor);
                    pixelQueue.push(std::move(next));
                }
            }
        }
    }
}