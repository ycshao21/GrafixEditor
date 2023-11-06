#include "Bullet.h"

Grafix::PolygonComponent Bullet::s_Bullet;

Bullet::Bullet()
{
    m_Transform.Scale = { 10.0f, 5.0f };
}

void Bullet::Init()
{
    s_Bullet.Vertices = {
        {-1.0f, -1.0f},
        {0.3f, -1.0f},
        {1.0f, 0.0f},
        {0.3f, 1.0f},
        {-1.0f, 1.0f}
    };
    s_Bullet.Color = glm::vec3(0.8f);
}

void Bullet::OnUpdate(float ts)
{
    m_LifeRemaining -= ts;
    m_Transform.Translation += m_Velocity * ts;
}

void Bullet::OnRender()
{
    Grafix::Renderer::DrawPolygon(m_Transform, s_Bullet);
}

std::vector<glm::vec2> Bullet::GetCollisionPoints() const
{
    return Grafix::Math::Transform(m_Transform.GetTransformMatrix(), m_CollisionPoints);
}

void Bullet::Activate(glm::vec2 position, float degree)
{
    m_LifeRemaining = m_LifeTime;

    glm::mat3 rotationMatrix = Grafix::Math::CalcRotationMatrix(glm::radians(degree));

    m_Transform.Translation = position + Grafix::Math::Transform(rotationMatrix, glm::vec2(60.0f, 0.0f));
    m_Transform.Rotation = degree;
	m_Velocity = Grafix::Math::Transform(rotationMatrix, glm::vec2(1000.0f, 0.0f));
}

void Bullet::Deactivate()
{
    m_LifeRemaining = 0.0f;
}
