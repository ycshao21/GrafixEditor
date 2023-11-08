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
    void SetAlive(glm::vec2 pos);
    void SetDead() { m_Alive = false; }
private:
    bool m_Alive = true;

    glm::vec2 m_Velocity = { -30.0f, 0.0f };
    std::vector<glm::vec2> m_CollisionPoints = {
		{-0.9f, 0.0f},
		{-0.5f,-0.2f},
		{ 0.5f,-0.1f},
		{ 0.8f,-0.25f},
		{ 0.8f, 0.3f},
		{ 0.5f, 0.0f},
		{ 0.1f, 0.65f},
		{-0.3f, 0.35f},
		{-0.8f, 0.3f}
	};


    std::vector<Grafix::CurveComponent> m_Body;
    std::vector<Grafix::LineComponent> m_Line;
    Grafix::CircleComponent m_Eye;
};