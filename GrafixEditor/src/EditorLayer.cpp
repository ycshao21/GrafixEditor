#include "EditorLayer.h"

#include "Grafix/Renderer/Algorithms/ClippingAlgorithm.h"
#include "Grafix/Math/Math.h"

namespace Grafix
{
    static uint32_t s_MaxViewportSize = 16384;

    EditorLayer::EditorLayer() : Layer("Editor Layer"), m_Camera3D(45.0f, 0.1f, 100.0f) {}

    void EditorLayer::Render()
    {
        for (auto entity : m_ActiveScene->GetEntities())
        {
            int entityID = (int)(entt::entity)entity;
            auto& transform = entity.GetComponent<TransformComponent>();

            if (entity.HasComponent<LineComponent>())
            {
                // Clipping
                auto& line = entity.GetComponent<LineComponent>();
                if (m_HierarchyPanel.IsLineClipping())
                {
                    auto& clip = m_HierarchyPanel.GetLineClippingComponent();

                    std::vector<glm::vec2> newPoints;
                    if (clip.Algorithm == ClippingAlgorithmType::CohenSutherland)
                        newPoints = ClippingAlgorithm::CohenSutherland(line.P0, line.P1, clip.GetBottomLeft(), clip.GetTopRight());
                    else
                        newPoints = ClippingAlgorithm::Midpoint(line.P0, line.P1, clip.GetBottomLeft(), clip.GetTopRight());

                    LineComponent newLine(line);
                    newLine.P0 = newPoints[0];
                    newLine.P1 = newPoints[1];
                    m_Renderer.DrawLine(transform, newLine, entityID);
                }
                else
                {
                    m_Renderer.DrawLine(transform, line, entityID);
                }
            }
            else if (entity.HasComponent<CircleComponent>())
            {
                auto& circle = entity.GetComponent<CircleComponent>();
                m_Renderer.DrawCircle(transform, circle, entityID);
            }
            else if (entity.HasComponent<ArcComponent>())
            {
                auto& arc = entity.GetComponent<ArcComponent>();
                m_Renderer.DrawArc(transform, arc, entityID);
            }
            else if (entity.HasComponent<PolygonComponent>())
            {
                auto& polygon = entity.GetComponent<PolygonComponent>();
                if (!polygon.IsClosed)
                    continue;

                if (m_HierarchyPanel.IsPolyClipping())
                {
                    auto& clip = m_HierarchyPanel.GetPolyClippingComponent();
                    std::vector<glm::vec2> newPoints = ClippingAlgorithm::SutherlandHodgman(polygon.Vertices, clip.Vertices);
                    m_Renderer.DrawPolygon(transform, newPoints, polygon.Color, entityID);
                }
                else
                {
                    m_Renderer.DrawPolygon(transform, polygon, entityID);
                }
            }
            else if (entity.HasComponent<CurveComponent>())
            {
                auto& curve = entity.GetComponent<CurveComponent>();
                m_Renderer.DrawCurve(transform, curve, entityID);
            }
            else if (entity.HasComponent<FillComponent>())
            {
                auto& seed = entity.GetComponent<FillComponent>();
                m_Renderer.Fill(seed.Point, seed.Color);
            }
        }

        // Aux lines
        if (m_HierarchyPanel.IsTransforming())
        {
            TransformComponent transform = m_HierarchyPanel.GetTransformComponent();

            transform.Scale = { 1.0f, 1.0f };
            auto& pivot = transform.Pivot;

            auto translation = Math::CalcTranslationMatrix(transform.Translation);
            auto translatedPivot = Math::Transform(translation, pivot);

            m_Renderer.DrawLine(transform, pivot, { pivot.x + 18.0f, pivot.y }, glm::vec3(0.9f, 0.2f, 0.2f));
            m_Renderer.DrawLine(transform, pivot, { pivot.x, pivot.y + 18.0f }, glm::vec3(0.2f, 0.9f, 0.2f));
            return;
        }

        if (m_HierarchyPanel.IsLineClipping())
        {
            auto& clip = m_HierarchyPanel.GetLineClippingComponent();
            m_Renderer.DrawRect(clip.GetBottomLeft(), clip.GetTopRight(), m_AuxColor);
            return;
        }

        if (m_HierarchyPanel.IsPolyClipping())
        {
            auto& clip = m_HierarchyPanel.GetPolyClippingComponent();
            for (int i = 0; i < clip.Vertices.size() - 1; i++)
                m_Renderer.DrawLine(clip.Vertices[i], clip.Vertices[i + 1], m_AuxColor, 1.0f, LineStyleType::Dashed);

            m_Renderer.DrawLine(clip.Vertices.back(), clip.Vertices.front(), m_AuxColor, 1.0f, LineStyleType::Dashed);
            return;
        }

        for (auto entity : m_HierarchyPanel.GetSelectedEntities())
        {
            int entityID = (int)(entt::entity)entity;
            auto& transform = entity.GetComponent<TransformComponent>();

            if (entity.HasComponent<LineComponent>())
            {
                auto& line = entity.GetComponent<LineComponent>();
                m_Renderer.DrawSquare(line.P0, m_ControlPointWidth, m_ControlPointColor, entityID);
                m_Renderer.DrawSquare(line.P1, m_ControlPointWidth, m_ControlPointColor, entityID);
            }
            else if (entity.HasComponent<CircleComponent>())
            {
                auto& circle = entity.GetComponent<CircleComponent>();
                m_Renderer.DrawCross(transform, circle.Center, 5.0f, m_AuxColor, entityID);
            }
            else if (entity.HasComponent<ArcComponent>())
            {
                auto& arc = entity.GetComponent<ArcComponent>();
                m_Renderer.DrawCross(transform, arc.Center, 5.0f, m_AuxColor, entityID);

                float radiansAngle1 = glm::radians(arc.Angle1);
                float radiansAngle2 = glm::radians(arc.Angle2);
                glm::vec2 delta1 = arc.Radius * glm::vec2{ glm::cos(radiansAngle1), glm::sin(radiansAngle1) };
                glm::vec2 delta2 = arc.Radius * glm::vec2{ glm::cos(radiansAngle2), glm::sin(radiansAngle2) };
                m_Renderer.DrawLine(transform, arc.Center, arc.Center + delta1, m_AuxColor, 1.0f, LineStyleType::Dashed, LineAlgorithmType::Bresenham, entityID);
                m_Renderer.DrawLine(transform, arc.Center, arc.Center + delta2, m_AuxColor, 1.0f, LineStyleType::Dashed, LineAlgorithmType::Bresenham, entityID);
            }
            else if (entity.HasComponent<PolygonComponent>())
            {
                auto& polygon = entity.GetComponent<PolygonComponent>();

                for (int i = 0; i < polygon.Vertices.size() - 1; i++)
                    m_Renderer.DrawLine(transform, polygon.Vertices[i], polygon.Vertices[i + 1], m_AuxColor, 1.0f, LineStyleType::Solid, LineAlgorithmType::Bresenham, entityID);
                if (polygon.IsClosed)
                    m_Renderer.DrawLine(transform, polygon.Vertices.back(), polygon.Vertices.front(), m_AuxColor, 1.0f, LineStyleType::Solid, LineAlgorithmType::Bresenham, entityID);

                for (const auto& point : entity.GetComponent<PolygonComponent>().Vertices)
                    m_Renderer.DrawSquare(point, m_ControlPointWidth, m_ControlPointColor, entityID);
            }
            else if (entity.HasComponent<CurveComponent>())
            {
                auto& curve = entity.GetComponent<CurveComponent>();
                for (auto& point : curve.ControlPoints)
                    m_Renderer.DrawSquare(point, m_ControlPointWidth, m_ControlPointColor, entityID);
            }
        }
    }

