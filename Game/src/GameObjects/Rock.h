#pragma once
#include "GameObject.h"

class Rock : public GameObject
{
public:
	static void Init();

	virtual void OnRender() override;

	std::vector<glm::vec2> GetCollisionPoints();
private:
    static Grafix::PolygonComponent s_Rock;
};
