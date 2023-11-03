#pragma once

#include "GameObject.h"

class Tube : public GameObject
{
    using Rock = Grafix::PolygonComponent;
public:
    void Init();

    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
private:
    glm::vec3 m_Color = { 0.2f, 0.2f, 0.2f };

    std::vector<Rock> m_Rock;
};