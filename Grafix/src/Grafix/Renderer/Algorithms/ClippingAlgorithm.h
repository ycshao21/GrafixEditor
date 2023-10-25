#pragma once
#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class ClippingAlgorithm final : public GraphicsAlgorithm
    {
    public:
        // Line
        static std::vector<glm::vec2> CohenSutherland(glm::vec2 lineP0, glm::vec2 lineP1, glm::vec2 bottomLeft, glm::vec2 topRight);
        static std::vector<glm::vec2> Midpoint(glm::vec2 lineP0, glm::vec2 lineP1, glm::vec2 bottomLeft, glm::vec2 topRight);
        //Polygon
        static std::vector<glm::vec2> SutherlandHodgman(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& clippingArea);

    private:
        // Line
        static int Encode(glm::vec2 point, glm::vec2 bottomLeft, glm::vec2 topRight);
        //Polygon
        static std::vector<glm::vec2> Clip(std::vector<glm::vec2> vertices, glm::vec2 point1, glm::vec2 point2);
        static glm::vec2 Intersection(glm::vec2 point1, glm::vec2 point2, glm::vec2 point3, glm::vec2 point4);
    };
}
