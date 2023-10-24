#pragma once

#include "Image.h"

#include "Grafix/Scene/Scene.h"
#include "Grafix/Scene/Entity.h"

#include "Camera.h"
#include "Camera3D.h"
#include "Algorithms/LineAlgorithm.h"

#include <memory>

#include "Ray.h"

namespace Grafix
{
    class Renderer final
    {
    public:
        std::shared_ptr<Image> GetImage() const { return m_Image; }
        void OnResize(uint32_t newWidth, uint32_t newHeight);

        void BeginScene(const Camera& camera);
        void EndScene();

        void SetClearColor(const glm::vec3& color) { m_ClearColor = color; }
        int ReadPixel(glm::vec2 pos) const;

        // *******************************************************************************************************************************************************************
        // Line
        // *******************************************************************************************************************************************************************
        inline void DrawLine(glm::vec2 p0, glm::vec2 p1, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid, LineAlgorithmType algorithm = LineAlgorithmType::Bresenham, int id = -1)
        {
            DrawLine(TransformComponent(), p0, p1, color, lineWidth, lineStyle, algorithm, id);
        }

        inline void DrawLine(const TransformComponent& transform, const LineComponent& line, int id = -1)
        {
            DrawLine(transform, line.P0, line.P1, line.Color, line.LineWidth, line.LineStyle, line.Algorithm, id);
        }

        void DrawLine(const TransformComponent& transform, glm::vec2 p0, glm::vec2 p1, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid, LineAlgorithmType algorithm = LineAlgorithmType::Bresenham, int id = -1);

        // *******************************************************************************************************************************************************************
        // Circle
        // *******************************************************************************************************************************************************************
        inline void DrawCircle(glm::vec2 center, float radius, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType style = LineStyleType::Solid, int id = -1)
        {
            DrawCircle(TransformComponent(), center, radius, color, lineWidth, style, id);
        }

        inline void DrawCircle(const TransformComponent& transform, const CircleComponent& circle, int id = -1)
        {
            DrawCircle(transform, circle.Center, circle.Radius, circle.Color, circle.LineWidth, circle.LineStyle, id);
        }

        void DrawCircle(const TransformComponent& transform, glm::vec2 center, float radius, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType style = LineStyleType::Solid, int id = -1);

        // *******************************************************************************************************************************************************************
        // Arc
        // *******************************************************************************************************************************************************************
        inline void DrawArc(glm::vec2 center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType style = LineStyleType::Solid, int id = -1)
        {
            DrawArc(TransformComponent(), center, radius, angle1, angle2, major, color, lineWidth, style, id);
        }

        inline void DrawArc(const TransformComponent& transform, const ArcComponent& arc, int id = -1)
        {
            DrawArc(transform, arc.Center, arc.Radius, arc.Angle1, arc.Angle2, arc.Major, arc.Color, arc.LineWidth, arc.LineStyle, id);
        }

        void DrawArc(const TransformComponent& transform, glm::vec2 center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth = 1.0f, LineStyleType style = LineStyleType::Solid, int id = -1);

        // *******************************************************************************************************************************************************************
        // Polygon
        // *******************************************************************************************************************************************************************
        inline void DrawPolygon(const std::vector<glm::vec2>& vertices, const glm::vec3& color, int id = -1)
        {
            DrawPolygon(TransformComponent(), vertices, color, id);
        }

        inline void DrawPolygon(const TransformComponent& transform, const PolygonComponent& polygon, int id = -1)
        {
            DrawPolygon(transform, polygon.Vertices, polygon.Color, id);
        }

        void DrawPolygon(const TransformComponent& transform, const std::vector<glm::vec2>& vertices, const glm::vec3& color, int id = -1);

        // *******************************************************************************************************************************************************************
        // Curve
        // *******************************************************************************************************************************************************************
        inline void DrawCurve(const std::vector<glm::vec2>& controlPoints, const glm::vec3& color, int order, float step, std::vector<float>& knots, std::vector<float>& weights,
            float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid, CurveAlgorithmType algorithm = CurveAlgorithmType::Bezier, int id = -1)
        {
            DrawCurve(TransformComponent(), controlPoints, color, order, step, knots, weights, lineWidth, lineStyle, algorithm, id);
        }

        inline void DrawCurve(const TransformComponent& transform, CurveComponent& curve, int id = -1)
        {
            DrawCurve(transform, curve.ControlPoints, curve.Color, curve.Order, curve.Step, curve.Knots, curve.Weights, curve.LineWidth, curve.LineStyle, curve.Algorithm, id);
        }

        void DrawCurve(const TransformComponent& transform, const std::vector<glm::vec2>& controlPoints, const glm::vec3& color, int order, float step, std::vector<float>& knots, std::vector<float>& weights,
            float lineWidth = 1.0f, LineStyleType lineStyle = LineStyleType::Solid, CurveAlgorithmType algorithm = CurveAlgorithmType::Bezier, int id = -1);

        // *******************************************************************************************************************************************************************
        // Others (not reliant on any transformation)
        // *******************************************************************************************************************************************************************
        void DrawSquare(glm::vec2 center, float length, const glm::vec3& color, int id = -1);

        void DrawCross(glm::vec2 center, float radius, const glm::vec3& color, int id = -1);
        void DrawCross(const TransformComponent& transform, glm::vec2 center, float radius, const glm::vec3& color, int id = -1);

        void Fill(glm::vec2 seedPoint, const glm::vec3& fillColor);

        // NEW
        ////void SetClipRange(const glm::vec2 p0, const glm::vec2 p1);

        void SphereRender(const Camera3D& camera, glm::vec3 envir, bool flag=false, float p=4.0f);
        glm::vec4 TraceRay(Ray ray, glm::vec3 Envir, bool flag,float p);

    private:
        std::shared_ptr<Image> m_Image = nullptr;
        uint32_t* m_Pixels = nullptr;
        int* m_IdMap = nullptr;

        glm::vec2 m_ClipP0 = { 0.0f, 0.0f };
        glm::vec2 m_ClipP1 = { 1280, 720 };
        glm::vec3 m_ClearColor = { 0.158f, 0.191f, 0.214f };

        friend class GraphicsAlgorithm;
    };
}
