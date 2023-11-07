#pragma once

#include "Grafix.h"

class GameObject
{
public:
    virtual ~GameObject() = default;

    virtual void OnUpdate(float ts) {}
    virtual void OnRender() {}

    void SetPosition(const glm::vec2& position) { m_Transform.Translation = position; }
    void SetRotation(float rotation) { m_Transform.Rotation = rotation; }
    void SetScale(const glm::vec2& scale) { m_Transform.Scale = scale; }

    glm::vec2 GetPosition() const { return m_Transform.Translation; }
    float GetRotation() const { return m_Transform.Rotation; }
    glm::vec2 GetScale() const { return m_Transform.Scale; }

    Grafix::TransformComponent& GetTransform() { return m_Transform; }
    const Grafix::TransformComponent& GetTransform() const { return m_Transform; }
protected:
    Grafix::TransformComponent m_Transform;
};
