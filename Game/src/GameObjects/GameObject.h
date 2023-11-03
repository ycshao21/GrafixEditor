#pragma once

#include "Grafix.h"

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void OnUpdate(float ts) = 0;
    virtual void OnRender() = 0;

    void SetTranslation(const glm::vec2& translation) { m_Transform.Translation = translation; }
    void SetRotation(float rotation) { m_Transform.Rotation = rotation; }
    void SetScale(const glm::vec2& scale) { m_Transform.Scale = scale; }

    const glm::vec2& GetTranslation() const { return m_Transform.Translation; }
    float GetRotation() const { return m_Transform.Rotation; }
    const glm::vec2& GetScale() const { return m_Transform.Scale; }

    Grafix::TransformComponent& GetTransform() { return m_Transform; }
    const Grafix::TransformComponent& GetTransform() const { return m_Transform; }
protected:
    Grafix::TransformComponent m_Transform;
};
