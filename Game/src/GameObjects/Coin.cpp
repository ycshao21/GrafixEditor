#include "Coin.h"

Coin::Coin()
{
    m_Transform.Scale = glm::vec2(10.0f);

    m_Coin.Center = { 0.0f, 0.0f };
    m_Coin.Radius = 1.0f;
    m_Coin.LineWidth = 4.0f;
    m_Coin.Color = { 1.0f, 0.843f, 0.0f };
}

void Coin::OnRender()
{
    Grafix::Renderer::DrawCircle(m_Transform, m_Coin);
}
