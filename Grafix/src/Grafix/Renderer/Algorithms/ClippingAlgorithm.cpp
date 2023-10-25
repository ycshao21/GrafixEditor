#include "pch.h"

#include "ClippingAlgorithm.h"

namespace Grafix
{
    // ***************************************************************************************************************************************************************
    // Line Clipping
    // ***************************************************************************************************************************************************************

    static const int LEFT = 1;
    static const int RIGHT = 2;
    static const int BOTTOM = 4;
    static const int TOP = 8;

    std::vector<glm::vec2> ClippingAlgorithm::CohenSutherland(glm::vec2 lineP0, glm::vec2 lineP1, glm::vec2 bottomLeft, glm::vec2 topRight)
    {
        int code = 0;
        int code1 = Encode(lineP0, bottomLeft, topRight);
        int code2 = Encode(lineP1, bottomLeft, topRight);

        int x = 0, y = 0;

        while (code1 != 0 || code2 != 0)
        {
            if ((code1 & code2) != 0)
                return { { -1.0f, -1.0f }, { -1.0f, -1.0f } };

            code = code1 == 0 ? code2 : code1;

            if ((code & LEFT) != 0)
            {
                x = bottomLeft.x;
                y = lineP0.y + (lineP1.y - lineP0.y) * (x - lineP0.x) / (lineP1.x - lineP0.x);
            }
            else if ((code & RIGHT) != 0)
            {
                x = topRight.x;
                y = lineP0.y + (lineP1.y - lineP0.y) * (x - lineP0.x) / (lineP1.x - lineP0.x);
            }
            else if ((code & BOTTOM) != 0)
            {
                y = bottomLeft.y;
                x = lineP0.x + (lineP1.x - lineP0.x) * (y - lineP0.y) / (lineP1.y - lineP0.y);
            }
            else if ((code & TOP) != 0)
            {
                y = topRight.y;
                x = lineP0.x + (lineP1.x - lineP0.x) * (y - lineP0.y) / (lineP1.y - lineP0.y);
            }

            if (code == code1)
            {
                lineP0.x = x;
                lineP0.y = y;
                code1 = Encode(lineP0, bottomLeft, topRight);
            }
            else
            {
                lineP1.x = x;
                lineP1.y = y;
                code2 = Encode(lineP1, bottomLeft, topRight);
            }
        }
        return { lineP0, lineP1 };
    }

    std::vector<glm::vec2> ClippingAlgorithm::Midpoint(glm::vec2 lineP0, glm::vec2 lineP1, glm::vec2 bottomLeft, glm::vec2 topRight)
    {
        int code1 = Encode(lineP0, bottomLeft, topRight);
        int code2 = Encode(lineP1, bottomLeft, topRight);
        int code = 0, x = 0, y = 0;
        float epsilon = 0.1f;

        if (code1 == 0 && code2 == 0)
            return { lineP0, lineP1 };

        if ((code1 & code2) != 0)
            return { { -1.0f, -1.0f }, { -1.0f, -1.0f } };

        if (code1 != 0)
        {
            glm::vec2 begin = lineP0;
            glm::vec2 end = lineP1;

            while (glm::distance(begin, end) >= epsilon)
            {
                glm::vec2 mid = (begin + end) / 2.0f;
                code = Encode(mid, bottomLeft, topRight);
                if ((code & code1) == 0)
                {
                    end = mid;
                }
                else
                {
                    begin = mid;
                    code1 = code;
                }
            }
            lineP0 = begin;
        }

        if (code2 != 0)
        {
            glm::vec2 begin = lineP0;
            glm::vec2 end = lineP1;

            while (glm::distance(begin, end) >= epsilon)
            {
                glm::vec2 mid = glm::vec2({ (begin[0] + end[0]) / 2, (begin[1] + end[1]) / 2 });
                code = Encode(mid, bottomLeft, topRight);
                if ((code & code2) == 0)
                    begin = mid;
                else
                {
                    end = mid;
                    code2 = code;
                }
            }
            lineP1 = end;
        }

        return { lineP0, lineP1 };
    }

    int ClippingAlgorithm::Encode(glm::vec2 point, glm::vec2 bottomLeft, glm::vec2 topRight)
    {
        int code = 0;

        if (point.x < bottomLeft.x)
            code |= LEFT;
        else if (point.x > topRight.x)
            code |= RIGHT;
        if (point.y < bottomLeft.y)
            code |= BOTTOM;
        else if (point.y > topRight.y)
            code |= TOP;

        return code;
    }

    // ********************************************************************************************************************************************************************
    // Polygon Clipping
    // ********************************************************************************************************************************************************************

    //  裁剪区域需要为顺时针
    std::vector<glm::vec2> ClippingAlgorithm::SutherlandHodgman(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& clippingArea)
    {
        std::vector<glm::vec2> clippedVertices(vertices);
        for (int i = 0; i < clippingArea.size(); i++)
        {
            int j = (i + 1) % clippingArea.size();
            clippedVertices = Clip(clippedVertices, clippingArea[i], clippingArea[j]);
        }
        return clippedVertices;
    }

    std::vector<glm::vec2> ClippingAlgorithm::Clip(std::vector<glm::vec2> vertices, glm::vec2 point1, glm::vec2 point2)
    {
        std::vector<glm::vec2> clippedVertices;

        for (int i = 0; i < vertices.size(); i++)
        {
            int k = (i + 1) % vertices.size();  // index of the next vertex

            glm::vec2 pointI(vertices[i]);
            glm::vec2 pointK(vertices[k]);

            int posI = (point2.x - point1.x) * (pointI.y - point1.y) - (point2.y - point1.y) * (pointI.x - point1.x);
            int posK = (point2.x - point1.x) * (pointK.y - point1.y) - (point2.y - point1.y) * (pointK.x - point1.x);

            if (posI < 0 && posK < 0)
            {
                clippedVertices.push_back(pointK);
            }
            else if (posI >= 0 && posK < 0)
            {
                clippedVertices.push_back(Intersection(pointI, pointK, point1, point2));
                clippedVertices.push_back(pointK);
            }
            else if (posI < 0 && posK >= 0)
            {
                clippedVertices.push_back(Intersection(pointI, pointK, point1, point2));
            }
        }

        return clippedVertices;
    }

    glm::vec2 ClippingAlgorithm::Intersection(glm::vec2 point1, glm::vec2 point2, glm::vec2 point3, glm::vec2 point4)
    {
        glm::vec2 point;

        point.x = ((point1.x * point2.y - point1.y * point2.x) * (point3.x - point4.x) - (point1.x - point2.x) * (point3.x * point4.y - point3.y * point4.x))
            / ((point1.x - point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x - point4.x));

        point.y = ((point1.x * point2.y - point1.y * point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x * point4.y - point3.y * point4.x))
            / ((point1.x - point2.x) * (point3.y - point4.y) - (point1.y - point2.y) * (point3.x - point4.x));

        return point;
    }
}