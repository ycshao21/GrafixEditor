#include "EditorLayer.h"

#include "Grafix/Renderer/Algorithms/ClippingAlgorithm.h"
#include "Grafix/Math/Math.h"

namespace Grafix
{
	static uint32_t s_MaxViewportSize = 16384;

	EditorLayer::EditorLayer() : Layer("Editor Layer"), m_Camera3D(45.0f, 0.1f, 100.0f) {}

	void EditorLayer::Render()
	{
		for (auto entity : m_EditorScene->GetEntities())
		{
			int entityID = (int)(entt::entity)entity;
			auto& transform = entity.GetComponent<TransformComponent>();

            if (entity.HasComponent<LineComponent>())
            {
                auto& line = entity.GetComponent<LineComponent>();
                m_Renderer.DrawLine(transform, line, entityID);
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
                if (polygon.IsClosed)
                    m_Renderer.DrawPolygon(transform, polygon, entityID);
            }
            else if (entity.HasComponent<FillComponent>())  // TEMP
            {
                auto& seed = entity.GetComponent<FillComponent>();
                m_Renderer.Fill(seed.FillPoint, seed.FillColor);
            }
            else if (entity.HasComponent<CurveComponent>())
            {
                auto& curve = entity.GetComponent<CurveComponent>();
                m_Renderer.DrawCurve(transform, curve, entityID);
            }
        }

        if (m_HierarchyPanel.IsTransforming())
        {
            m_Renderer.DrawCross(m_HierarchyPanel.GetPivot(), 5.0f, m_AuxColor);
            return;
        }

        // Aux lines
        for (auto entity : m_HierarchyPanel.GetSelectedEntities())
        {
            int entityID = (int)(entt::entity)entity;
            auto& transform = entity.GetComponent<TransformComponent>();

            if (entity.HasComponent<LineComponent>())
            {
                auto& line = entity.GetComponent<LineComponent>();
                m_Renderer.DrawSquare(line.P0, m_ControlPointWidth, m_AuxColor, entityID);
                m_Renderer.DrawSquare(line.P1, m_ControlPointWidth, m_AuxColor, entityID);
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
                    m_Renderer.DrawSquare(point, m_ControlPointWidth, m_AuxColor, entityID);
            }
            else if (entity.HasComponent<CurveComponent>())
            {
                auto& curve = entity.GetComponent<CurveComponent>();
                for (auto& point : curve.ControlPoints)
                    m_Renderer.DrawSquare(point, m_ControlPointWidth, m_AuxColor, entityID);
            }
        }
    }

	void EditorLayer::OnAttach()
	{
		m_EditorScene = std::make_shared<Scene>();
		m_HierarchyPanel.BindScene(m_EditorScene);

		m_Renderer.OnResize(m_CanvasWidth, m_CanvasHeight);
		m_Camera.SetViewportSize((float)m_CanvasWidth, (float)m_CanvasHeight);
		m_Camera3D.OnResize(m_CanvasWidth, m_CanvasHeight);
	}

	void EditorLayer::OnUpdate()
	{
		UpdateMousePos();

		////m_Camera.SetPosition(glm::vec2{ transform.Translation.x - 320.f, 0.0f });
		////m_Camera.OnUpdate();

		m_Camera3D.OnUpdate(0.1f);

        if (!m_HierarchyPanel.IsTransforming() && !m_HierarchyPanel.IsModalOpen())
        {
            switch (m_ToolState)
            {
            case ToolState::Move: { OnMoveToolUpdate(); break; }
            case ToolState::Line: { OnLineToolUpdate(); break; }
            case ToolState::Circle: { OnCircleToolUpdate(); break; }
            case ToolState::Arc: { OnArcToolUpdate(); break; }
            case ToolState::Fill: { OnFillToolUpdate(); break; }
            case ToolState::Clip: { OnClipToolUpdate(); break; }
            case ToolState::Polygon: { OnPolygonToolUpdate(); break; }
            case ToolState::Curve: { OnCurveUpdate(); break; }
            case ToolState::Sphere: { OnSphereUpdate(); break; }
            }
        }

        m_HierarchyPanel.OnUpdate();

        m_EditorScene->OnUpdate();
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
			if (m_ToolState != ToolState::Sphere) {
				m_Renderer.BeginScene(m_Camera);
				Render();
				m_Renderer.EndScene();
			}
			else {
				auto entity = m_EditorScene->GetEntities().front();
				auto& sphere = entity.GetComponent<SphereComponent>();
				m_Renderer.SphereRender(m_Camera3D, sphere.EnvirDirection, sphere.Flag, sphere.P);
			}

			if (IsMouseInCanvas())
			{
				int pixelData = m_Renderer.ReadPixel(m_MousePosInCanvas);
				m_HoveredEntity = pixelData < 0 ? Entity() : Entity((entt::entity)pixelData, m_EditorScene.get());
			}

            ImGui::ShowDemoWindow();

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
                if (control && m_HierarchyPanel.HasSelectedEntity())
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
		m_MousePosInWorld = Math::Transform(translationMatrix, m_MousePosInCanvas);
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

                Entity entity = m_EditorScene->CreateEntity("Line");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

                auto& line = entity.AddComponent<LineComponent>();
                line.P0 = m_MousePosInWorld;
                line.P1 = m_MousePosInWorld;
                line.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& line = entity.GetComponent<LineComponent>();

            // If right mouse button is pressed, cancel drawing
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SwitchSelectedEntity({});
                return;
            }

