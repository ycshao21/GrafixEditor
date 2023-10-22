#include "pch.h"
#include "CircleAlgorithm.h"

namespace Grafix
{
    void CircleAlgorithm::Midpoint(glm::vec2 center, float radius, const glm::vec3& color)
    {
        // Preprocess the values
        const int centerX = Math::Round(center.x), centerY = Math::Round(center.y);
        const int r = Math::Round(radius);
        uint32_t colorValue = RGBToUint32(color);

        int x = 0, y = r;
        int e = 1 - r;
        while (x <= y)
        {
            SetCirclePixels(centerX, centerY, x, y, colorValue);

            if (e < 0)
            {
                e += 2 * x + 3;
            }
            else
            {
                e += 2 * (x - y) + 5;
                --y;
            }
            ++x;
        }
    }

    void CircleAlgorithm::SetCirclePixels(int centerX, int centerY, int x, int y, uint32_t colorValue)
    {
        SetPoint(centerX + x, centerY + y, colorValue);
        SetPoint(centerX + y, centerY + x, colorValue);

        SetPoint(centerX - x, centerY + y, colorValue);
        SetPoint(centerX - y, centerY + x, colorValue);

        SetPoint(centerX - x, centerY - y, colorValue);
        SetPoint(centerX - y, centerY - x, colorValue);

        SetPoint(centerX + x, centerY - y, colorValue);
        SetPoint(centerX + y, centerY - x, colorValue);

        UpdateLinePatternIndex();
    }
}