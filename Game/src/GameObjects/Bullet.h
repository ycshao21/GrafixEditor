#pragma once

#include "GameObject.h"

class Bullet final : public GameObject
{
public:
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;

    bool IsAlive() const { return m_Alive; }
    void SetAlive(bool alive) { m_Alive = alive; }
private:
    float m_Age = 0.0f;
    bool m_Alive = false;
};