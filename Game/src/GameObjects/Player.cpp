#include "Player.h"

#include "../Level.h"

Player::Player()
{
	// Bubbles (particle system)
	{
		m_BubbleProps.Position = glm::vec2(0.0f);

		m_BubbleProps.Velocity = glm::vec2(-50.0f, 0.0f);
		m_BubbleProps.VelocityVariation = glm::vec2(26.0f, 40.0f);

		m_BubbleProps.BeginSize = 7.0f;
		m_BubbleProps.EndSize = 0.0f;
		m_BubbleProps.SizeVariation = 7.0f;

		m_BubbleProps.LifeTime = 1.8f;
		m_BubbleProps.LifeRemaining = 1.8f;

		m_BubbleProps.Color = glm::vec3(0.7f);
		m_BubbleProps.ColorVariation = 0.15f;
	}

    {
        // Submarine Body
        m_Body.Vertices = {
            {0.95f, 0.0f},
            {0.9f, 0.1f},
            {0.85f, 0.2f},
            {0.7f, 0.3f},
            {0.4f, 0.3f},
            {0.4f, 0.5f},
            {0.0f, 0.5f},
            {0.0f, 0.3f},
            {-0.4f, 0.3f},
            {-0.75f, 0.2f},
            {-0.8f, 0.2f},
            {-0.8f, 0.25f},
            {-0.9f, 0.25f},
            {-0.9f, -0.3f},
            {-0.8f, -0.3f},
            {-0.8f, -0.25f},
            {-0.75f, -0.25f},
            {-0.5f, -0.3f},
            {0.7f, -0.3f},
            {0.85f, -0.2f},
            {0.9f, -0.1f}
        };
        m_Body.Color = glm::vec3(0.851f, 0.428f, 0.132f);
    }

    // Submarine Windows
    {
        m_Windows[0].Center = { -0.1f, 0.0f };
        m_Windows[1].Center = { 0.2f, 0.0f };
        m_Windows[2].Center = { 0.5f, 0.0f };

        for (int i = 0; i < 3; i++)
        {
            m_Windows[i].Radius = 0.1f;
            m_Windows[i].LineWidth = 2.0f;
            m_Windows[i].Color = {0.897f, 0.764f, 0.596f};
        }
	}
}

void Player::Reset()
{
	m_Transform.Translation = glm::vec2(0.0f, 50.0f);
    m_Transform.Scale = glm::vec2(60.0f);

	m_Velocity = glm::vec2(150.0f, 0.0f);

	m_BulletCount = 3;
}

void Player::OnUpdate(float ts)
{
    // Press up arrow to go up
    if (Grafix::Input::IsKeyPressed(Grafix::Key::Up))
    {
        // Add power to velocity
        if(m_Velocity.y < 0.0f)
            m_Velocity.y += m_Power * 2.5f;
        else
            m_Velocity.y += m_Power;

        // Generate bubbles
		glm::mat3 rotationMatrix = Grafix::Math::CalcRotationMatrix(glm::radians(m_Transform.Rotation));
		m_BubbleProps.Position = m_Transform.Translation + Grafix::Math::Transform(rotationMatrix, glm::vec2(-50.0f, 0.0f));
		m_BubbleProps.Velocity.x = -0.7f * m_Velocity.x;
		m_BubbleGenerator.Emit(m_BubbleProps);
	}

	// Update velocity: v_t = v_0 - g * t
	m_Velocity.y -= Level::GetGravity();
	m_Velocity.y = glm::clamp(m_Velocity.y, -300.0f, 360.0f);

    // Update position and rotation
    m_Transform.Translation += m_Velocity * ts;
    m_Transform.Rotation = m_Velocity.y / 6.0f;

    m_BubbleGenerator.OnUpdate(ts);
}

void Player::OnRender()
{
    // Draw bubbles
	m_BubbleGenerator.OnRender();

    // Draw body
	Grafix::Renderer::DrawPolygon(m_Transform, m_Body);

    // Draw windows
	for (int i = 0; i < 3; i++)
		Grafix::Renderer::DrawCircle(m_Transform, m_Windows[i]);
}

std::vector<glm::vec2> Player::GetCollisionPoints() const
{
	return Grafix::Math::Transform(m_Transform.GetTransformMatrix(), m_CollisionPoints);
}
