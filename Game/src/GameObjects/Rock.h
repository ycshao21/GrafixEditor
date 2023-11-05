#pragma once
#include "GameObject.h"

class Rock : public GameObject
{
	using rock = Grafix::PolygonComponent;
public:
	void Init();
	rock GetRock() { return m_Rock; };
	std::vector<glm::vec2> collisionPoints();
	virtual void OnUpdate(float ts) override;
	virtual void OnRender() override;
	

private:
	glm::vec3 m_Color = { 0.2f, 0.2f, 0.2f };

	rock m_Rock;
};
