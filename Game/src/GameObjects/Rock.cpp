#include "Rock.h"

void Rock::Init()
{
	std::vector<glm::vec2> rockVertices = {
		{0.3f, 0.0f},
		{0.29f, 0.1f},
		{0.25f, 0.3f},
		{0.2f, 0.5f},
		{0.0f, 1.0f},
		{-0.2f, 0.5f},
		{-0.25f, 0.3f},
		{-0.29f, 0.1f},
		{-0.3f, 0.0f},
		
	};
	m_Rock.Vertices = rockVertices;
	m_Color = glm::vec3{ 0.0f };

	m_Transform.Translation = glm::vec2(0.0f, 50.0f);
	m_Transform.Rotation = 0.0f;
	m_Transform.Scale = { 300.0f, 270.0f };

}

std::vector<glm::vec2> Rock::collisionPoints()
{
	return Grafix::Math::Transform(m_Transform.GetTransformMatrix(), m_Rock.Vertices);
}

void Rock::OnUpdate(float ts)
{

}

void Rock::OnRender()
{
	Grafix::Renderer::DrawPolygon(m_Transform, m_Rock.Vertices, m_Color, 2.0f);
}

