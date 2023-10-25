#include "pch.h"
#include "CurveAlgorithm.h"

namespace Grafix
{
    // ***************************************************************************************************************************************************************
    // Bezier
    // ***************************************************************************************************************************************************************

    void CurveAlgorithm::Bezier(const std::vector<glm::vec2>& controlPoints, float step, const glm::vec3& color)
    {
        if (controlPoints.size() < 2)
            return;

        uint32_t colorValue = RGBToUint32(color);

        for (float u = 0.0f; u <= 1.0f; u += step)
        {
            glm::vec2 point = GetBezierPoint(controlPoints, u);
            SetPoint(Math::Round(point.x), Math::Round(point.y), colorValue);
            UpdateLinePatternIndex();
        }
    }

    glm::vec2 CurveAlgorithm::GetBezierPoint(const std::vector<glm::vec2>& controlPoints, float u)
    {
        std::vector<glm::vec2> bezierPoints(controlPoints.size() - 1);
        for (int i = 0; i < bezierPoints.size(); i++)
            bezierPoints[i] = (1 - u) * controlPoints[i] + u * controlPoints[i + 1];

        if (bezierPoints.size() == 1)
            return bezierPoints[0];

        return GetBezierPoint(bezierPoints, u);
    }

    // ***************************************************************************************************************************************************************
    // NURBS
    // ***************************************************************************************************************************************************************

    void CurveAlgorithm::NURBS(const std::vector<glm::vec2>& controlPoints, int order, float step, const glm::vec3& color, std::vector<float>& knots, std::vector<float>& weights)
    {
        if (controlPoints.size() < order)
            return;

        uint32_t colorValue = RGBToUint32(color);

        int controlPointSize = controlPoints.size();
        // Update knots and weights
        if (knots.size() != controlPointSize + order)
            GenerateKnots(knots, controlPointSize, order);

        if (weights.size() != controlPointSize)
            GenerateWeights(weights, controlPointSize);

        for (float u = 0.0f; u <= 1.0f; u += step)
        {
            glm::vec2 point{ 0.0f, 0.0f };
            for (int i = 0; i < controlPointSize; i++)
                point += controlPoints[i] * BaseFunction(i, order, u, knots);

            SetPoint(Math::Round(point.x), Math::Round(point.y), colorValue);
            UpdateLinePatternIndex();
        }
    }

    float CurveAlgorithm::BaseFunction(int i, int order, float u, const std::vector<float>& knots)
    {
        if (order == 1)
            return (knots[i] <= u && u <= knots[i + 1]) ? 1.0f : 0.0f;

        float div1 = knots[i + order - 1] - knots[i];
        float div2 = knots[i + order] - knots[i + 1];

        if (div1 == 0.0f)
            div1 = 1.0f;
        if (div2 == 0.0f)
            div2 = 1.0f;

        return (u - knots[i]) / div1 * BaseFunction(i, order - 1, u, knots)
            + (knots[i + order] - u) / div2 * BaseFunction(i + 1, order - 1, u, knots);
    }

    void CurveAlgorithm::GenerateKnots(std::vector<float>& knots, int controlPointSize, int order)
    {
        srand(time(nullptr));
        knots.resize(controlPointSize + order);

        for (int i = 0; i < order; ++i)
            knots[i] = 0.0f;

        for (int i = order; i < controlPointSize; ++i)
        {
            float r = (float)rand() / (RAND_MAX * knots.size() / 3);
            knots[i] = r * (1.0f - knots[i - 1]) + knots[i - 1];
        }

        for (int i = controlPointSize; i < knots.size(); ++i)
            knots[i] = 1.0f;
    }

    void CurveAlgorithm::GenerateWeights(std::vector<float>& weights, int controlPointSize)
    {
        srand(time(nullptr));
        weights.resize(controlPointSize);

        float sum = 0.0f;
        for (int i = 0; i < weights.size(); ++i)
        {
            weights[i] = (float)rand() / RAND_MAX;
            sum += weights[i];
        }

        for (int i = 0; i < weights.size(); ++i)
            weights[i] /= sum;
    }
}