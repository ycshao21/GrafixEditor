#include "HierarchyPanel.h"
#include "misc/cpp/imgui_stdlib.h"
#include "misc/cpp/imgui_stdlib.cpp"

#include "Grafix/Renderer/Algorithms/ClippingAlgorithm.h"

namespace Grafix
{
    // *****************************************************************************************************************************************************************
    // Control Panel Templates
    // *****************************************************************************************************************************************************************

    static void DrawFloatControl(const std::string& label, float* value, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::DragFloat("##Value", value, speed, vMin, vMax, format);

        ImGui::Columns(1);

        ImGui::PopID();
    }

    static  void DrawFloat2Control(const std::string& label, float* x, float* y, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth() / 2.0f);

        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::DragFloat("##X", x, speed, vMin, vMax, format);

        ImGui::SameLine();

        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::DragFloat("##Y", y, speed, vMin, vMax, format);

        ImGui::PopItemWidth();
        ImGui::Columns(1);

        ImGui::PopID();
    }

    static void DrawFloat2Control(const std::string& label, glm::vec2& values, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f)
    {
        DrawFloat2Control(label, &values.x, &values.y, vMin, vMax, format, speed, columnWidth);
    }

    static  void DrawFloat3Control(const std::string& label, float* x, float* y, float* z, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f)
    {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushItemWidth(ImGui::CalcItemWidth() / 3.0f);

        ImGui::Text("X");
        ImGui::SameLine();
        ImGui::DragFloat("##X", x, speed, vMin, vMax, format);

        ImGui::SameLine();

        ImGui::Text("Y");
        ImGui::SameLine();
        ImGui::DragFloat("##Y", y, speed, vMin, vMax, format);

        ImGui::SameLine();

        ImGui::Text("Z");
        ImGui::SameLine();
        ImGui::DragFloat("##Z", z, speed, vMin, vMax, format);

        ImGui::PopItemWidth();
        ImGui::Columns(1);
        ImGui::PopID();
    }

    static  void DrawFloat3Control(const std::string& label, glm::vec3& values, float vMin = -FLT_MAX, float vMax = FLT_MAX, const char* format = "%.3f", float speed = 0.5f, float columnWidth = 100.0f)
    {
        DrawFloat3Control(label, &values.x, &values.y, &values.z, vMin, vMax, format, speed, columnWidth);
    }

    static  void DrawIntControl(const std::string& label, int* value, int vMin = -INT_MIN, int vMax = INT_MAX, float columnWidth = 100.0f)
    {
        ImGui::PushID(label.c_str());
        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::DragInt("##Value", value, 1, vMin, vMax);
        ImGui::Columns(1);
        ImGui::PopID();
    }

    static void DrawColorControl(glm::vec3& color)
    {
        ImGui::Text("Color");
        ImGui::SameLine();
        ImGui::ColorEdit3("##Color", glm::value_ptr(color));
    }

    // *****************************************************************************************************************************************************************
    // Hierarchy Panel
    // *****************************************************************************************************************************************************************

    void HierarchyPanel::SwitchSelectedEntity(Entity entity)
    {
        m_SelectedEntities.clear();

        if (entity)
            m_SelectedEntities.emplace(entity);
    }

    void HierarchyPanel::ToggleSelectedEntity(Entity entity)
    {
        if (!entity)
        {
            m_SelectedEntities.clear();
            return;
        }

        if (IsSelected(entity))
            m_SelectedEntities.erase(entity);
        else
            m_SelectedEntities.emplace(entity);
    }

    void HierarchyPanel::OnUpdate()
    {
        switch (m_PanelState)
        {
        case PanelState::Transform:
        {
            for (auto entity : m_SelectedEntities)
            {
                auto& transform = entity.GetComponent<TransformComponent>();
                transform = *m_TransformComponent;
            }
            break;
        }
        }
    }

    void HierarchyPanel::BeginTransformation()
    {
        GF_INFO("Begin transformation");
        m_PanelState = PanelState::Transform;
        m_MustKeepRatio = false;
        m_TransformComponent = std::make_shared<TransformComponent>();

        for (auto entity : m_SelectedEntities)
        {
            if (entity.HasComponent<LineComponent>())
            {
                auto& line = entity.GetComponent<LineComponent>();
                m_TransformComponent->Pivot += line.GetCenterOfGravity();
            }
            else if (entity.HasComponent<CircleComponent>())
            {
                auto& circle = entity.GetComponent<CircleComponent>();
                m_TransformComponent->Pivot += circle.GetCenterOfGravity();
                m_MustKeepRatio = true;
            }
            else if (entity.HasComponent<ArcComponent>())
            {
                auto& arc = entity.GetComponent<ArcComponent>();
                m_TransformComponent->Pivot += arc.GetCenterOfGravity();
                m_MustKeepRatio = true;
            }
            else if (entity.HasComponent<PolygonComponent>())
            {
                auto& polygon = entity.GetComponent<PolygonComponent>();
                m_TransformComponent->Pivot += polygon.GetCenterOfGravity();
            }
            else if (entity.HasComponent<CurveComponent>())
            {
                auto& curve = entity.GetComponent<CurveComponent>();
                m_TransformComponent->Pivot += curve.GetCenterOfGravity();
            }
        }

        m_TransformComponent->Pivot /= (float)m_SelectedEntities.size();
    }

    void HierarchyPanel::EndTransformation(bool apply)
    {
        GF_INFO("End transformation");
        auto transformMatrix = m_TransformComponent->GetTransformMatrix();
        for (auto selectedEntity : m_SelectedEntities)
        {
            if (apply)
            {
                if (selectedEntity.HasComponent<LineComponent>())
                {
                    auto& line = selectedEntity.GetComponent<LineComponent>();
                    line.P0 = Math::Transform(transformMatrix, line.P0);
                    line.P1 = Math::Transform(transformMatrix, line.P1);
                }
                else if (selectedEntity.HasComponent<CircleComponent>())
                {
                    auto& circle = selectedEntity.GetComponent<CircleComponent>();
                    circle.Center = Math::Transform(transformMatrix, circle.Center);
                    circle.Radius = circle.Radius * m_TransformComponent->Scale.x;
                }
                else if (selectedEntity.HasComponent<ArcComponent>())
                {
                    auto& arc = selectedEntity.GetComponent<ArcComponent>();

                    glm::vec2 newCenter = Math::Transform(transformMatrix, arc.Center);

                    float radiansAngle1 = glm::radians(arc.Angle1);
                    float radiansAngle2 = glm::radians(arc.Angle2);
                    glm::vec2 delta1 = arc.Radius * glm::vec2{ glm::cos(radiansAngle1), glm::sin(radiansAngle1) };
                    glm::vec2 delta2 = arc.Radius * glm::vec2{ glm::cos(radiansAngle2), glm::sin(radiansAngle2) };
                    delta1 = Math::Transform(transformMatrix, delta1 + arc.Center) - newCenter;
                    delta2 = Math::Transform(transformMatrix, delta2 + arc.Center) - newCenter;

                    arc.Center = newCenter;
                    arc.Radius = arc.Radius * m_TransformComponent->Scale.x;
                    arc.Angle1 = glm::degrees(glm::atan(delta1.y, delta1.x));
                    arc.Angle2 = glm::degrees(glm::atan(delta2.y, delta2.x));
                }
                else if (selectedEntity.HasComponent<PolygonComponent>())
                {
                    auto& polygon = selectedEntity.GetComponent<PolygonComponent>();
                    polygon.Vertices = Math::Transform(transformMatrix, polygon.Vertices);
                }
                else if (selectedEntity.HasComponent<CurveComponent>())
                {
                    auto& curve = selectedEntity.GetComponent<CurveComponent>();
                    curve.ControlPoints = Math::Transform(transformMatrix, curve.ControlPoints);
                }
            }

            auto& transform = selectedEntity.GetComponent<TransformComponent>();
            transform = {};
        }
        m_TransformComponent = nullptr;
        m_PanelState = PanelState::Entity;
        m_SelectedEntities.clear();
    }

    void HierarchyPanel::BeginLineClipping()
    {
        GF_INFO("Begin line clipping");
        m_IsClipping = true;
        m_SelectedEntities.clear();
    }

    void HierarchyPanel::EndLineClipping(bool apply)
    {
        GF_INFO("End line clipping");
        if (apply)
        {
            auto view = m_Scene->GetEntitiesWith<LineComponent>();

            for (auto e : view)
            {
                Entity entity{ e, m_Scene.get() };
                auto& line = entity.GetComponent<LineComponent>();
                std::vector<glm::vec2> newPoints;
                auto& clip = m_LineClippingComponent;
                switch (clip.Algorithm)
                {
                case LineClippingAlgorithmType::CohenSutherland:
                {
                    newPoints = ClippingAlgorithm::CohenSutherland(line.P0, line.P1, clip.GetBottomLeft(), clip.GetTopRight());
                    break;
                }
                case LineClippingAlgorithmType::Midpoint:
                {
                    newPoints = ClippingAlgorithm::Midpoint(line.P0, line.P1, clip.GetBottomLeft(), clip.GetTopRight());
                    break;
                }
                }
                line.P0 = newPoints[0];
                line.P1 = newPoints[1];
            }
        }
        m_IsClipping = false;
    }

    void HierarchyPanel::OnUIRender()
    {
        switch (m_PanelState)
        {
        case Grafix::PanelState::Entity: { UI_Properties();; break; }
        case Grafix::PanelState::Transform: { UI_Transformation(); break; }
        case Grafix::PanelState::LineClip: { UI_LineClipping(); break; }
        }

        UI_Hierarchy();
    }

    // *****************************************************************************************************************************************************************
    // UI Functions
    // *****************************************************************************************************************************************************************

    void HierarchyPanel::UI_Hierarchy()
    {
        ImGui::Begin("Hierarchy");
        m_Scene->m_Registry.each(
            [this](auto entityID) {
                DrawItem(Entity{ entityID, m_Scene.get() });
            }
        );

        // Left-click on blank space to unselect
        if (m_PanelState != PanelState::Entity)
        {
            ImGui::End();
            return;
        }

        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            m_SelectedEntities.clear();

        if (!m_SelectedEntities.empty() && ImGui::IsKeyPressed(ImGuiKey_Delete))
            ImGui::OpenPopup("Warning");

        m_IsModalOpen = ImGui::BeginPopupModal("Warning", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
        if (m_IsModalOpen)
        {
            ImGui::Text("Delete this entity?");

            if (ImGui::Button("OK", ImVec2(120, 0)))
            {
                for (auto entity : m_SelectedEntities)
                    m_Scene->RemoveEntity(entity);
                m_SelectedEntities.clear();
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0)))
                ImGui::CloseCurrentPopup();

            ImGui::EndPopup();
        }
        ImGui::End();
    }

    void HierarchyPanel::UI_Properties()
    {
        ImGui::Begin("Properties");
        if (m_SelectedEntities.size() != 1)
        {
            ImGui::End();
            return;
        }

        // The following controls are only available when a single entity is selected
        auto selectedEntity = *m_SelectedEntities.begin();

        ImGui::Text("Tag");
        ImGui::SameLine();
        auto& tag = selectedEntity.GetComponent<TagComponent>().Tag;
        m_TagBuffer = tag;

        ImGui::InputText("##Tag", &m_TagBuffer);
        if (!m_TagBuffer.empty())
            tag = m_TagBuffer;

        ImGui::Separator();

        if (selectedEntity.HasComponent<LineComponent>())
        {
            ImGui::Text("Line");
            auto& line = selectedEntity.GetComponent<LineComponent>();
            std::vector<std::string> lineAlgorithmStrings = { "Midpoint", "Bresenham" };
            DrawAlgorithmControl(lineAlgorithmStrings, line.Algorithm);
            DrawFloat2Control("Point A", &line.P0.x, &line.P0.y);
            DrawFloat2Control("Point B", &line.P1.x, &line.P1.y);
            DrawLineWidthAndStyleControl(&line.LineWidth, line.LineStyle);
            DrawColorControl(line.Color);
        }
        else if (selectedEntity.HasComponent<CircleComponent>())
        {
            ImGui::Text("Circle");
            auto& circle = selectedEntity.GetComponent<CircleComponent>();
            DrawFloat2Control("Center", &circle.Center.x, &circle.Center.y);
            DrawFloatControl("Radius", &circle.Radius, 0.0f);
            ImGui::Separator();
            DrawLineWidthAndStyleControl(&circle.LineWidth, circle.LineStyle);
            ImGui::Separator();
            DrawColorControl(circle.Color);
        }
        else if (selectedEntity.HasComponent<ArcComponent>())
        {
            ImGui::Text("Arc");
            auto& arc = selectedEntity.GetComponent<ArcComponent>();
            DrawFloat2Control("Center", &arc.Center.x, &arc.Center.y);
            DrawFloatControl("Radius", &arc.Radius, 0.0f);
            DrawFloatControl("Angle 1", &arc.Angle1);
            DrawFloatControl("Angle 2", &arc.Angle2);
            ImGui::Checkbox("Major", &arc.Major);
            DrawLineWidthAndStyleControl(&arc.LineWidth, arc.LineStyle);
            DrawColorControl(arc.Color);
        }
        else if (selectedEntity.HasComponent<FillComponent>())
        {
            auto& fill = selectedEntity.GetComponent<FillComponent>();
            DrawFloat2Control("Point", fill.Point);
            DrawColorControl(fill.Color);
        }
        else if (selectedEntity.HasComponent<PolygonComponent>())
        {
            ImGui::Text("Polygon");
            auto& polygon = selectedEntity.GetComponent<PolygonComponent>();
            for (int i = 0; i < polygon.Vertices.size(); ++i)
            {
                ImGui::PushID(i);
                std::string label = "Vertex " + std::to_string(i + 1);
                DrawFloat2Control(label, &polygon.Vertices[i].x, &polygon.Vertices[i].y);
                ImGui::PopID();
            }
            DrawColorControl(polygon.Color);
        }
        else if (selectedEntity.HasComponent<CurveComponent>())
        {
            ImGui::Text("Curve");
            auto& curve = selectedEntity.GetComponent<CurveComponent>();
            std::vector<std::string> curveAlgorithmStrings = { "Bezier", "NURBS" };
            DrawAlgorithmControl(curveAlgorithmStrings, curve.Algorithm);
            for (int i = 0; i < curve.ControlPoints.size(); ++i)
            {
                ImGui::PushID(i);
                std::string label = "Ctrl Point " + std::to_string(i + 1);
                DrawFloat2Control(label, &curve.ControlPoints[i].x, &curve.ControlPoints[i].y);
                ImGui::PopID();
            }
            if (curve.Algorithm == CurveAlgorithmType::NURBS)
            {
                int minOrder = 2;
                int maxOrder = std::max(3, (int)curve.ControlPoints.size() - 1);
                DrawIntControl("Order", &curve.Order, minOrder, maxOrder);
            }
            DrawFloatControl("Step", &curve.Step, 0.0002f, 0.02f, "%.4f", 0.0001f);
            DrawLineWidthAndStyleControl(&curve.LineWidth, curve.LineStyle);
            DrawColorControl(curve.Color);
        }
        else if (selectedEntity.HasComponent<SphereComponent>())
        {
            ImGui::Text("Sphere");
            auto& sphere = selectedEntity.GetComponent<SphereComponent>();
            ImGui::Text("Control Environment Light");
            ImGui::DragFloat3("envir", glm::value_ptr(sphere.EnvirDirection), 0.01f, -1.0f, 1.0f);

            ImGui::Text("Model");
            ImGui::Checkbox("Lambert(n) or Phong(y)", &sphere.Model);
            if (sphere.Model)
            {
                ImGui::Text("p");
                ImGui::DragFloat("p", &sphere.P, 1.0f, 4.0f, 64.0f);
            }
        }
        ImGui::End();
    }

    void HierarchyPanel::UI_Transformation()
    {
        ImGui::Begin("Transformation");

        ////glm::vec2 pivot = GetPivot();
        ////ImGui::Text("Pivot: (%.3f, %.3f)", pivot.x, pivot.y);

        DrawFloat2Control("Position", m_TransformComponent->Translation);
        DrawFloatControl("Rotation", &m_TransformComponent->Rotation);

        if (m_TransformComponent->KeepRatio)
        {
            DrawFloatControl("Scale", &m_TransformComponent->Scale.x, 0.0f);
            m_TransformComponent->Scale.y = m_TransformComponent->Scale.x;
        }
        else
        {
            DrawFloat2Control("Scale", m_TransformComponent->Scale, 0.0f);
        }

        if (!m_MustKeepRatio)
        {
            ImGui::SameLine();
            ImGui::Checkbox("Keep Ratio", &m_TransformComponent->KeepRatio);
        }

        if (ImGui::Button("Apply"))
            EndTransformation(true);
        ImGui::SameLine();
        if (ImGui::Button("Cancel"))
            EndTransformation(false);

        ImGui::End();
    }

    void HierarchyPanel::UI_LineClipping()
    {
        ImGui::Begin("Line Clipping");
        std::vector<std::string> clipAlgorithmStrings = { "CohenSutherland", "Midpoint" };
        DrawAlgorithmControl(clipAlgorithmStrings, m_LineClippingComponent.Algorithm);

        if (m_IsClipping)
        {
            if (ImGui::Button("Apply"))
                EndLineClipping(true);
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
                EndLineClipping(false);
        }
        ImGui::End();
    }

    void HierarchyPanel::DrawItem(Entity entity)
    {
        ImGui::Selectable(entity.GetTag().c_str(), IsSelected(entity));
        if (ImGui::IsItemClicked())
        {
            if (m_PanelState != PanelState::Entity)
                return;

            bool control = ImGui::IsKeyDown(ImGuiKey_LeftCtrl) || ImGui::IsKeyDown(ImGuiKey_RightCtrl);
            if (control)
                ToggleSelectedEntity(entity);
            else
                SwitchSelectedEntity(entity);
        }
    }

    void HierarchyPanel::DrawLineWidthAndStyleControl(float* lineWidth, LineStyleType& lineStyle)
    {
        DrawFloatControl("Width", lineWidth, 0.0f, 50.0f);

        const char* lineStyleStrings[] = { "Solid", "Dashed", "Dotted" };
        const char* currentLineStyleString = lineStyleStrings[(int)lineStyle];
        ImGui::Text("Line Style");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Line Style", currentLineStyleString))
        {
            for (int i = 0; i < 3; ++i)
            {
                bool isSelected = currentLineStyleString == lineStyleStrings[i];
                if (ImGui::Selectable(lineStyleStrings[i], isSelected))
                {
                    currentLineStyleString = lineStyleStrings[i];
                    lineStyle = (LineStyleType)i;
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    void HierarchyPanel::DrawAlgorithmControl(const std::vector<std::string>& algorithmStrings, auto& algorithm)
    {
        const char* currentAlgorithmString = algorithmStrings[(int)algorithm].c_str();

        ImGui::Text("Algorithm");
        ImGui::SameLine();
        if (ImGui::BeginCombo("##Algorithm", currentAlgorithmString))
        {
            for (int i = 0; i < algorithmStrings.size(); ++i)
            {
                bool isSelected = currentAlgorithmString == algorithmStrings[i];
                if (ImGui::Selectable(algorithmStrings[i].c_str(), isSelected))
                {
                    currentAlgorithmString = algorithmStrings[i].c_str();
                    algorithm = decltype(algorithm)(i);
                }

                if (isSelected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
}