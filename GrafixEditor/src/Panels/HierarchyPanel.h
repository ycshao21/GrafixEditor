#pragma once
#include "Grafix.h"

namespace std
{
    template<typename T> struct hash;

    template<>
    struct hash<Grafix::Entity>
    {
        std::size_t operator()(Grafix::Entity entity) const
        {
            return (uint32_t)(entt::entity)entity;
        }
    };
}

namespace Grafix
{
    enum class PanelState : uint8_t
    {
        Entity = 0,
        Transform, LineClip, PolyClip
    };

    class HierarchyPanel
    {
    public:
        HierarchyPanel() = default;
        ~HierarchyPanel() = default;

        void BindScene(const std::shared_ptr<Scene>& scene) { m_Scene = scene; }
        void OnUIRender();

        inline void SetPanelState(PanelState state) { m_PanelState = state; }
        inline PanelState GetPanelState() const { return m_PanelState; }

        void SwitchSelectedEntity(Entity entity);
        void ToggleSelectedEntity(Entity entity);
        inline bool HasSelectedEntity() const { return !m_SelectedEntities.empty(); }
        inline int GetNumOfSelectedEntities() const { return (int)m_SelectedEntities.size(); }
        std::set<Entity>& GetSelectedEntities() { return m_SelectedEntities; }

        void OnUpdate();

        // Transformation
        void BeginTransformation();
        bool IsTransforming() const { return m_PanelState == PanelState::Transform; }
        TransformComponent& GetTransformComponent() { return m_TransformComponent; }
        void EndTransformation(bool apply);

        // Clipping
        void BeginLineClipping();
        bool IsLineClipping() const { return m_IsLineClipping; }
        LineClippingComponent& GetLineClippingComponent() { return m_LineClippingComponent; }
        void EndLineClipping(bool apply);

        void BeginPolyClipping();
        bool IsPolyClipping() const { return m_IsPolyClipping; }
        PolyClippingComponent& GetPolyClippingComponent() { return m_PolyClippingComponent; }
        void EndPolyClipping(bool apply);

        bool IsModalOpen() const { return m_IsModalOpen; }
    private:
        inline bool IsSelected(Entity entity) const { return m_SelectedEntities.find(entity) != m_SelectedEntities.end(); }

        void DrawItem(Entity entity);
        void DrawLineWidthAndStyleControl(float* width, LineStyleType& lineStyle);
        void DrawAlgorithmControl(const std::vector<std::string>& algorithmStrings, auto& lineAlgorithm);

        void UI_Hierarchy();
        void UI_Properties();
        void UI_Transformation();
        void UI_LineClipping();
        void UI_PolyClipping();
    private:
        std::set<Entity> m_SelectedEntities{};
        std::shared_ptr<Scene> m_Scene = nullptr;

        PanelState m_PanelState = PanelState::Entity;

        bool m_IsModalOpen = false;
        std::string m_TagBuffer;

        bool m_IsLineClipping = false;
        LineClippingComponent m_LineClippingComponent;;

        bool m_IsPolyClipping = false;
        PolyClippingComponent m_PolyClippingComponent;

        bool m_MustKeepRatio = false;
        TransformComponent m_TransformComponent;
    };
}
