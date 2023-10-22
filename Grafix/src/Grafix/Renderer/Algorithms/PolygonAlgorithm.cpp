#include "pch.h"

#include "PolygonAlgorithm.h"
#include "ClippingAlgorithm.h"

namespace Grafix
{
    void PolygonAlgorithm::Scanline(const std::vector<glm::vec2>& vertices, const glm::vec3& color)
    {
        const std::vector<glm::vec2> clippedVertices = CalculateClippedVertices(vertices);
        if (clippedVertices.size() <= 2)
            return;

        uint32_t colorValue = RGBToUint32(color);

        // Find yMax and yMin
        int yMax = Math::Round(clippedVertices[0].y);
        int yMin = yMax;
        for (int i = 1; i < clippedVertices.size(); ++i)
        {
            int roundedY = Math::Round(clippedVertices[i].y);
            if (roundedY > yMax)
                yMax = roundedY;

            if (roundedY < yMin)
                yMin = roundedY;
        }

        // Initialize ET
        EdgeTable ET(yMax + 1);
        std::set<int> yValues;

        for (int i = 0; i < clippedVertices.size(); ++i)
        {
            const glm::vec2* bottomVertex = &clippedVertices[i];
            const glm::vec2* topVertex = (i < clippedVertices.size() - 1) ? &clippedVertices[i + 1] : &clippedVertices[0];

            int bottomY = Math::Round(bottomVertex->y), topY = Math::Round(topVertex->y);
            if (bottomY == topY)
                continue;

            if (bottomY > topY)
            {
                std::swap(bottomVertex, topVertex);
                std::swap(bottomY, topY);
            }

            float deltaX = (topVertex->x - bottomVertex->x) / (topVertex->y - bottomVertex->y);
            ET[bottomY].emplace_back(topY, bottomVertex->x, deltaX);
            yValues.emplace(bottomY);
        }

        for (int y : yValues)
            std::sort(ET[y].begin(), ET[y].end());

        // Create AEL
        ActiveEdgeTable AEL{};
        for (int y = yMin; y < yMax; ++y)
        {
            // If there are new edges, add them to AEL
            if (!ET[y].empty())
            {
                for (Edge& edge : ET[y])
                    AEL.push_back(std::move(edge));
            }

            std::sort(AEL.begin(), AEL.end());

            // If there are edges that end at y, remove them from AEL
            for (int i = 0; i < AEL.size(); ++i)
            {
                if (y == AEL[i].YMax)
                {
                    AEL.erase(AEL.begin() + i);
                    --i;
                }
            }

            // Draw the scanline
            for (int i = 0; i < AEL.size(); i += 2)
            {
                int x1 = Math::Round(AEL[i].X), x2 = Math::Round(AEL[i + 1].X);
                for (int x = x1; x < x2; ++x)
                    SetPixel(x, y, colorValue);
            }

            for (Edge& edge : AEL)
                edge.X += edge.DeltaX;
        }
    }

    std::vector<glm::vec2> PolygonAlgorithm::CalculateClippedVertices(const std::vector<glm::vec2>& vertices)
    {
        float canvasWidth = (float)GetCanvasWidth(), canvasHeight = (float)GetCanvasHeight();

        std::vector<glm::vec2> clippingArea{
            { 0.0f, 0.0f },
            { 0.0f , canvasHeight },
            { canvasWidth , canvasHeight },
            { canvasWidth , 0.0f}
        };

        return ClippingAlgorithm::SutherlandHodgman(vertices, clippingArea);
    }
}