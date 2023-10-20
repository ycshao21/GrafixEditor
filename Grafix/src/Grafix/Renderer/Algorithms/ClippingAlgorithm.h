#pragma once
#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class ClippingAlgorithm : public GraphicsAlgorithm
    {
        // Line
    public:
        static std::vector<glm::vec2> CohenSutherland(glm::vec2 beginPoint, glm::vec2 endPoint, glm::vec2 leftBottom, glm::vec2 rightTop);
        static std::vector<glm::vec2> MidPoint(glm::vec2 beginPoint, glm::vec2 endPoint, glm::vec2 leftBottom, glm::vec2 rightTop);
    private:
        static int Encode(glm::vec2 point, glm::vec2 leftBottom, glm::vec2 rightTop);

        //Polygon
    public:
        static std::vector<glm::vec2> SutherlandHodgman(const std::vector<glm::vec2>& vertices, const std::vector<glm::vec2>& clippingArea);
    private:
        static std::vector<glm::vec2> Clip(std::vector<glm::vec2> vertices, glm::vec2 point1, glm::vec2 point2);
        static glm::vec2 Intersection(glm::vec2 point1, glm::vec2 point2, glm::vec2 point3, glm::vec2 point4);
    };
}
