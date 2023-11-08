#pragma once

#include "../GameObject.h"

class Medusozoa final : public GameObject
{
public:
    Medusozoa();
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    std::vector<glm::vec2> GetCollisionPoints();
private:
    float m_Time = 0.0f;
    glm::vec2 m_Velocity = { 0.0f, 0.0f };

    std::vector<glm::vec2> m_CollisionPoints = {
		{ 0.7f,  0.7f},
		{-0.7f,  0.7f},
		{-0.7f,  0.0f},
		{-0.4f,  0.0f},
		{-0.4f, -0.7f},
		{ 0.4f, -0.7f},
		{ 0.4f,  0.0f},
		{ 0.7f,  0.0f}
	};


    Grafix::PolygonComponent m_Body;
    std::vector<Grafix::CurveComponent> m_Tentacle;
};