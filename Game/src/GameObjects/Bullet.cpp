#include "Bullet.h"

void Bullet::OnUpdate(float ts)
{
    if(!m_Alive)
        return;

    m_Age += ts;

    if (m_Age > 200.0f)
    {
        m_Alive = false;
        m_Age = 0.0f;
        return;
    }

    m_Transform.Translation.x += 1000.0f * ts;
}

void Bullet::OnRender()
{
    if (m_Alive)
        Grafix::Renderer::DrawCircle(m_Transform.Translation, 10.0f, glm::vec3(0.2f, 0.2f, 0.2f));
}