    void EditorLayer::OnAttach()
    {
        // Default scene
        m_Scene2D = std::make_shared<Scene>();
        m_ActiveScene = m_Scene2D;
        m_HierarchyPanel.BindScene(m_ActiveScene);

        m_Scene3D = std::make_shared<Scene>();
        m_Scene3D->CreateEntity("Environment").AddComponent<EnvironmentComponent>();

        m_Renderer.OnResize(m_CanvasWidth, m_CanvasHeight);
        m_Camera.SetViewportSize((float)m_CanvasWidth, (float)m_CanvasHeight);
        m_Camera3D.OnResize(m_CanvasWidth, m_CanvasHeight);
    }

    void EditorLayer::OnUpdate(float ts)
    {
        UpdateMousePos();

        ////m_Camera.SetPosition(glm::vec2{ transform.Translation.x - 320.f, 0.0f });
        ////m_Camera.OnUpdate(ts);
        m_Camera3D.OnUpdate(ts);

        if (!m_HierarchyPanel.IsModalOpen())
        {
            switch (m_ToolState)
            {
            case ToolState::Move: { OnMoveToolUpdate(); break; }
            case ToolState::Line: { OnLineToolUpdate(); break; }
            case ToolState::Circle: { OnCircleToolUpdate(); break; }
            case ToolState::Arc: { OnArcToolUpdate(); break; }
            case ToolState::Fill: { OnFillToolUpdate(); break; }
            case ToolState::LineClip: { OnLineClipToolUpdate(); break; }
            case ToolState::PolyClip: { OnPolyClipToolUpdate(); break; }
            case ToolState::Polygon: { OnPolygonToolUpdate(); break; }
            case ToolState::Curve: { OnCurveUpdate(); break; }
            }
        }
        m_HierarchyPanel.OnUpdate();

        ////m_EditorScene->OnUpdate();
    }

