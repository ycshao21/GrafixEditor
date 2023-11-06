#include "ParticleSystem.h"

ParticleSystem::ParticleSystem()
	: m_Particles(300)
{
}

void ParticleSystem::Emit(const ParticleProps& particleProps)
{
	auto& particle = m_Particles[m_Index];

	particle.Position = particleProps.Position;

	particle.Velocity = particleProps.Velocity;
	particle.Velocity.x += particleProps.VelocityVariation.x * Grafix::Random::GenerateFloat();
	particle.Velocity.y += particleProps.VelocityVariation.y * Grafix::Random::GenerateFloat();

	particle.BeginSize = particleProps.BeginSize;
	particle.EndSize = particleProps.EndSize;

	particle.LifeTime = particleProps.LifeTime;
	particle.LifeRemaining = particleProps.LifeTime;

	particle.Color = particleProps.Color;
	particle.Color += particleProps.ColorVariation * Grafix::Random::GenerateFloat(0.0f, 1.0f);
	particle.Color = glm::clamp(particle.Color, 0.0f, 1.0f);

	m_Index = (m_Index + 1) % m_Particles.size();
}


void ParticleSystem::OnUpdate(float ts)
{
	for (auto& particle : m_Particles)
	{
		if (particle.LifeRemaining <= 0.0f)
			continue;

		particle.LifeRemaining -= ts;
		particle.Position += particle.Velocity * (float)ts;
	}
}

void ParticleSystem::OnRender()
{
	for (auto& particle : m_Particles)
	{
        if (particle.LifeRemaining <= 0.0f)
            continue;

		float lifeRatio = particle.LifeRemaining / particle.LifeTime;
        float size = particle.BeginSize * lifeRatio + particle.EndSize * (1.0f - lifeRatio);
		Grafix::Renderer::DrawCircle(particle.Position, size, particle.Color, 2.0f);
	}
}
