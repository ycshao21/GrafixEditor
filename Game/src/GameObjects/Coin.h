#pragma once

#include "../GameObject.h"

class Coin : public GameObject
{
public:
    Coin();
    virtual void OnRender() override;
private:
    Grafix::CircleComponent m_Coin;
};
