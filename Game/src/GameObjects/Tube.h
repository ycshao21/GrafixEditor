#pragma once

#include "GameObject.h"

class Tube : public GameObject
{
public:
    virtual void OnUpdate(float ts) override;
    virtual void OnRender() override;
private:
    glm::vec3 m_Color = { 0.2f, 0.2f, 0.2f };

    
};
