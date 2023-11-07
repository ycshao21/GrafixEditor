#include "shark.h"

Shark::Shark()
{
	m_CollisionPoints = {
		{-0.9f, 0.0f},
		{-0.5f,-0.2f},
		{ 0.5f,-0.1f},
		{ 0.8f,-0.25f},
		{ 0.8f, 0.3f},
		{ 0.5f, 0.0f},
		{ 0.1f, 0.65f},
		{-0.3f, 0.35f},
		{-0.8f, 0.3f}
	};

	//Body
	{
		glm::vec3 color{ 1.0f };
		m_Body.resize(7);

		for (auto& body : m_Body)
		{
			body.LineWidth = 2.0f;
			body.Color = color;
		}

		m_Body[0].ControlPoints = {
			{-0.9f, 0.0f},
			{-0.8f, 0.3f},
			{-0.3f, 0.35f},
		};

		m_Body[1].ControlPoints = {
			{-0.3f,  0.35f},
			{-0.2f, 0.55f},
			{ 0.1f,  0.65f},
		};

		m_Body[2].ControlPoints = {
			{0.1f,   0.65f},
			{0.0f,   0.4f},
			{-0.05f, 0.2f},
		};

		m_Body[3].ControlPoints = {
			{-0.05f, 0.2f},
			{0.2f,   0.2f},
			{0.3f,   0.0f},
			{0.5f,  -0.1f}, //tail point
			{0.6f,  -0.15f},
			{0.7f,  -0.2f},
			{0.8f,  -0.25f},
		};

		m_Body[4].ControlPoints = {
			{0.8f, -0.25f},
			{0.6f, -0.1f},
			{0.8f,  0.3f}
		};

		m_Body[5].ControlPoints = {
			{0.5f,-0.1f},
			{0.7f, 0.2f},
			{0.8f, 0.3f}
		};

		m_Body[6].ControlPoints = {
			{-0.9f, 0.0f},
			{-0.4f, -0.3f},
			{-0.3f, -0.35f},
			{0.5f, -0.1f},
		};
	}

	//Eye 
	{
		m_Eye.Center = { -0.7f, 0.05f };
		m_Eye.Radius = 0.05f;
		m_Eye.Color = { 1.0f, 1.0f, 1.0f };
		m_Eye.LineWidth = 1.0f;
	}

	//Line
	{
		m_Line.resize(3);
		for (auto& line : m_Line)
		{
			line.LineWidth = 1.0f;
			line.Color = { 1.0f, 1.0f, 1.0f };
		}

		m_Line[0].P0 = { -0.4f, 0.0f };
		m_Line[0].P1 = { -0.65f,-0.16f };

		m_Line[1].P0 = { -0.4f, 0.0f };
		m_Line[1].P1 = { -0.5f,-0.2f };

		m_Line[2].P0 = { -0.2f,-0.2f };
		m_Line[2].P1 = { 0.2f,-0.3f };
	}
}

void Shark::OnUpdate(float ts)
{
	m_Transform.Translation += m_Velocity * ts;
}

void Shark::OnRender()
{
	for (auto& body : m_Body)
		Grafix::Renderer::DrawCurve(m_Transform, body);

	for (auto& line : m_Line)
		Grafix::Renderer::DrawLine(m_Transform, line);

	Grafix::Renderer::DrawCircle(m_Transform, m_Eye);
}

std::vector<glm::vec2> Shark::GetCollisionPoints()
{
	return Grafix::Math::Transform(m_Transform.GetTransformMatrix(), m_CollisionPoints);
}