			// Press Shift key to draw horizontal/vertical lines
			if (ImGui::IsKeyDown(ImGuiKey_LeftShift) || ImGui::IsKeyDown(ImGuiKey_RightShift))
			{
				if (std::abs(m_MousePosInWorld.x - line.P0.x) < std::abs(m_MousePosInWorld.y - line.P0.y))
					line.P1 = { line.P0.x, m_MousePosInWorld.y };
				else
					line.P1 = { m_MousePosInWorld.x, line.P0.y };
			}
			else
			{
				line.P1 = m_MousePosInWorld;
			}

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				m_IsDrawing = false;

                // If the line is too short, remove it
                if (glm::distance(line.P0, line.P1) < 0.1f)
                {
                    m_EditorScene->RemoveEntity(entity);
                    m_HierarchyPanel.SwitchSelectedEntity({});
                }
            }
        }
    }

    void EditorLayer::OnArcToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;
                m_OperationState = 0;

                Entity entity = m_EditorScene->CreateEntity("Arc");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

				auto& arc = entity.AddComponent<ArcComponent>();

                arc.Center = m_MousePosInWorld;
                arc.Radius = 0.0f;
                arc.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& arc = entity.GetComponent<ArcComponent>();

            // If right mouse button is pressed, cancel drawing
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SwitchSelectedEntity({});
                return;
            }

			switch (m_OperationState)
			{
			case 0:
			{
				float radius = glm::distance(arc.Center, m_MousePosInWorld);

				float dx = m_MousePosInWorld.x - arc.Center.x;
				float dy = m_MousePosInWorld.y - arc.Center.y;
				float angle = glm::degrees(glm::atan(dy, dx));

				arc.Radius = radius;
				arc.Angle1 = angle;
				arc.Angle2 = angle;

                if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
                {
                    // If the radius is too small, remove the arc
                    if (radius < 0.1f)
                    {
                        m_IsDrawing = false;
                        m_EditorScene->RemoveEntity(entity);
                        m_HierarchyPanel.SwitchSelectedEntity({});
                    }
                    else
                    {
                        ++m_OperationState;
                    }
                }
                break;
            }
            case 1:
            {
                float dx = m_MousePosInWorld.x - arc.Center.x;
                float dy = m_MousePosInWorld.y - arc.Center.y;
                float angle = glm::degrees(glm::atan(dy, dx));
                arc.Angle2 = angle;

				// Press Ctrl key to draw a major arc
				if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl))
					arc.Major = true;
				else
					arc.Major = false;

                // Confirm
                if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                    m_IsDrawing = false;
                break;
            }
            }
        }
    }

    void EditorLayer::OnCircleToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_EditorScene->CreateEntity("Circle");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

				auto& circle = entity.AddComponent<CircleComponent>();

                circle.Center = m_MousePosInWorld;
                circle.Color = m_PickedColor;
            }
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& circle = entity.GetComponent<CircleComponent>();

            // If right mouse button is pressed, cancel drawing
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SwitchSelectedEntity({});
                return;
            }

			circle.Radius = glm::distance(circle.Center, m_MousePosInWorld);

			// Confirm
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				m_IsDrawing = false;

                // If the radius is too small, remove the circle
                if (circle.Radius < 0.1f)
                {
                    m_EditorScene->RemoveEntity(entity);
                    m_HierarchyPanel.SwitchSelectedEntity({});
                }
            }
        }
    }

    void EditorLayer::OnFillToolUpdate()
    {
        if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            Entity entity = m_EditorScene->CreateEntity("Fill");
            m_HierarchyPanel.SwitchSelectedEntity(entity);

			auto& bucket = entity.AddComponent<FillComponent>();
			bucket.FillPoint = m_MousePosInCanvas;
			bucket.FillColor = m_PickedColor;
		}
	}

    void EditorLayer::OnClipToolUpdate()
    {
        ////if (!m_IsDrawing)
        ////{
        ////    if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        ////    {
        ////        m_IsDrawing = true;

        ////        Entity entity = m_EditorScene->CreateEntity("Clip");
        ////        m_HierarchyPanel.SetSelectedEntity(entity);

        ////        auto& clip = entity.AddComponent<ClipComponent>();

        ////        clip.P0 = m_MousePosInWorld;
        ////        clip.P1 = m_MousePosInWorld;
        ////        ////m_Renderer.SetClipRange(clip.P0, clip.P1);
        ////    }
        ////}
        ////else
        ////{
        ////    Entity entity = m_HierarchyPanel.GetSelectedEntity();
        ////    auto& clip = entity.GetComponent<ClipComponent>();

        ////    // If right mouse button is pressed, cancel drawing
        ////    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        ////    {
        ////        m_IsDrawing = false;
        ////        m_EditorScene->RemoveEntity(entity);
        ////        m_HierarchyPanel.SetSelectedEntity({});
        ////        return;
        ////    }

        ////    clip.P1 = m_MousePosInWorld;
        ////    ////m_Renderer.SetClipRange(clip.P0, clip.P1);

        ////    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        ////    {
        ////        m_IsDrawing = false;

        ////        if (glm::distance(clip.P0, clip.P1) < 0.1f)
        ////        {
        ////            m_EditorScene->RemoveEntity(entity);
        ////            m_HierarchyPanel.SetSelectedEntity({});
        ////            ////m_Renderer.SetClipRange(glm::vec2(0.0f, 0.0f), glm::vec2(m_CanvasWidth, m_CanvasHeight));
        ////        }
        ////    }
        ////}
    }

    void EditorLayer::OnPolygonToolUpdate()
    {
        if (!m_IsDrawing)
        {
            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                m_IsDrawing = true;

                Entity entity = m_EditorScene->CreateEntity("Polygon");
                m_HierarchyPanel.SwitchSelectedEntity(entity);

                auto& polygon = entity.AddComponent<PolygonComponent>();
                polygon.Vertices = { m_MousePosInWorld, m_MousePosInWorld };
                polygon.Color = m_PickedColor;
                polygon.IsClosed = false;
            }
        }
        else
        {
            Entity entity = *m_HierarchyPanel.GetSelectedEntities().begin();
            auto& polygon = entity.GetComponent<PolygonComponent>();

            // If right mouse button is pressed, cancel drawing
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
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
                    if (polygon.Vertices.size() <= 3)
                    {
                        m_EditorScene->RemoveEntity(entity);
                        m_HierarchyPanel.SwitchSelectedEntity({});
                    }
                    else
                    {
                        polygon.Vertices.pop_back();
                        polygon.IsClosed = true;
                    }
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

                Entity entity = m_EditorScene->CreateEntity("Curve");
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

            // If right mouse button is pressed, cancel drawing
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
            {
                m_IsDrawing = false;
                m_EditorScene->RemoveEntity(entity);
                m_HierarchyPanel.SwitchSelectedEntity({});
                return;
            }

            if (m_CanvasHovered && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                curve.ControlPoints.push_back(m_MousePosInWorld);

			if (ImGui::IsKeyPressed(ImGuiKey_Enter))
			{
				m_IsDrawing = false;

                if (curve.ControlPoints.size() <= 1)
                {
                    m_EditorScene->RemoveEntity(entity);
                    m_HierarchyPanel.SwitchSelectedEntity({});
                }
            }
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
                    m_EditorScene->Clear();
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
			else
			{
				GF_WARN("Attempt to resize viewport to ({0}, {1})", m_CanvasWidth, m_CanvasHeight);
			}
		}
		ImGui::End();
		ImGui::PopStyleVar();
	}

    void EditorLayer::UI_Toolbar()
    {
        ImGui::Begin("Tools", nullptr, ImGuiWindowFlags_NoResize);
        {
            auto ButtonFn = [this](const char* name, ToolState toolState, const char* desc)
                {
                    if (ImGui::Button(name, ImVec2{ 80.0f, 30.0f }))
                    {
                        if (m_ToolState == toolState)
                            return;

                        GF_INFO("Switched to {0} tool.", name);
                        ImGui::SetItemDefaultFocus();
                        m_ToolState = toolState;
                        m_IsDrawing = false;
                        m_HierarchyPanel.SwitchSelectedEntity({});

                        if (m_ToolState == ToolState::Sphere)
                        {
                            m_HierarchyPanel.SwitchSelectedEntity({});
                            m_EditorScene->Clear();

                            Entity entity = m_EditorScene->CreateEntity("Sphere");
                            m_HierarchyPanel.SwitchSelectedEntity(entity);
                            auto& sphere = entity.AddComponent<SphereComponent>();
						}
                    }

                    ImGui::SetItemTooltip(desc);
                };

            ButtonFn("Move", ToolState::Move, "Select entities and move control points.");
            ImGui::SameLine();
            ButtonFn("Line", ToolState::Line, "Drag to draw a line.");
            ImGui::SameLine();
            ButtonFn("Circle", ToolState::Circle, "Drag to draw a circle.");
            ImGui::SameLine();
            ButtonFn("Arc", ToolState::Arc, "Drag first to decide the center, radius and the start angle, then click again to decide the end angle.");
            ImGui::SameLine();
            ButtonFn("Fill", ToolState::Fill, "Click to fill an area.");
            ImGui::SameLine();
            ButtonFn("Clip", ToolState::Clip, "Drag to draw a rectangle to clip.");
            ImGui::SameLine();
            ButtonFn("Polygon", ToolState::Polygon, "Click on each vertex to draw a polygon.");
            ImGui::SameLine();
            ButtonFn("Curve", ToolState::Curve, "Click on each control point to draw a curve.");
            ImGui::SameLine();
            ButtonFn("Sphere", ToolState::Sphere, "Drag to draw a sphere.");
        }
        ImGui::End();
    }

    void EditorLayer::UI_Info()
    {
        ImGui::Begin("Info");
        ImGui::Text("Last Frame Time: %.5fs", Application::Get().GetLastFrameTime());

		ImGui::Separator();

		std::string hoveredEntityName = "None";
		if (m_HoveredEntity)
			hoveredEntityName = m_HoveredEntity.GetTag();
		ImGui::Text("Hovered Entity: %s", hoveredEntityName.c_str());

        std::string selectedEntityName = "None";
        if (m_HierarchyPanel.GetNumOfSelectedEntities() == 1)
        {
            auto selectedEntity = *m_HierarchyPanel.GetSelectedEntities().begin();
            selectedEntityName = selectedEntity.GetTag();
        }
        else if (m_HierarchyPanel.GetNumOfSelectedEntities() > 1)
        {
            selectedEntityName = "Multiple";
        }
        ImGui::Text("Selected Entity: %s", selectedEntityName.c_str());

		ImGui::Separator();

        glm::vec2 cameraPos = m_Camera.GetPosition();
        ImGui::Text("Camera Position: (%.3f, %.3f)", cameraPos.x, cameraPos.y);

		if (IsMouseInCanvas())
		{
			ImGui::Text("Mouse Position In World: (%d, %d)", (int)m_MousePosInWorld.x, (int)m_MousePosInWorld.y);
			ImGui::Text("Mouse Position In Canvas: (%d, %d)", (int)m_MousePosInCanvas.x, (int)m_MousePosInCanvas.y);
		}
		ImGui::End();  // Info
	}

    void EditorLayer::UI_Color()
    {
        ImGui::Begin("Color");

        if (m_HierarchyPanel.GetNumOfSelectedEntities() == 1)
        {
            float* color = nullptr;  // glm::vec3
            Entity selectedEntity = *m_HierarchyPanel.GetSelectedEntities().begin();

            if (selectedEntity.HasComponent<LineComponent>())
                color = glm::value_ptr(selectedEntity.GetComponent<LineComponent>().Color);
            else if (selectedEntity.HasComponent<CircleComponent>())
                color = glm::value_ptr(selectedEntity.GetComponent<CircleComponent>().Color);
            else if (selectedEntity.HasComponent<ArcComponent>())
                color = glm::value_ptr(selectedEntity.GetComponent<ArcComponent>().Color);
            else if (selectedEntity.HasComponent<PolygonComponent>())
                color = glm::value_ptr(selectedEntity.GetComponent<PolygonComponent>().Color);
            else if (selectedEntity.HasComponent<CurveComponent>())
                color = glm::value_ptr(selectedEntity.GetComponent<CurveComponent>().Color);

            if (color)
            {
                m_PickedColor = glm::vec3(color[0], color[1], color[2]);
                ImGui::ColorPicker3("Color", color);
            }
        }
        else
        {
            ImGui::ColorPicker3("Color", glm::value_ptr(m_PickedColor));
        }

        ImGui::End();  // Color
    }
}