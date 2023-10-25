#pragma once

#include "GraphicsAlgorithm.h"

namespace Grafix
{
    class CurveAlgorithm final : public GraphicsAlgorithm
    {
    public:
        static void Bezier(const std::vector<glm::vec2>& point, float step, const glm::vec3& color);
        // n+1个控制点，k阶，次数=k-1
        static void NURBS(const std::vector<glm::vec2>& controlPoints, int order, float step, const glm::vec3& color, std::vector<float>& knots, std::vector<float>& weights);
    private:
        //For Bezier
        static glm::vec2 GetBezierPoint(const std::vector<glm::vec2>& controlPoints, float u);
        // For NURBS
        static float BaseFunction(int i, int k, float u, const std::vector<float>& knots);
        static void GenerateKnots(std::vector<float>& knots, int controlPointSize, int order);
        static void GenerateWeights(std::vector<float>& weights, int controlPointSize);
    };
}
