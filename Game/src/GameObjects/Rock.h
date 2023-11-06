#pragma once
#include "../GameObject.h"

class Rock final : public GameObject
{
public:
	static void Init();

	virtual void OnRender() override;

	std::vector<glm::vec2> GetCollisionPoints() const;
private:
    static Grafix::PolygonComponent s_Rock;
};
