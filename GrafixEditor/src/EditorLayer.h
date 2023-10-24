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
            Clip, Fill,
            Polygon,
            Curve,
            Sphere
        };

    public:
        EditorLayer();
        virtual ~EditorLayer() = default;

        virtual void OnAttach() override;
        virtual void OnUpdate() override;
        virtual void OnUIRender() override;
        virtual void OnEvent(Event& e) override;

        void Render();
    private:
        bool OnKeyPressed(KeyPressedEvent& e);
        bool OnMouseButtonPressed(MouseButtonPressedEvent& e);

        void UpdateMousePos();
        bool IsMouseInCanvas() const;

        void OnMoveToolUpdate();
        void OnFillToolUpdate();
        void OnPolygonToolUpdate();
        void OnLineToolUpdate();
        void OnArcToolUpdate();
        void OnCircleToolUpdate();
        void OnCurveUpdate();
        void OnSphereUpdate(){}
        // NEW
        void OnClipToolUpdate();

        void UI_MenuBar();
        void UI_Canvas();
        void UI_Toolbar();
        void UI_Info();
        void UI_Color();
    private:
        uint32_t m_CanvasWidth = 1280, m_CanvasHeight = 720;
        glm::vec2 m_CanvasBounds[2];
        bool m_CanvasFocused = false, m_CanvasHovered = false;

        glm::vec2 m_MousePosInCanvas{ 0.0f, 0.0f };
        glm::vec2 m_MousePosInWorld{ 0.0f, 0.0f };
        glm::vec2 m_MousePositionDelta = { 0.0f, 0.0f };
        Entity m_HoveredEntity;

        Renderer m_Renderer;
        Camera m_Camera;
        Camera3D m_Camera3D;

        std::shared_ptr<Scene> m_EditorScene = nullptr;

        ToolState m_ToolState = ToolState::Move;
        bool m_IsDrawing = false;
        int m_OperationState = 1;
        int m_ControlPointWidth = 10;

        glm::vec2* m_SelectedControlPoint = nullptr;

        glm::vec3 m_PickedColor{ 0.9f, 0.9f, 0.9f };
        glm::vec3 m_AuxColor{ 0.6f, 0.6f, 0.6f };

        HierarchyPanel m_HierarchyPanel;
    };
}