#include "pch.h"
#include "GraphicsAlgorithm.h"

#include "LineAlgorithm.h"

namespace Grafix
{
    static uint32_t* s_PixelData = nullptr;
    static int* s_IdMap = nullptr;
    static int s_CanvasWidth = 0, s_CanvasHeight = 0;

    static int s_ID = -1;
    static int s_LineWidth = 1;
    static LineStyleType s_LineStyle = LineStyleType::Solid;

    static std::vector<char> s_Pattern{};
    static int s_Index = 0;

    void GraphicsAlgorithm::UpdatePixelData(uint32_t* pixelData, int* idMap, uint32_t width, uint32_t height)
    {
        s_PixelData = pixelData;
        s_IdMap = idMap;
        s_CanvasWidth = (int)width;
        s_CanvasHeight = (int)height;
    }

    void GraphicsAlgorithm::SetLineProperties(float lineWidth, LineStyleType lineStyle)
    {
        int oldLineWidth = s_LineWidth;
        LineStyleType oldLineStyle = s_LineStyle;

        s_Index = 0;
        s_LineWidth = Math::Round(lineWidth);
        s_LineStyle = lineStyle;

        if (lineStyle == LineStyleType::Solid || (s_LineWidth == oldLineWidth && s_LineStyle == oldLineStyle))
            return;

        switch (s_LineStyle)
        {
        case LineStyleType::Dashed:
        {
            s_Pattern.clear();
            for (int i = 0; i <= 7 * s_LineWidth; i++)
                s_Pattern.push_back('0');
            for (int i = 0; i <= 15 * s_LineWidth; i++)
                s_Pattern.push_back('1');
            break;
        }
        case LineStyleType::Dotted:
        {
            s_Pattern.clear();
            for (int i = 0; i <= 3 * s_LineWidth; i++)
                s_Pattern.push_back('0');
            for (int i = 0; i <= s_LineWidth / 2; i++)
                s_Pattern.push_back('1');
            for (int i = 0; i <= 3 * s_LineWidth; i++)
                s_Pattern.push_back('0');
            for (int i = 0; i <= 12 * s_LineWidth; i++)
                s_Pattern.push_back('1');
            break;
        }
        }
    }

    void GraphicsAlgorithm::SetID(int id)
    {
        s_ID = id;
    }

    void GraphicsAlgorithm::DrawSquare(glm::vec2 center, float length, const glm::vec3& color)
    {
        DrawSquare(Math::Round(center.x), Math::Round(center.y), Math::Round(length), RGBToUint32(color));
    }

    void GraphicsAlgorithm::DrawSquare(int centerX, int centerY, int length, uint32_t color)
    {
        int halfWidth = length >> 1;

        int xMin = Math::Clamp(centerX - halfWidth, 0, s_CanvasWidth);
        int xMax = Math::Clamp(centerX - halfWidth + length, 0, s_CanvasWidth);
        int yMin = Math::Clamp(centerY - halfWidth, 0, s_CanvasHeight);
        int yMax = Math::Clamp(centerY - halfWidth + length, 0, s_CanvasHeight);

        for (int y = yMin; y < yMax; ++y)
            for (int x = xMin; x < xMax; ++x)
                SetPixel(x, y, color);
    }

    void GraphicsAlgorithm::UpdateLinePatternIndex()
    {
        ++s_Index;
    }

    int GraphicsAlgorithm::GetCanvasWidth()
    {
        return s_CanvasWidth;
    }

    int GraphicsAlgorithm::GetCanvasHeight()
    {
        return s_CanvasHeight;
    }

    bool GraphicsAlgorithm::IsInCanvas(int x, int y)
    {
        return x >= 0 && x < s_CanvasWidth && y >= 0 && y < s_CanvasHeight;
    }

    void GraphicsAlgorithm::SetPoint(int x, int y, uint32_t color)
    {
        if (s_LineStyle == LineStyleType::Solid || s_Pattern[s_Index % s_Pattern.size()] == '1')
            DrawSquare(x, y, s_LineWidth, color);
    }

    void GraphicsAlgorithm::SetPixel(int x, int y, uint32_t color)
    {
        int index = x + y * s_CanvasWidth;
        s_PixelData[index] = color;
        s_IdMap[index] = s_ID;
    }

    uint32_t GraphicsAlgorithm::GetPixel(int x, int y)
    {
        return s_PixelData[x + y * s_CanvasWidth];
    }
}