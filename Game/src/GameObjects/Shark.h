#pragma once
#include "../GameObject.h"

class Shark final : public GameObject
{
public:
    Shark();
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    std::vector<glm::vec2> GetCollisionPoints();
    bool IsAlive() { return m_Alive; }
    void Hit() { m_Alive = false; }
    void ReSet() { m_Alive = true; }
private:
    glm::vec2 m_Velocity = { -30.0f, 0.0f };
    std::vector<glm::vec2> m_CollisionPoints;

    std::vector<Grafix::CurveComponent> m_Body;
    std::vector<Grafix::LineComponent> m_Line;
    Grafix::CircleComponent m_Eye;
    bool m_Alive ; 
};