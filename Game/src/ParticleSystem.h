#pragma once

#include "Grafix.h"


struct ParticleProps
{
	glm::vec2 Position;

	glm::vec2 Velocity = glm::vec2(-20.0f, 0.0f);
	glm::vec2 VelocityVariation = glm::vec2(5.0f, 10.0f);

	float BeginSize = 20.0f, EndSize = 0.0f;
	float SizeVariation = 3.0f;

	float LifeTime = 1.0f, LifeRemaining = 1.0f;

	glm::vec3 Color = glm::vec3(0.5f);
	float ColorVariation = 0.3f;
};

class ParticleSystem final
{
public:
	ParticleSystem();

	void OnUpdate(float ts);
	void OnRender();

	void Emit(const ParticleProps& particleProps);
private:
    struct Particle
    {
        glm::vec2 Position;
        glm::vec2 Velocity;
        float BeginSize, EndSize = 0.0f;

        float LifeTime = 0.0f;
		float LifeRemaining = 0.0f;

        glm::vec3 Color;
    };

	uint32_t m_Index = 0;
	std::vector<Particle> m_Particles;
};