    void EditorLayer::OnUIRender()
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

            // NOTE: Entity may have been created or destroyed during the OnUpdate() call,
            // but the pixel data is of the previous frame.
            // We need to render the current frame first, then read the pixel.
            if (m_ToolState != ToolState::Sphere)
            {
                m_Renderer.BeginScene(m_Camera);
                Render();
            }
            else
            {
                m_Renderer.DrawSphere(m_Camera3D, *m_ActiveScene);
            }
            m_Renderer.EndScene();

            if (IsMouseInCanvas())
            {
                int pixelData = m_Renderer.ReadPixel(m_MousePosInCanvas);
                m_HoveredEntity = pixelData < 0 ? Entity() : Entity((entt::entity)pixelData, m_ActiveScene.get());
            }

            UI_MenuBar();
            UI_Canvas();
            UI_Color();
            UI_Toolbar();
            UI_Info();
            m_HierarchyPanel.OnUIRender();
        }
        ImGui::End(); // DockSpace
    }

    void EditorLayer::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));
    }

    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        if (e.IsRepeat())
            return false;

        // Shortcuts
        bool control = Input::IsKeyPressed(Key::LeftControl) || Input::IsKeyPressed(Key::RightControl);
        bool shift = Input::IsKeyPressed(Key::LeftShift) || Input::IsKeyPressed(Key::RightShift);

        if (!m_IsDrawing)
        {
            switch (e.GetKey())
            {
            case Key::T:
            {
                if (control && m_HierarchyPanel.GetPanelState() == PanelState::Entity && m_HierarchyPanel.HasSelectedEntity())
                    m_HierarchyPanel.BeginTransformation();
                break;
            }
            }
        }
        return false;
    }

    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        return false;
    }

    void EditorLayer::UpdateMousePos()
    {
        auto [mx, my] = ImGui::GetMousePos();
        glm::vec2 newMousePosInCanvas = {
            mx - m_CanvasBounds[0].x,
            m_CanvasBounds[1].y - my
        };

        m_MousePositionDelta = newMousePosInCanvas - m_MousePosInCanvas;
        m_MousePosInCanvas = newMousePosInCanvas;

        glm::mat3 translationMatrix = m_Camera.GetTranslationMatrix();
        m_MousePosInWorld = Math::Transform(translationMatrix,m_MousePosInCanvas);
    }

    bool EditorLayer::IsMouseInCanvas() const
    {
        return m_MousePosInCanvas.x >= 0.0f && m_MousePosInCanvas.x < (float)m_CanvasWidth
            && m_MousePosInCanvas.y >= 0.0f && m_MousePosInCanvas.y < (float)m_CanvasHeight;
    }

    // -------------------------------------------------------------------
    // ------------------------------ Tools ------------------------------
    // -------------------------------------------------------------------

    void EditorLayer::OnMoveToolUpdate()
    {
        if (!IsMouseInCanvas() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            return;

        if (!m_HierarchyPanel.HasSelectedEntity())
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                m_HierarchyPanel.SwitchSelectedEntity(m_HoveredEntity);
            return;
        }

        if (m_HierarchyPanel.IsTransforming())
            return;

        if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))  // Check if the mouse is clicked near a control point
        {
            if (m_HierarchyPanel.GetNumOfSelectedEntities() == 1)
            {
                auto selectedEntity = *m_HierarchyPanel.GetSelectedEntities().begin();
                if (selectedEntity.HasComponent<LineComponent>())
                {
                    auto& line = selectedEntity.GetComponent<LineComponent>();
                    if (glm::distance(m_MousePosInWorld, line.P0) <= m_ControlPointWidth / 2.0f)
                        m_SelectedControlPoint = &line.P0;
                    else if (glm::distance(m_MousePosInWorld, line.P1) <= m_ControlPointWidth / 2.0f)
                        m_SelectedControlPoint = &line.P1;
                }
                else if (selectedEntity.HasComponent<PolygonComponent>())
                {
                    auto& polygon = selectedEntity.GetComponent<PolygonComponent>();
                    for (auto& vertex : polygon.Vertices)
                    {
                        if (glm::distance(m_MousePosInWorld, vertex) <= m_ControlPointWidth / 2.0f)
                        {
                            m_SelectedControlPoint = &vertex;
                            break;
                        }
                    }
                }
                else if (selectedEntity.HasComponent<CurveComponent>())
                {
                    auto& curve = selectedEntity.GetComponent<CurveComponent>();
                    for (auto& controlPoint : curve.ControlPoints)
                    {
                        if (glm::distance(m_MousePosInWorld, controlPoint) <= m_ControlPointWidth / 2.0f)
                        {
                            m_SelectedControlPoint = &controlPoint;
                            break;
                        }
                    }
                }
            }

            if (!m_SelectedControlPoint)
            {
                if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
                    m_HierarchyPanel.ToggleSelectedEntity(m_HoveredEntity);
                else
                    m_HierarchyPanel.SwitchSelectedEntity(m_HoveredEntity);
            }
        }
        else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            m_SelectedControlPoint = nullptr;
        }
        else
        {
            if (m_SelectedControlPoint)
                *m_SelectedControlPoint += m_MousePositionDelta;
        }
    }

    void EditorLayer::OnLineToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_ActiveScene->CreateEntity("Line");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

                auto& line = entity.AddComponent<LineComponent>();
                line.P0 = m_MousePosInWorld;
                line.P1 = m_MousePosInWorld;
                line.Color = m_PickedColor;
            }
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                m_HierarchyPanel.SwitchSelectedEntity({});
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& line = entity.GetComponent<LineComponent>();

            // If right mouse button is pressed, cancel drawing
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(entity);
                m_HierarchyPanel.SwitchSelectedEntity({});
                return;
            }
            line.P1 = m_MousePosInWorld;

            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                m_IsDrawing = false;
        }
    }

    void EditorLayer::OnCircleToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_ActiveScene->CreateEntity("Circle");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

                auto& circle = entity.AddComponent<CircleComponent>();
                circle.Center = m_MousePosInWorld;
                circle.Color = m_PickedColor;
            }
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                m_HierarchyPanel.SwitchSelectedEntity({});
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& circle = entity.GetComponent<CircleComponent>();

            // If right mouse button is pressed, cancel drawing
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(entity);
                m_HierarchyPanel.SwitchSelectedEntity({});
                return;
            }

            circle.Radius = glm::distance(circle.Center, m_MousePosInWorld);

            // Confirm
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                m_IsDrawing = false;
        }
    }

    void EditorLayer::OnArcToolUpdate()
    {
        static int operationState = 0;  // 0: Radius, 1: Angle

        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;
                operationState = 0;

                Entity entity = m_ActiveScene->CreateEntity("Arc");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

                auto& arc = entity.AddComponent<ArcComponent>();
                arc.Center = m_MousePosInWorld;
                arc.Radius = 0.0f;
                arc.Color = m_PickedColor;
            }
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                m_HierarchyPanel.SwitchSelectedEntity({});
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& arc = entity.GetComponent<ArcComponent>();

            // If right mouse button is pressed, cancel drawing
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(entity);
                m_HierarchyPanel.SwitchSelectedEntity({});
                return;
            }

            if (operationState == 0)
            {
                float radius = glm::distance(arc.Center, m_MousePosInWorld);

                float dx = m_MousePosInWorld.x - arc.Center.x;
                float dy = m_MousePosInWorld.y - arc.Center.y;
                float angle = glm::degrees(glm::atan(dy, dx));

                arc.Radius = radius;
                arc.Angle1 = angle;
                arc.Angle2 = angle;

                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                    ++operationState;
            }
            else
            {
                float dx = m_MousePosInWorld.x - arc.Center.x;
                float dy = m_MousePosInWorld.y - arc.Center.y;
                float angle = glm::degrees(glm::atan(dy, dx));
                arc.Angle2 = angle;

                // Press Ctrl key to draw a major arc
                arc.Major = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);

                // Confirm
                if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    m_IsDrawing = false;
            }
        }
    }

    void EditorLayer::OnFillToolUpdate()
    {
        if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            Entity entity = m_ActiveScene->CreateEntity("Fill");
            m_HierarchyPanel.SwitchSelectedEntity(entity);

            auto& fill = entity.AddComponent<FillComponent>();
            fill.Point = m_MousePosInCanvas;
            fill.Color = m_PickedColor;
        }
    }

    void EditorLayer::OnLineClipToolUpdate()
    {
        auto view = m_ActiveScene->GetEntitiesWith<LineComponent>();
        if (view.empty())
            return;

        // If right mouse button is pressed, cancel drawing
        if (m_HierarchyPanel.IsLineClipping() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            m_HierarchyPanel.EndLineClipping(false);
            m_IsDrawing = false;
            return;
        }

        auto& clip = m_HierarchyPanel.GetLineClippingComponent();
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;
                m_HierarchyPanel.BeginLineClipping();
                clip.P0 = m_MousePosInWorld;
                clip.P1 = m_MousePosInWorld;
            }
        }
        else
        {
            clip.P1 = m_MousePosInWorld;
            if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                m_IsDrawing = false;
        }
    }

    void EditorLayer::OnPolyClipToolUpdate()
    {
        auto view = m_ActiveScene->GetEntitiesWith<PolygonComponent>();
        if (view.empty())
            return;

        // If right mouse button is pressed, cancel drawing
        if (m_HierarchyPanel.IsPolyClipping() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            m_HierarchyPanel.EndPolyClipping(false);
            m_IsDrawing = false;
            return;
        }

        auto& clip = m_HierarchyPanel.GetPolyClippingComponent();
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                GF_INFO("First clip vertex");
                m_IsDrawing = true;
                m_HierarchyPanel.BeginPolyClipping();
                clip.Vertices = { m_MousePosInWorld };
            }
        }
        else
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                clip.Vertices.push_back(m_MousePosInWorld);

            if (ImGui::IsKeyPressed(ImGuiKey_Enter))
                m_IsDrawing = false;
        }
    }

    void EditorLayer::OnPolygonToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_ActiveScene->CreateEntity("Polygon");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

                auto& polygon = entity.AddComponent<PolygonComponent>();
                polygon.Vertices = { m_MousePosInWorld, m_MousePosInWorld };
                polygon.Color = m_PickedColor;
                polygon.IsClosed = false;
            }
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                m_HierarchyPanel.SwitchSelectedEntity({});
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& polygon = entity.GetComponent<PolygonComponent>();

            // If right mouse button is pressed, cancel drawing
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_ActiveScene->RemoveEntity(entity);
                m_HierarchyPanel.SwitchSelectedEntity({});
                return;
            }

            // Update the last vertex
            glm::vec2& currentVertex = polygon.Vertices.back();
            if (glm::distance(polygon.Vertices.front(), m_MousePosInWorld) < m_ControlPointWidth)
                currentVertex = polygon.Vertices.front();
            else
                currentVertex = m_MousePosInWorld;

            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                // Confirm
                if (currentVertex == polygon.Vertices.front())
                {
                    polygon.Vertices.pop_back();
                    polygon.IsClosed = true;
                    m_IsDrawing = false;
                }
                else
                {
                    polygon.Vertices.push_back(currentVertex);
                }
            }
        }
    }

    void EditorLayer::OnCurveUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_ActiveScene->CreateEntity("Curve");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

                auto& curve = entity.AddComponent<CurveComponent>();
                curve.ControlPoints.push_back(m_MousePosInWorld);
                curve.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& curve = entity.GetComponent<CurveComponent>();

            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                curve.ControlPoints.push_back(m_MousePosInWorld);
        }

        if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            m_IsDrawing = false;
            m_HierarchyPanel.SwitchSelectedEntity({});
        }
    }

    // -----------------------------------------------------------------
    // --------------------------- UI Panels ---------------------------
    // -----------------------------------------------------------------

    void EditorLayer::UI_MenuBar()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Exit"))
                    Application::Get().Close();

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Edit"))
            {
                if (ImGui::MenuItem("Transform", nullptr, false, m_HierarchyPanel.HasSelectedEntity()))
                    m_HierarchyPanel.BeginTransformation();

                if (ImGui::MenuItem("Clear", nullptr, false))
                {
                    m_ActiveScene->Clear();
                    m_HierarchyPanel.SwitchSelectedEntity({});
                }

                ImGui::EndMenu();
            }

            ImGui::EndMenuBar();
        }
    }

    void EditorLayer::UI_Canvas()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("Canvas");
        {
            ImVec2 canvasMinRegion = ImGui::GetWindowContentRegionMin();
            ImVec2 canvasMaxRegion = ImGui::GetWindowContentRegionMax();
            ImVec2 canvasOffset = ImGui::GetWindowPos();
            m_CanvasBounds[0] = { canvasMinRegion.x + canvasOffset.x, canvasMinRegion.y + canvasOffset.y };  // Top-left
            m_CanvasBounds[1] = { m_CanvasBounds[0].x + m_CanvasWidth, m_CanvasBounds[0].y + m_CanvasHeight };  // Bottom-right

            m_CanvasFocused = ImGui::IsWindowFocused() && IsMouseInCanvas();
            m_CanvasHovered = ImGui::IsWindowHovered() && IsMouseInCanvas();
            Application::Get().GetImGuiLayer()->BlockEvents(!m_CanvasFocused);

            if (m_CanvasWidth > 0 && m_CanvasWidth <= s_MaxViewportSize && m_CanvasHeight > 0 && m_CanvasHeight <= s_MaxViewportSize)
            {
                if (auto image = m_Renderer.GetImage())
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

    void EditorLayer::UI_Toolbar()
    {
        ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoResize);
        {
            auto ToolButtonFn = [this](const char* name, ToolState toolState, const char* desc)
                {
                    if (ImGui::Button(name, ImVec2{ 80.0f, 30.0f }))
                    {
                        if (m_IsDrawing || m_ToolState == toolState || m_HierarchyPanel.IsTransforming())
                            return;

                        GF_INFO("Switched to {0} tool.", name);
                        m_ToolState = toolState;

                        if (m_ToolState == ToolState::Sphere)
                        {
                            SwitchScene(m_Scene3D);
                            return;
                        }

                        SwitchScene(m_Scene2D);
                        if (m_ToolState == ToolState::LineClip)
                            m_HierarchyPanel.SetPanelState(PanelState::LineClip);
                        else if (m_ToolState == ToolState::PolyClip)
                            m_HierarchyPanel.SetPanelState(PanelState::PolyClip);
                        else
                            m_HierarchyPanel.SetPanelState(PanelState::Entity);

                        // Remove all the fill entities
                        auto view = m_ActiveScene->GetEntitiesWith<FillComponent>();
                        for (auto e : view)
                            m_ActiveScene->RemoveEntity(Entity(e, m_ActiveScene.get()));
                    }
                    ImGui::SetItemTooltip(desc);
                };

            ToolButtonFn("Move", ToolState::Move, "Select entities and move control points.");
            ImGui::SameLine();
            ToolButtonFn("Line", ToolState::Line, "Drag to draw a line.");
            ImGui::SameLine();
            ToolButtonFn("Circle", ToolState::Circle, "Drag to draw a circle.");
            ImGui::SameLine();
            ToolButtonFn("Arc", ToolState::Arc, "Drag first to decide the center, radius and the start angle, then click again to decide the end angle.");
            ImGui::SameLine();
            ToolButtonFn("Fill", ToolState::Fill, "Click to fill an area.");
            ImGui::SameLine();
            ToolButtonFn("Line Clip", ToolState::LineClip, "Drag to draw a rectangle to clip all the lines.");
            ImGui::SameLine();
            ToolButtonFn("PolyClip", ToolState::PolyClip, "Drag to draw a polygon to clip all the polygons.");
            ImGui::SameLine();
            ToolButtonFn("Polygon", ToolState::Polygon, "Drag to draw a polygon.");
            ImGui::SameLine();
            ToolButtonFn("Curve", ToolState::Curve, "Drag to draw a curve.");
            ImGui::SameLine();
            ToolButtonFn("Sphere", ToolState::Sphere, "Switch to 3D scene and draw a sphere.");
            ImGui::SameLine();

            const char* toolNames[10] = { "Move", "Line", "Circle", "Arc", "Fill", "Line Clip", "PolyClip", "Polygon", "Curve", "Sphere" };
            ImGui::Text("Current Tool: %s", toolNames[(int)m_ToolState]);
        }
        ImGui::End();
    }

    void EditorLayer::UI_Info()
    {
        ImGui::Begin("Info");
        ////ImGui::Text("Last Frame Time: %.5fs", Application::Get().GetLastFrameTime());

        ////ImGui::Separator();

        glm::vec2 cameraPos = m_Camera.GetPosition();
        ImGui::Text("Camera Position: (%.3f, %.3f)", cameraPos.x, cameraPos.y);
        if (m_CanvasHovered)
            ImGui::Text("Mouse Position In Canvas: (%d, %d)", (int)m_MousePosInCanvas.x, (int)m_MousePosInCanvas.y);
        ImGui::End();  // Info
    }

    void EditorLayer::UI_Color()
    {
        ImGui::Begin("Color");
        ImGui::ColorPicker3("Color", glm::value_ptr(m_PickedColor));
        ImGui::End();  // Color
    }

    void EditorLayer::SwitchScene(const std::shared_ptr<Scene>& scene)
    {
        if (m_ActiveScene != scene)
        {
            m_ActiveScene = scene;
            m_HierarchyPanel.BindScene(m_ActiveScene);
        }
        m_HierarchyPanel.SwitchSelectedEntity({});
    }
}