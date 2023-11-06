#pragma once

#include "../GameObject.h"
#include "../ParticleSystem.h"

class Player final : public GameObject
{
public:
    Player();
    virtual ~Player() = default;

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    void Reset();

    std::vector<glm::vec2> GetCollisionPoints() const;

    // Bullets
    void UseBullet() { --m_BulletCount; }
    int GetBulletCount() const { return m_BulletCount; }
    bool HasBullets() const { return m_BulletCount > 0; }
private:
    // Movement
    float m_Power = 16.0f;
    glm::vec2 m_Velocity;

    ParticleProps m_BubbleProps;
    ParticleSystem m_BubbleGenerator;

    // Bullets
    int m_BulletCount;

    // Collision
    std::vector<glm::vec2> m_CollisionPoints = {
        {0.85f,0.3f},
        {0.0f, 0.3f},
        {-0.9f,0.3f},
        {-0.9f,-0.3f},
		{0.0f,-0.3f},
        {0.85f,-0.3f}
    };

    // Shapes
    Grafix::PolygonComponent m_Body;
    Grafix::CircleComponent m_Windows[3];
};
