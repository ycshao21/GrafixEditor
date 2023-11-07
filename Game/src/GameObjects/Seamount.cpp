#include "Seamount.h"

Grafix::PolygonComponent Seamount::s_Seamount;

std::vector<glm::vec2> Seamount::GetCollisionPoints() const
{
	return Grafix::Math::Transform(m_Transform.GetTransformMatrix(), s_Seamount.Vertices);
}

void Seamount::Init()
{
    s_Seamount.Vertices = {
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

    s_Seamount.Color = {0.158f, 0.175f, 0.196f};
}

void Seamount::OnRender()
{
	Grafix::Renderer::DrawPolygon(m_Transform, s_Seamount);
}

