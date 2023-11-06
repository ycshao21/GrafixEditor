#include "Rock.h"

Grafix::PolygonComponent Rock::s_Rock;

std::vector<glm::vec2> Rock::GetCollisionPoints() const
{
	return Grafix::Math::Transform(m_Transform.GetTransformMatrix(), s_Rock.Vertices);
}

void Rock::Init()
{
    s_Rock.Vertices = {
        {0.3f, 0.0f},
        {0.29f, 0.1f},
        {0.25f, 0.3f},
        {0.2f, 0.5f},
        {0.05f, 1.0f},
        {-0.2f, 0.5f},
        {-0.25f, 0.3f},
        {-0.29f, 0.1f},
        {-0.3f, 0.0f},
    };

    s_Rock.Color = {0.158f, 0.175f, 0.196f};
}

void Rock::OnRender()
{
	Grafix::Renderer::DrawPolygon(m_Transform, s_Rock);
}

