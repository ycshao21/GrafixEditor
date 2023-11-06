#pragma once

#include "../GameObject.h"

class Bullet final : public GameObject
{
public:
    Bullet();
    static void Init();

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    std::vector<glm::vec2> GetCollisionPoints() const;

    bool IsAlive() const { return m_LifeRemaining > 0.0f; }
    void Activate(glm::vec2 position, float degree);
    void Deactivate();
private:
    glm::vec2 m_Velocity = glm::vec2(0.0f);

    const float m_LifeTime = 1.5f;
    float m_LifeRemaining = 0.0f;

    std::vector<glm::vec2> m_CollisionPoints = {
        {-1.0f, 0.0f},
        {1.0f, 0.0f},
        {1.0f, 1.0f},
        {-1.0f, 1.0f}
    };
   
    static Grafix::PolygonComponent s_Bullet;
};