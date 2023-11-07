#pragma once

#include "../GameObject.h"

class Bullet final : public GameObject
{
public:
    Bullet();
    static void Init();

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    glm::vec2 GetCollisionPoint() const;

    bool IsActive() const { return m_LifeRemaining > 0.0f; }
    void Activate(glm::vec2 position, float degree);
    void Deactivate();
private:
    glm::vec2 m_Velocity = glm::vec2(0.0f);

    const float m_LifeTime = 1.5f;
    float m_LifeRemaining = 0.0f;

    glm::vec2 m_CollisionPoint = glm::vec2(1.0f, 0.0f);

    static Grafix::PolygonComponent s_Bullet;
};