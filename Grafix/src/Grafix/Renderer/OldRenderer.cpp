#include "pch.h"
#include "OldRenderer.h"

#include "Grafix/Utils/ColorConvert.hpp"
#include "Grafix/Math/Math.h"

#include "Algorithms/GraphicsAlgorithm.h"
#include "Algorithms/LineAlgorithm.h"
#include "Algorithms/CircleAlgorithm.h"
#include "Algorithms/ArcAlgorithm.h"
#include "Algorithms/PolygonAlgorithm.h"
#include "Algorithms/SeedFillAlgorithm.h"
#include "Algorithms/CurveAlgorithm.h"

namespace Grafix
{
    static glm::mat3 s_ViewMatrix = glm::mat3(1.0f);

    void OldRenderer::OnResize(uint32_t newWidth, uint32_t newHeight)
    {
        if (m_Image)
        {
            if (m_Image->GetWidth() == newWidth && m_Image->GetHeight() == newHeight)
                return;
            m_Image->Resize(newWidth, newHeight);
        }
        else
        {
            m_Image = std::make_shared<Image>(newWidth, newHeight, ImageFormat::RGBA);
        }

        delete[] m_Pixels;
        delete[] m_IdMap;
        m_Pixels = new uint32_t[newWidth * newHeight];
        m_IdMap = new int[newWidth * newHeight];
    }

    void OldRenderer::BeginScene(const Camera& camera)
    {
        s_ViewMatrix = camera.GetViewMatrix();
        GraphicsAlgorithm::UpdatePixelData(m_Pixels, m_IdMap, m_Image->GetWidth(), m_Image->GetHeight());

        // Clear color
        uint32_t clearColor = RGBToUint32(m_ClearColor);
        std::fill(m_Pixels, m_Pixels + m_Image->GetWidth() * m_Image->GetHeight(), clearColor);
        std::fill(m_IdMap, m_IdMap + m_Image->GetWidth() * m_Image->GetHeight(), -1);
    }

    void OldRenderer::EndScene()
    {
        m_Image->SetPixels(m_Pixels);
    }

    int OldRenderer::ReadPixel(glm::vec2 pos) const
    {
        return m_IdMap[(int)pos.x + (int)pos.y * m_Image->GetWidth()];
    }

    // ******************************************************************************************************************************************
    // Draw Functions
    // ******************************************************************************************************************************************

