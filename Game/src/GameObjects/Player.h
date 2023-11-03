#pragma once

#include "GameObject.h"

class Player final : public GameObject
{
    using Transform = Grafix::TransformComponent;
    using Polygon = Grafix::PolygonComponent;
public:
    void Init();

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    std::vector<glm::vec2> GetCollisionPoints() const;
private:
    glm::vec2 m_MoveSpeed = glm::vec2(200.0f, 200.0f);
    float m_RotationSpeed = 30.0f;

    Polygon m_Plane;

    std::vector<glm::vec2> m_CollisionPoints = {
        {1.0f, 0.0f},
        {0.5f, 0.3f},
        {-1.0f, 0.3f},
        {-1.0f, -0.3f},
        {0.5f, -0.3f}
    };
};
