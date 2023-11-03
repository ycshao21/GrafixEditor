#pragma once

#include "GameObject.h"
#include "../ParticleSystem.h"

class Player final : public GameObject
{
    using Polygon = Grafix::PolygonComponent;
public:
    void Init();

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    std::vector<glm::vec2> GetCollisionPoints() const;

    // Bullets
    void UseBullet() { --m_BulletCount; }
    int GetBulletCount() const { return m_BulletCount; }
    bool HasBullets() const { return m_BulletCount > 0; }
private:
    // Movement
    float m_Power = 16.0f;
    glm::vec2 m_Velocity;

    ParticleProps m_SmokeProps;
    ParticleSystem m_SmokeGenerator;

    // Bullets
    int m_BulletCount;

    // Collision
    std::vector<glm::vec2> m_CollisionPoints = {
        {1.0f, 0.0f},
        {0.5f, 0.3f},
        {-1.0f, 0.3f},
        {-1.0f, -0.3f},
        {0.5f, -0.3f}
    };

    // Shapes
    Polygon m_Plane;
};