    void OldRenderer::DrawLine(const TransformComponent& transform, glm::vec2 p0, glm::vec2 p1, const glm::vec3& color, float lineWidth, LineStyleType lineStyle, LineAlgorithmType algorithm, int id)
    {
        GraphicsAlgorithm::SetID(id);
        GraphicsAlgorithm::SetLineProperties(lineWidth, lineStyle);

        glm::vec2 newP0 = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), p0));
        glm::vec2 newP1 = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), p1));

        switch (algorithm)
        {
        case LineAlgorithmType::Midpoint:
        {
            LineAlgorithm::Midpoint(newP0, newP1, color);
            break;
        }
        case LineAlgorithmType::Bresenham:
        {
            LineAlgorithm::Bresenham(newP0, newP1, color);
            break;
        }
        }
    }

    void OldRenderer::DrawCircle(const TransformComponent& transform, glm::vec2 center, float radius, const glm::vec3& color, float lineWidth, LineStyleType lineStyle, int id)
    {
        GraphicsAlgorithm::SetID(id);
        GraphicsAlgorithm::SetLineProperties(lineWidth, lineStyle);

        glm::vec2 newCenter = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), center));
        float newRadius = radius * transform.Scale.x;
        CircleAlgorithm::Midpoint(newCenter, newRadius, color);
    }

    // NOTE: Not reliant on camera
    void OldRenderer::DrawArc(const TransformComponent& transform, glm::vec2 center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth, LineStyleType lineStyle, int id)
    {
        GraphicsAlgorithm::SetID(id);
        GraphicsAlgorithm::SetLineProperties(lineWidth, lineStyle);

        glm::vec2 newCenter = Math::Transform(transform.GetTransformMatrix(), center);
        float newRadius = radius * transform.Scale.x;

        float radiansAngle1 = glm::radians(angle1);
        float radiansAngle2 = glm::radians(angle2);
        glm::vec2 delta1 = radius * glm::vec2{ glm::cos(radiansAngle1), glm::sin(radiansAngle1) };
        glm::vec2 delta2 = radius * glm::vec2{ glm::cos(radiansAngle2), glm::sin(radiansAngle2) };
        delta1 = Math::Transform(transform.GetTransformMatrix(), delta1 + center) - newCenter;
        delta2 = Math::Transform(transform.GetTransformMatrix(), delta2 + center) - newCenter;

        float newAngle1 = glm::degrees(glm::atan(delta1.y, delta1.x));
        float newAngle2 = glm::degrees(glm::atan(delta2.y, delta2.x));

        ArcAlgorithm::Midpoint(newCenter, newRadius, newAngle1, newAngle2, major, color);
    }

    void OldRenderer::DrawPolygon(const TransformComponent& transform, const std::vector<glm::vec2>& vertices, const glm::vec3& color, int id)
    {
        GraphicsAlgorithm::SetID(id);

        auto newVertices = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), vertices));
        PolygonAlgorithm::Scanline(newVertices, color);
    }

    void OldRenderer::DrawCurve(const TransformComponent& transform, const std::vector<glm::vec2>& controlPoints,
        const glm::vec3& color, int order, float step, std::vector<float>& knots,
        std::vector<float>& weights, float lineWidth, LineStyleType lineStyle, CurveAlgorithmType algorithm, int id)
    {
        GraphicsAlgorithm::SetID(id);
        GraphicsAlgorithm::SetLineProperties(lineWidth, lineStyle);

        std::vector<glm::vec2> newControlPoints = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), controlPoints));

        switch (algorithm)
        {
        case CurveAlgorithmType::Bezier:
        {
            CurveAlgorithm::Bezier(newControlPoints, step, color);
            break;
        }
        case CurveAlgorithmType::NURBS:
        {
            CurveAlgorithm::NURBS(newControlPoints, order, step, color, knots, weights);
            break;
        }
        }
    }

    void OldRenderer::DrawCross(const TransformComponent& transform, glm::vec2 center, float radius, const glm::vec3& color, int id)
    {
        GraphicsAlgorithm::SetID(id);
        GraphicsAlgorithm::SetLineProperties(1.0f, LineStyleType::Solid);

        glm::vec2 transformedCenter = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), center));
        LineAlgorithm::Bresenham(
            { transformedCenter.x - radius, transformedCenter.y },
            { transformedCenter.x + radius, transformedCenter.y },
            color
        );

        LineAlgorithm::Bresenham(
            { transformedCenter.x, transformedCenter.y - radius },
            { transformedCenter.x, transformedCenter.y + radius },
            color
        );
    }

    void OldRenderer::DrawSquare(glm::vec2 center, float length, const glm::vec3& color, int id)
    {
        GraphicsAlgorithm::SetID(id);
        GraphicsAlgorithm::DrawSquare(center, length, color);
    }

    void OldRenderer::DrawRect(const TransformComponent& transform, glm::vec2 leftBottom, glm::vec2 rightTop, const glm::vec3& color, int id)
    {
        DrawLine(transform, leftBottom, { rightTop.x, leftBottom.y }, color, 1.0f, LineStyleType::Dashed, LineAlgorithmType::Bresenham, -1);
        DrawLine(transform, { rightTop.x, leftBottom.y }, rightTop, color, 1.0f, LineStyleType::Dashed, LineAlgorithmType::Bresenham, -1);
        DrawLine(transform, rightTop, { leftBottom.x, rightTop.y }, color, 1.0f, LineStyleType::Dashed, LineAlgorithmType::Bresenham, -1);
        DrawLine(transform, { leftBottom.x, rightTop.y }, leftBottom, color, 1.0f, LineStyleType::Dashed, LineAlgorithmType::Bresenham, -1);
    }

    void OldRenderer::Fill(glm::vec2 point, const glm::vec3& color)
    {
        uint32_t oldColor = m_Pixels[(uint32_t)point.x + (uint32_t)point.y * m_Image->GetWidth()];
        SeedFillAlgorithm::FloodFill(point, RGBToUint32(color), oldColor);
    }

    // ****************************************************************************************************************************************************************
    // Sphere
    // ****************************************************************************************************************************************************************

    void OldRenderer::DrawSphere(const Camera3D& camera, const Scene& scene)
    {
        m_ActiveCamera = &camera;
        m_ActiveScene = &scene;

        for (int y = 0; y < m_Image->GetHeight(); y++)
        {
            for (int x = 0; x < m_Image->GetWidth(); x++)
            {
                glm::vec3 color = CalculateSpherePixel(x, y);
                m_Pixels[x + y * m_Image->GetWidth()] = RGBToUint32(color);
            }
        }
    }

    glm::vec3 OldRenderer::CalculateSpherePixel(uint32_t x, uint32_t y)
    {
        Ray ray;
        ray.Origin = m_ActiveCamera->GetPosition();
        ray.Direction = m_ActiveCamera->GetRayDirections()[x + y * m_Image->GetWidth()];

        glm::vec3 color = TraceRay(ray);
        return glm::clamp(color, 0.0f, 1.0f);
    }

    glm::vec3 OldRenderer::TraceRay(const Ray& ray)
    {
        glm::vec3 envirColor = glm::vec3(0.0f);

        auto envirView = m_ActiveScene->GetEntitiesWith<EnvironmentComponent>();
        if (envirView.empty())
            return envirColor;

        auto& envir = Entity(envirView.front(), (Scene*)m_ActiveScene).GetComponent<EnvironmentComponent>();
        envirColor = envir.LightColor * 0.01f;

        auto sphereView = m_ActiveScene->GetEntitiesWith<SphereComponent>();
        if (sphereView.empty())
            return envirColor;

        auto& sphere = Entity(sphereView.front(), (Scene*)m_ActiveScene).GetComponent<SphereComponent>();

        glm::vec3 origin = ray.Origin - sphere.Position;

        float a = glm::dot(ray.Direction, ray.Direction);
        float b = 2.0f * glm::dot(origin, ray.Direction);
        float c = glm::dot(origin, origin) - sphere.Radius * sphere.Radius;

        float discriminant = b * b - 4.0f * a * c;
        if (discriminant < 0.0f)  // Miss
            return envirColor;

        float t0 = (-b + sqrt(discriminant)) / (2.0f * a);
        float closestT = (-b - sqrt(discriminant)) / (2.0f * a);

        glm::vec3 hitPoint = origin + ray.Direction * closestT;
        glm::vec3 normal = glm::normalize(hitPoint);

        glm::vec3 lightDir = glm::normalize(envir.LightDir);
        glm::vec3 ambientIntensity = envir.LightColor * sphere.KAmbient;
        glm::vec3 diffuseIntensity = sphere.KDiffusion * envir.LightColor * glm::max(glm::dot(normal, -lightDir), 0.0f);

        if (!sphere.UsePhong)
            return ambientIntensity + diffuseIntensity;

        glm::vec3 viewDir = glm::normalize(origin);
        glm::vec3 halfDir = glm::normalize(-lightDir + viewDir);
        float spec = glm::pow(glm::max(glm::dot(normal, halfDir), 0.0f), sphere.Glossiness);
        glm::vec3 specularIntensity = sphere.KSpecular * envir.LightColor * spec;
        return ambientIntensity + diffuseIntensity + specularIntensity;
    }
}