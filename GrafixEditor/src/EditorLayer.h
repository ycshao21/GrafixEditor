#pragma once

#include "Grafix.h"
#include "Panels/HierarchyPanel.h"

namespace Grafix
{
    class EditorLayer final : public Layer
    {
        enum class ToolState : uint8_t
        {
            Move = 0,
            Line, Circle, Arc,
            Fill, LineClip, PolyClip,
            Polygon,
            Curve,
            Sphere,
            Transform
        };
    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnUpdate() override;
        virtual void OnUIRender() override;
        virtual void OnEvent(Event& e) override;
    private:
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void Render();

        bool IsMouseInCanvas() const;
        void UpdateMousePos();
        void OnMoveToolUpdate();
        void OnLineToolUpdate();
        void OnCircleToolUpdate();
        void OnArcToolUpdate();
        void OnFillToolUpdate();
        void OnLineClipToolUpdate();
        void OnPolyClipToolUpdate();
        void OnPolygonToolUpdate();
        void OnCurveUpdate();

        void UI_MenuBar();
        void UI_Canvas();
        void UI_Toolbar();
        void UI_Info();
        void UI_Color();

        void SwitchScene(const std::shared_ptr<Scene>& scene);
    private:
        // Canvas
        uint32_t m_CanvasWidth = 1280, m_CanvasHeight = 720;
        glm::vec2 m_CanvasBounds[2];
        bool m_CanvasFocused = false, m_CanvasHovered = false;

        // Scene
        std::shared_ptr<Scene> m_ActiveScene = nullptr;
        std::shared_ptr<Scene> m_Scene2D = nullptr;
        std::shared_ptr<Scene> m_Scene3D = nullptr;
        Renderer m_Renderer;
        Camera m_Camera;
        Camera3D m_Camera3D;

        // Mouse
        glm::vec2 m_MousePosInCanvas{ 0.0f, 0.0f };
        glm::vec2 m_MousePosInWorld{ 0.0f, 0.0f };
        glm::vec2 m_MousePositionDelta = { 0.0f, 0.0f };

        Entity m_HoveredEntity;
        glm::vec2* m_SelectedControlPoint = nullptr;

        // Tool
        ToolState m_ToolState = ToolState::Move;
        bool m_IsDrawing = false;

        // Color
        glm::vec3 m_PickedColor{ 0.9f, 0.9f, 0.9f };
        glm::vec3 m_AuxColor{ 0.6f, 0.6f, 0.6f };
        glm::vec3 m_ControlPointColor{ 0.179f, 0.609f, 0.742f };
        float m_ControlPointWidth = 12.0f;

        HierarchyPanel m_HierarchyPanel;
    };
}