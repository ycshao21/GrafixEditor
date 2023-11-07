#include "Medusozoa.h"

Medusozoa::Medusozoa()
{
	m_CollisionPoints = {
		{ 0.7f,  0.7f},
		{-0.7f,  0.7f},
		{-0.7f,  0.0f},
		{-0.4f,  0.0f},
		{-0.4f, -0.7f},
		{ 0.4f, -0.7f},
		{ 0.4f,  0.0f},
		{ 0.7f,  0.0f}
	};

	glm::vec3 color = {0.875f,0.55f,0.93f};
	//Body
	{
		m_Body.Color = color;

		m_Body.Vertices = {
			{0.0f, 0.0f},
			{0.3f,0.0f},
			{0.4f,-0.02f},
			{0.5f, 0.0f},
			{0.55f,0.05f},
			{0.6f,0.1f},
			{0.65f,0.12f},
			{0.68f,0.2f},
			{0.7f,0.4f},
			{0.6f,0.5f},
			{0.5f,0.55f},
			{0.4f,0.57f},
			{0.3f,0.6f},
			{0.2f,0.63f},
			{0.1f,0.65f},
			{0.0f,0.67f},
			{-0.1f,0.65f},
			{-0.2f,0.63f},
			{-0.3f,0.6f},
			{-0.4f,0.57f},
			{-0.5f,0.55f},
			{-0.6f,0.5f},
			{-0.7f,0.4f},
			{-0.68f,0.2f},
			{-0.65f,0.12f},
			{-0.6f,0.1f},
			{-0.55f,0.05f},
			{-0.5f,0.0f},
			{-0.4f,-0.02f},
			{-0.3f,0.0f},
		};
	}

	//tentacle
	{
		m_Tentacle.resize(4);
		for (auto& tentacle : m_Tentacle)
		{
			tentacle.LineWidth = 3.0f;
			tentacle.Color = color;
		}

		m_Tentacle[0].ControlPoints = {
			{-0.3f, 0.0f},
			{-0.4f,-0.3f},
			{-0.2f,-0.5f},
			{-0.3f,-0.7f}
		};

		m_Tentacle[1].ControlPoints = {
			{-0.1f, 0.0f},
			{-0.2f,-0.3f},
			{ 0.0f,-0.4f},
			{-0.1f,-0.6f}
		};

		m_Tentacle[2].ControlPoints = {
			{0.1f, 0.0f},
			{0.0f,-0.1f},
			{0.2f,-0.3f},
			{0.1f,-0.7f}
		};

		m_Tentacle[3].ControlPoints = {
			{0.3f, 0.0f},
			{0.2f,-0.2f},
			{0.4f,-0.4f},
			{0.3f,-0.6f}
		};
	}
}

void Medusozoa::OnUpdate(float ts)
{
	m_Time += ts;

	float amplitude = 100.0f;
	float frequency = 0.2f;

    m_Velocity.y = amplitude * glm::cos(2.0f * frequency * 3.14f * m_Time);
	m_Transform.Translation += m_Velocity * ts;
}

void Medusozoa::OnRender()
{
	Grafix::Renderer::DrawPolygon(m_Transform, m_Body);

	for (auto& tentacle : m_Tentacle)
		Grafix::Renderer::DrawCurve(m_Transform,tentacle);
}

std::vector<glm::vec2> Medusozoa::GetCollisionPoints()
{
	return Grafix::Math::Transform(m_Transform.GetTransformMatrix(), m_CollisionPoints);
}
