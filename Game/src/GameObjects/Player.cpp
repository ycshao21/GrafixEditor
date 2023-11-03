#include "Player.h"

void Player::Init()
{
    m_Transform.Translation = glm::vec2(0.0f);
    m_Transform.Rotation = 0.0f;
    m_Transform.Scale = { 50.0f, 60.0f };

    m_Plane.Vertices = {
        {1.0f, 0.0f},
        {0.5f, 0.3f},
        {-1.0f, 0.3f},
        {-1.0f, -0.3f},
        {0.5f, -0.3f}
    };
    m_Plane.Color = glm::vec3(0.8f, 0.4f, 0.1f);
}

std::vector<glm::vec2> Player::GetCollisionPoints() const
{
    return Grafix::Math::Transform(m_Transform.GetTransformMatrix(), m_CollisionPoints);
}

void Player::OnUpdate(float ts)
{
    m_Transform.Translation.x += m_MoveSpeed.x * ts;

    if(Grafix::Input::IsKeyPressed(Grafix::Key::Space))
    ////if (ImGui::IsKeyPressed(ImGuiKey_Space))
    {
        m_Transform.Translation.y += m_MoveSpeed.y * ts;
        m_Transform.Rotation = glm::clamp(m_Transform.Rotation + m_RotationSpeed * ts, -10.0f, 10.0f);
    }
    else
    {
        m_Transform.Translation.y -= m_MoveSpeed.y * ts;
        m_Transform.Rotation = std::clamp(m_Transform.Rotation - m_RotationSpeed * ts, -10.0f, 10.0f);
    }
}

void Player::OnRender()
{
    Grafix::Renderer::DrawPolygon(m_Transform, m_Plane);
}