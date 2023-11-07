#pragma once

#include "../GameObject.h"

class BulletItem : public GameObject
{
public:
    BulletItem();
    virtual void OnRender() override;
private:
    Grafix::CircleComponent m_Shell;
    Grafix::PolygonComponent m_Bullet;
};

