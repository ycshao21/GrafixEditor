#include "BulletItem.h"

BulletItem::BulletItem()
{
    m_Transform.Rotation = 90.0f;
    m_Transform.Scale = { 11.0f, 4.0f };

    m_Bullet.Vertices = {
        { -1.0f, -1.0f },
        {  0.3f, -1.0f },
        {  1.0f,  0.0f },
        {  0.3f,  1.0f },
        { -1.0f,  1.0f }
    };
    m_Bullet.Color = { 0.724, 0.9f, 0.881f };

    m_Shell.Center = glm::vec2(0.0f);
    m_Shell.Radius = 1.5f;
    m_Shell.Color = glm::vec3(0.5f);
    m_Shell.LineWidth = 2.0f;
}

void BulletItem::OnRender()
{
    Grafix::Renderer::DrawPolygon(m_Transform, m_Bullet);
    Grafix::Renderer::DrawCircle(m_Transform, m_Shell);
}
