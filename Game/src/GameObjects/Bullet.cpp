#include "Bullet.h"

void Bullet::OnUpdate(float ts)
{
    if (IsAlive())
    {
        m_LifeRemaining -= ts;
        m_Transform.Translation.x += 1000.0f * ts;
    }
}

void Bullet::OnRender()
{
    if (IsAlive())
        Grafix::Renderer::DrawCircle(m_Transform.Translation, 10.0f, glm::vec3(0.2f, 0.2f, 0.2f));
}

void Bullet::SetAlive(bool alive)
{
    m_LifeRemaining = alive ? m_LifeTime : 0.0f;
}
