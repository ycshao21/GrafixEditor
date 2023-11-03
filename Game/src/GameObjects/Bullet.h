#pragma once

#include "GameObject.h"

class Bullet final : public GameObject
{
public:
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    bool IsAlive() const { return m_LifeRemaining > 0.0f; }
    void SetAlive(bool alive);
private:
    const float m_LifeTime = 1.5f;
    float m_LifeRemaining = 0.0f;
};