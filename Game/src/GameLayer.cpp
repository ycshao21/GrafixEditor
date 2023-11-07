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
    auto playerPos = m_Level->GetPlayer().GetPosition();
    m_Camera->SetPosition({ playerPos.x + 250.0f, playerPos.y });

    if (m_Level->IsGameOver())
    {
        m_GameState = GameState::GameOver;
        return;
    }

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
    windowFlags |= ImGuiWindowFlags_MenuBar;

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

        UI_MenuBar();
        UI_Info();
        UI_Canvas();
    }
    ImGui::End(); // DockSpace
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

    if (e.GetKey() == Grafix::Key::Up)
    {
        if (m_GameState == GameState::GameOver)
            m_Level->Reset();

        m_GameState = GameState::Playing;
    }
    return false;
}

// -----------------------------------------------------------------
// --------------------------- UI Panels ---------------------------
// -----------------------------------------------------------------

void GameLayer::UI_Canvas()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    ImGui::Begin("Canvas");
    {
        ////ImVec2 canvasMinRegion = ImGui::GetWindowContentRegionMin();
        ////ImVec2 canvasMaxRegion = ImGui::GetWindowContentRegionMax();
        ////ImVec2 canvasOffset = ImGui::GetWindowPos();
        ////m_CanvasBounds[0] = { canvasMinRegion.x + canvasOffset.x, canvasMinRegion.y + canvasOffset.y };  // Top-left
        ////m_CanvasBounds[1] = { m_CanvasBounds[0].x + m_CanvasWidth, m_CanvasBounds[0].y + m_CanvasHeight };  // Bottom-right

        ////m_CanvasFocused = ImGui::IsWindowFocused() && IsMouseInCanvas();
        ////m_CanvasHovered = ImGui::IsWindowHovered() && IsMouseInCanvas();

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
    }
    ImGui::End();
    ImGui::PopStyleVar();
}

void GameLayer::UI_MenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
                Grafix::Application::Get().Close();

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
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
        ////if (m_CanvasHovered)
        ////    ImGui::Text("Mouse Position In Canvas: (%d, %d)", (int)m_MousePosInCanvas.x, (int)m_MousePosInCanvas.y);

        ImGui::Text("Your Score: %d", m_Level->GetScore());
        ImGui::Text("Bullet Remaining: %d", m_Level->GetPlayer().GetBulletCount());

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
