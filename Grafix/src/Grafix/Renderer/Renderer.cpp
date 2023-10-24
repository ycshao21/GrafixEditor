#include "pch.h"
#include "Renderer.h"

#include "Grafix/Utils/ColorConvert.hpp"
#include "Grafix/Math/Math.h"

#include "Algorithms/GraphicsAlgorithm.h"
#include "Algorithms/LineAlgorithm.h"
#include "Algorithms/CircleAlgorithm.h"
#include "Algorithms/ArcAlgorithm.h"
#include "Algorithms/PolygonAlgorithm.h"
#include "Algorithms/SeedFillAlgorithm.h"
#include "Algorithms/CurveAlgorithm.h"

// NEW
#include "Algorithms/ClippingAlgorithm.h"

namespace Grafix
{
	static glm::mat3 s_ViewMatrix = glm::mat3(1.0f);

	void Renderer::OnResize(uint32_t newWidth, uint32_t newHeight)
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

	void Renderer::BeginScene(const Camera& camera)
	{
		s_ViewMatrix = camera.GetViewMatrix();
		GraphicsAlgorithm::UpdatePixelData(m_Pixels, m_IdMap, m_Image->GetWidth(), m_Image->GetHeight());

		// Clear color
		uint32_t clearColor = RGBToUint32(m_ClearColor);
		std::fill(m_Pixels, m_Pixels + m_Image->GetWidth() * m_Image->GetHeight(), clearColor);
		std::fill(m_IdMap, m_IdMap + m_Image->GetWidth() * m_Image->GetHeight(), -1);
	}

	void Renderer::EndScene()
	{
		m_Image->SetPixels(m_Pixels);
	}

	int Renderer::ReadPixel(glm::vec2 pos) const
	{
		return m_IdMap[(int)pos.x + (int)pos.y * m_Image->GetWidth()];
	}

	// ******************************************************************************************************************************************
	// Draw Functions
	// ******************************************************************************************************************************************

    ////void Renderer::SetClipRange(const glm::vec2 p0, const glm::vec2 p1)
    ////{
    ////    m_ClipP0 = p0;
    ////    m_ClipP1 = p1;
    ////}

	void Renderer::DrawLine(const TransformComponent& transform, glm::vec2 p0, glm::vec2 p1, const glm::vec3& color, float lineWidth, LineStyleType lineStyle, LineAlgorithmType algorithm, int id)
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

    void Renderer::DrawCircle(const TransformComponent& transform, glm::vec2 center, float radius, const glm::vec3& color, float lineWidth, LineStyleType lineStyle, int id)
    {
        GraphicsAlgorithm::SetID(id);
        GraphicsAlgorithm::SetLineProperties(lineWidth, lineStyle);

        glm::vec2 newCenter = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), center));
        float newRadius = radius * transform.Scale.x;
        CircleAlgorithm::Midpoint(newCenter, newRadius, color);
    }

    // NOTE: Not reliant on camera
    void Renderer::DrawArc(const TransformComponent& transform, glm::vec2 center, float radius, float angle1, float angle2, bool major, const glm::vec3& color, float lineWidth, LineStyleType lineStyle, int id)
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

    void Renderer::DrawPolygon(const TransformComponent& transform, const std::vector<glm::vec2>& vertices, const glm::vec3& color, int id)
    {
        GraphicsAlgorithm::SetID(id);

        auto newVertices = Math::Transform(s_ViewMatrix, Math::Transform(transform.GetTransformMatrix(), vertices));
        PolygonAlgorithm::Scanline(newVertices, color);
    }

	void Renderer::DrawCurve(const TransformComponent& transform, const std::vector<glm::vec2>& controlPoints,
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

	void Renderer::DrawCross(glm::vec2 center, float radius, const glm::vec3& color, int id)
	{
		DrawCross(TransformComponent(), center, radius, color);
	}

	void Renderer::DrawCross(const TransformComponent& transform, glm::vec2 center, float radius, const glm::vec3& color, int id)
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

	void Renderer::DrawSquare(glm::vec2 center, float length, const glm::vec3& color, int id)
	{
		GraphicsAlgorithm::SetID(id);
		GraphicsAlgorithm::DrawSquare(center, length, color);
	}

	void Renderer::Fill(glm::vec2 seedPoint, const glm::vec3& fillColor)
	{
		uint32_t oldColor = m_Pixels[(uint32_t)seedPoint.x + (uint32_t)seedPoint.y * m_Image->GetWidth()];
		SeedFillAlgorithm::FloodFill(seedPoint, RGBToUint32(fillColor), oldColor);
	}

	void Renderer::SphereRender(const Camera3D& camera, glm::vec3 envir, bool flag, float p)
	{
		const glm::vec3& rayOrigin = camera.GetPosition();

		Ray ray;
		ray.origin = rayOrigin;

		for (uint32_t y = 0; y < m_Image->GetHeight(); y++)
		{
			for (uint32_t x = 0; x < m_Image->GetWidth(); x++)
			{
				const glm::vec3& rayDirection = camera.GetRayDirections()[x + y * m_Image->GetWidth()];
				ray.direction = rayDirection;
				glm::vec4 color = TraceRay(ray, envir, flag, p);
				color = glm::clamp(color, 0.0f, 1.0f);
				m_Pixels[x + y * m_Image->GetWidth()] = RGBToUint32(color);
			}
		}

		m_Image->SetPixels(m_Pixels);
	}

	glm::vec4 Renderer::TraceRay(Ray ray, glm::vec3 envir, bool flag, float p)
	{
		glm::vec3 rayorigin = ray.origin;
		glm::vec3 raydirection = ray.direction;
		float radius = 0.5f;

		float a = glm::dot(raydirection, raydirection);
		float b = 2.0f * glm::dot(rayorigin, raydirection);
		float c = glm::dot(rayorigin, rayorigin) - radius * radius;

		float discriminant = b * b - 4.0f * a * c;
		if (discriminant < 0.0f)
			return glm::vec4(0, 0, 0, 1);

		float t0 = (-b + sqrt(discriminant)) / (2.0f * a);
		float closestT = (-b - sqrt(discriminant)) / (2.0f * a);

		glm::vec3 hitPoint = rayorigin + raydirection * closestT;
		glm::vec3 normal = glm::normalize(hitPoint);

		glm::vec3 lightdir = glm::normalize(envir);

		glm::vec3 sphereColor(1, 0, 0); //surface color
		//lambert
		float d = glm::max(glm::dot(normal, -lightdir), 0.0f);
		sphereColor *= d;
		auto color = glm::vec4(sphereColor, 1.0f);
		//phong
		if (flag)
		{
			glm::vec3 viewdir = glm::normalize(rayorigin);
			glm::vec3 halfdir = glm::normalize(-lightdir + viewdir);
			float s = 0.7f * glm::pow(glm::max(glm::dot(normal, halfdir), 0.0f), p);
			color += glm::vec4(s, s, s, 1.0f);
		}
		// C =[ Kd * max{0,(N dot L)} + Ks * max{0,(N dot H)}^p ] *I
		return color;
	}

}