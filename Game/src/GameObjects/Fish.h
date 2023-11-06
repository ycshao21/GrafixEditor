#pragma once

#include "../GameObject.h"

class Fish final : public GameObject
{
public:
    static void Init();

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
private:
    glm::vec2 m_Velocity = { -30.0f, 0.0f };

    static std::vector<Grafix::CurveComponent> s_Body;
    static std::vector<Grafix::LineComponent>  s_Tail, s_Gill;
    static Grafix::CircleComponent             s_Eye;
};
