#include "Level.h"

void Level::OnAttach()
{
    Grafix::Renderer::SetClearColor({ 0.3f, 0.4f, 0.5f });

    m_Player.Init();
}

void Level::OnUpdate(float ts)
{
    m_Time += ts;

    m_Player.OnUpdate(ts);

    if (CollisionDetection())
    {
        m_GameOver = true;
        return;
    }

    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
        CreateBullet();

    m_Bullet.OnUpdate(ts);
}

void Level::OnRender()
{
    // Background
    RenderBackground();

    // Player
    m_Player.OnRender();
    m_Bullet.OnRender();
}

void Level::OnDetach()
{
}

void Level::Reset()
{
    m_Time = 0.0f;
    m_Score = 0;
    m_GameOver = false;

    m_Player.Init();
}

void Level::CreateBullet()
{
    m_Bullet.SetAlive(true);
    m_Bullet.SetTranslation(m_Player.GetTransform().Translation);
}

void Level::CreateTube()
{
}

bool Level::CollisionDetection()
{
    // Wall collision
    float wallEdge = m_WallHeightOffset - m_WallThickness / 2.0f;

    for (auto& v : m_Player.GetCollisionPoints())
    {
        if (std::abs(v.y) > wallEdge)
            return true;
    }
    return false;
}

void Level::RenderBackground()
{
    std::vector<glm::vec2> squareVertices = {
        {-0.5f, -0.5f},
        {0.5f, -0.5f},
        {0.5f, 0.5f},
        {-0.5f, 0.5f}
    };

    // Walls
    {

        glm::vec3 wallColor = { 0.07f, 0.07f, 0.07f };

        Grafix::TransformComponent ceilTransform;
        ceilTransform.Scale = { 2000.0f,  m_WallThickness };
        ceilTransform.Translation = { m_Player.GetTransform().Translation.x, m_WallHeightOffset  };
        Grafix::Renderer::DrawPolygon(ceilTransform, squareVertices, wallColor);

        Grafix::TransformComponent floorTransform;
        floorTransform.Scale = { 2000.0f,  m_WallThickness };
        floorTransform.Translation = { m_Player.GetTransform().Translation.x, -m_WallHeightOffset  };
        Grafix::Renderer::DrawPolygon(floorTransform, squareVertices, wallColor);
    }

    // Clouds
    {
        glm::vec3 cloudColor = { 0.8f, 0.8f, 0.8f };
        float cloudThickness = 50.0f;

        Grafix::TransformComponent cloudTransform;
        cloudTransform.Translation = { 300.0f, 0.0f };
        cloudTransform.Rotation = 45.0f;
        cloudTransform.Scale = { cloudThickness, cloudThickness };

        for (int i = 0; i < 20; i++)
        {
            Grafix::Renderer::DrawPolygon(cloudTransform, squareVertices, cloudColor);
            cloudTransform.Translation.x += 700.0f;
        }
    }
}