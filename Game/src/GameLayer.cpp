#include "GameLayer.h"

static uint32_t s_MaxViewportSize = 16384;

void GameLayer::OnAttach()
{
    Grafix::Renderer::Init(m_CanvasWidth, m_CanvasHeight);

    m_Camera = std::make_unique<Grafix::Camera>(m_CanvasWidth, m_CanvasHeight);
    m_Camera->SetOriginAtCenter();

    m_Level = std::make_unique<Level>();
    m_Level->Init();
}

void GameLayer::OnUpdate(float ts)
{
	m_Time += ts;

    auto playerPos = m_Level->GetPlayer().GetPosition();
    m_Camera->SetPosition({ playerPos.x + 250.0f, playerPos.y });

    if (m_Level->IsGameOver())
        m_GameState = GameState::GameOver;

    switch (m_GameState)
    {
        case GameState::Playing:
        {
            m_Level->OnUpdate(ts);
            break;
        }
    }

    // Render
    Grafix::Renderer::BeginFrame(*m_Camera);
    m_Level->OnRender();
    Grafix::Renderer::EndFrame();
}

void GameLayer::OnUIRender()
{
    static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
    ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;

    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

    if (dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        windowFlags |= ImGuiWindowFlags_NoBackground;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace", nullptr, windowFlags);
    {
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspaceId = ImGui::GetID("VulkanAppDockspace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }

        // Viewport
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Viewport");
        if (m_CanvasWidth > 0 && m_CanvasWidth <= s_MaxViewportSize && m_CanvasHeight > 0 && m_CanvasHeight <= s_MaxViewportSize)
        {
            if (auto image = Grafix::Renderer::GetImage())
            {
                ImGui::Image(image->GetDescriptorSet(),
                    { (float)image->GetWidth(), (float)image->GetHeight() },
                    ImVec2(0, 1), ImVec2(1, 0)
                );
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
        ////UI_Info();
    }
    ImGui::End(); // DockSpace

    ImVec2 pos = ImGui::GetWindowPos();
    uint32_t white = Grafix::RGBToUint32(glm::vec3(0.9f));

    switch (m_GameState)
    {
        case GameState::Ready:
        {
            pos.x += m_CanvasWidth / 2.0f - 340.0f;
            pos.y += 450.0f;
            uint32_t white = Grafix::RGBToUint32(glm::vec3(0.9f));
            ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 70.0f, pos, white, "Press Space to start");
            break;
        }
        case GameState::Playing:
        {
            pos.x += m_CanvasWidth - 210.0f;
            pos.y -= 30.0f;
            std::string scoreStr = std::format("Score: {}", m_Level->GetScore());
            ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 30.0f, pos, white, scoreStr.c_str());

            pos.y += 50.0f;
            std::string bulletStr = std::format("Bullet: {}", m_Level->GetPlayer().GetBulletCount());
            ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 30.0f, pos, white, bulletStr.c_str());
            break;
        }
        case GameState::GameOver:
        {
            pos.x += m_CanvasWidth / 2.0f - 340.0f;
            pos.y += 190.0f;
            uint32_t red = Grafix::RGBToUint32(glm::vec3(0.9f, 0.1f, 0.1f));
            ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 120.0f, pos, red, "Game Over");

            pos.x += 160.0f;
            pos.y += 150.0f;
            std::string scoreStr = std::format("Your score: {}", m_Level->GetScore());
            ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 40.0f, pos, white, scoreStr.c_str());

            pos.x -= 65.0f;
            pos.y += 120.0f;
            ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), 40.0f, pos, white, "Press Space to restart");
            break;
        }
    }
}

void GameLayer::OnEvent(Grafix::Event& e)
{
    Grafix::EventDispatcher dispatcher(e);
    dispatcher.Dispatch<Grafix::KeyPressedEvent>(BIND_EVENT_FN(GameLayer::OnKeyPressed));
}

void GameLayer::OnDetach()
{
    m_Camera = nullptr;

    m_Level->OnDetach();
    m_Level = nullptr;

    Grafix::Renderer::Shutdown();
}

bool GameLayer::OnKeyPressed(Grafix::KeyPressedEvent& e)
{
    if (e.IsRepeat())
        return false;

    if (e.GetKey() == Grafix::Key::Space)
    {
        if (m_GameState == GameState::GameOver)
            m_Level->Reset();

        m_GameState = GameState::Playing;
    }
    return false;
}

static glm::vec3 testColor;

void GameLayer::UI_Info()
{
    ImGui::Begin("Info");
    {
        ImGui::Text("FPS: %d", (int)(1.0f / Grafix::Application::Get().GetDeltaTime()));
        ImGui::Separator();

        glm::vec2 cameraPos = m_Camera->GetPosition();
        ImGui::Text("Camera Position: (%.3f, %.3f)", cameraPos.x, cameraPos.y);

        ImGui::Text("Your Score: %d", m_Level->GetScore());

        if (ImGui::Button("Play"))
        {
            switch (m_GameState)
            {
                case GameLayer::GameState::Ready:
                {
                    m_GameState = GameState::Playing;
                    break;
                }
                case GameLayer::GameState::GameOver:
                {
                    m_Level->Reset();
                    m_GameState = GameState::Playing;
                    break;
                }
            }
        }

        ImGui::ColorPicker3("Color", glm::value_ptr(testColor));
    }
    ImGui::End();
}
