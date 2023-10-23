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
    class HierarchyPanel
    {
    public:
        HierarchyPanel() = default;
        ~HierarchyPanel() = default;

        void BindScene(const std::shared_ptr<Scene>& scene) { m_Scene = scene; }

        void SwitchSelectedEntity(Entity entity);
        void ToggleSelectedEntity(Entity entity);
        inline bool HasSelectedEntity() const { return !m_SelectedEntities.empty(); }
        inline int GetNumOfSelectedEntities() const { return m_SelectedEntities.size(); }
        std::set<Entity>& GetSelectedEntities() { return m_SelectedEntities; }

        void OnUpdate();

        // Transformation
        inline bool IsTransforming() const { return m_IsTransforming; }
        void BeginTransformation();
        void EndTransformation(bool apply);
        glm::vec2 GetPivot() const;

        bool IsModalOpen() const { return m_IsModalOpen; }

        void OnUIRender();
    private:
        inline bool IsSelected(Entity entity) const { return m_SelectedEntities.find(entity) != m_SelectedEntities.end(); }

        void DrawItem(Entity entity);
        void DrawLineWidthAndStyleControl(float* width, LineStyleType& lineStyle);
        void DrawAlgorithmControl(const std::vector<std::string>& algorithmStrings, auto& lineAlgorithm);

        void UI_Hierarchy();
        void UI_Properties();
        void UI_Transformation();
    private:
        std::set<Entity> m_SelectedEntities{};
        std::shared_ptr<Scene> m_Scene = nullptr;

        bool m_IsTransforming = false;
        bool m_MustKeepRatio = false;

        bool m_IsModalOpen = false;
        std::string m_TagBuffer;

        TransformComponent m_TransformComponent{};
        FillComponent m_FillComponent{};
    };
}
