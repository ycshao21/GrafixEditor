#include "pch.h"
#include "LineAlgorithm.h"

namespace Grafix
{
    void LineAlgorithm::Midpoint(glm::vec2 p0, glm::vec2 p1, const glm::vec3& color)
    {
        uint32_t colorValue = RGBToUint32(color);

        int x0 = (int)p0.x, y0 = (int)p0.y;
        int x1 = (int)p1.x, y1 = (int)p1.y;

        // If |k| > 1, swap x and y.
        bool XYSwapped = std::abs(y1 - y0) > std::abs(x1 - x0);
        if (XYSwapped)
        {
            XYSwapped = true;
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

        int x = x0, y = y0;
        int count = 0;

        while (x <= x1)
        {
            if (XYSwapped)
                SetPixel(y, x, colorValue);
            else
                SetPixel(x, y, colorValue);

            UpdateLinePatternIndex();

            if (d < 0)
                y += sy, d += d2;
            else
                d += d1;

            ++x;
            ++count;
        }
    }

    void LineAlgorithm::Bresenham(glm::vec2 p0, glm::vec2 p1, const glm::vec3& color)
    {
        uint32_t colorValue = RGBToUint32(color);

        int x0 = (int)p0.x, y0 = (int)p0.y;
        int x1 = (int)p1.x, y1 = (int)p1.y;

        // If |k| > 1, swap x and y.
        bool XYSwapped = std::abs(y1 - y0) > std::abs(x1 - x0);
        if (XYSwapped)
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

        int dx = (int)(x1 - x0), dy = (int)(y1 - y0);
        int e = -dx;
        int sy = dy > 0 ? 1 : -1;
        dy = std::abs(dy);

        int x = x0, y = y0;

        for (int i = 0; i <= dx; i++)
        {
            if (XYSwapped)
                SetPixel(y, x, colorValue);
            else
                SetPixel(x, y, colorValue);

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