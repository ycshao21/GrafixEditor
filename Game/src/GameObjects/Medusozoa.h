#pragma once
#include "../GameObject.h"

//ShuiMu
class Medusozoa final : public GameObject
{
public:
    Medusozoa();
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
    void Hit() { m_Alive = false; };
    void Reset() { m_Time = 0.0f; m_Alive = true; };
    bool IsAlive() const { return m_Alive; }
    std::vector<glm::vec2> GetCollisionPoints();
private:
    float m_Time = 0.0f;
    glm::vec2 m_Velocity = { 0.0f, 0.0f };
    std::vector<glm::vec2> m_CollisionPoints;
    Grafix::PolygonComponent m_Body;
    
    std::vector<Grafix::CurveComponent> m_Tentacle;
    bool m_Alive;
};