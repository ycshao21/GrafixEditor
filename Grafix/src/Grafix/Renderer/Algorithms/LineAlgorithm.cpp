#include "pch.h"
#include "LineAlgorithm.h"

namespace Grafix
{
    void LineAlgorithm::Midpoint(glm::vec2 p0, glm::vec2 p1, const glm::vec3& color)
    {
        // Preprocess the values
        int x0 = Math::Round(p0.x), y0 = Math::Round(p0.y);
        int x1 = Math::Round(p1.x), y1 = Math::Round(p1.y);
        uint32_t colorValue = RGBToUint32(color);

        // If |k| > 1, swap x and y.
        bool xySwapped = std::abs(y1 - y0) > std::abs(x1 - x0);
        if (xySwapped)
        {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }

        // If p0 is to the right of p1, swap them.
        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        int sy = y0 < y1 ? 1 : -1;
        int a = (y0 - y1) * sy;
        int b = x1 - x0;

        int d = (a << 1) + b;
        int d1 = a << 1, d2 = (a + b) << 1;

        int y = y0;
        for (int x = x0; x <= x1; ++x)
        {
            if (xySwapped)
                SetPoint(y, x, colorValue);
            else
                SetPoint(x, y, colorValue);

            UpdateLinePatternIndex();

            if (d < 0)
                y += sy, d += d2;
            else
                d += d1;
        }
    }

    void LineAlgorithm::Bresenham(glm::vec2 p0, glm::vec2 p1, const glm::vec3& color)
    {
        // Preprocess the values
        int x0 = Math::Round(p0.x), y0 = Math::Round(p0.y);
        int x1 = Math::Round(p1.x), y1 = Math::Round(p1.y);
        uint32_t colorValue = RGBToUint32(color);

        // If |k| > 1, swap x and y.
        bool xySwapped = std::abs(y1 - y0) > std::abs(x1 - x0);
        if (xySwapped)
        {
            std::swap(x0, y0);
            std::swap(x1, y1);
        }

        // If p0 is to the right of p1, swap them.
        if (x0 > x1)
        {
            std::swap(x0, x1);
            std::swap(y0, y1);
        }

        int dx = x1 - x0;
        int dy = y1 - y0;
        int e = -dx;

        int sy = dy > 0 ? 1 : -1;
        dy = std::abs(dy);

        int x = x0, y = y0;
        for (int i = 0; i <= dx; ++i)
        {
            if (xySwapped)
                SetPoint(y, x, colorValue);
            else
                SetPoint(x, y, colorValue);

            UpdateLinePatternIndex();

            ++x;
            e += dy << 1;

            if (e >= 0)
            {
                y += sy;
                e -= dx << 1;
            }
        }
    }
}