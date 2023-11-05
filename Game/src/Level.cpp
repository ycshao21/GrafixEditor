#include "Level.h"
#include <random>

static bool IsInPolygon(glm::vec2 point,std::vector<glm::vec2> polygon) 
{ 
	int numIntersections = 0;
	int numVertices =polygon.size();
	for (int i = 0; i < numVertices; i++) 
	{
		const glm::vec2 p1 = polygon[i];
		const glm::vec2 p2 = polygon[(i + 1) % numVertices];

		if ((p1.y < point.y && p2.y >= point.y) || (p2.y < point.y && p1.y >= point.y)) 
			if (p1.x + (point.y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x) < point.x) 
				numIntersections++;
	}
	return (numIntersections % 2 != 0);
}


void Level::OnAttach()
{
	Grafix::Renderer::SetClearColor({ 0.3f, 0.4f, 0.5f });

	Reset();
}

void Level::OnUpdate(float ts)
{
	m_Time += ts;

	m_Player.OnUpdate(ts);

	if (IsPlayerDead())
	{
		m_GameOver = true;
		return;
	}

	if (Grafix::Input::IsKeyPressed(Grafix::Key::S))
	{
		if (m_Player.HasBullets() && !m_Bullet.IsAlive())
			ShootBullet();
	}

	m_Bullet.OnUpdate(ts);
}

void Level::OnRender()
{
	// Background
	RenderBackground();

	// Player
	for (auto& rock : m_Rocks)
		rock.OnRender();

	m_Player.OnRender();
	m_Bullet.OnRender();
}

void Level::OnDetach()
{
}

void Level::Reset()
{
	m_Time = 0.0f;
	m_Score = 0;
	m_GameOver = false;

	m_Player.Init();

	//init Rocks
	float wallEdge = m_WallHeightOffset - m_WallThickness / 2.0f;
	for (int i = 0; i < 10; i++)
	{
		Rock rock;
		rock.Init();
		rock.GetTransform().Translation.x = 1000.0f + i * 1000.0f;
		rock.GetTransform().Translation.y = -wallEdge;
		m_Rocks.push_back(rock);
	}
}

void Level::ShootBullet()
{
	m_Player.UseBullet();

	m_Bullet.SetAlive(true);

	auto& playerTranslation = m_Player.GetTranslation();
	auto bulletTranslation = glm::vec2(playerTranslation.x + 30.0f, playerTranslation.y);
	m_Bullet.SetTranslation(bulletTranslation);
}

void Level::GenerateBullet()
{
	// TODO: Generate bullet randomly
}

void Level::CreateRock()
{

}

bool Level::IsPlayerDead()
{
	// Wall collision
	float wallEdge = m_WallHeightOffset - m_WallThickness / 2.0f;

	for (auto& v : m_Player.GetCollisionPoints())
	{
		if (std::abs(v.y) > wallEdge + 3.0f)
			return true;
	}

	// Rock collision
	for (auto& rock : m_Rocks)
	{
		for (auto v : m_Player.GetCollisionPoints())
		{
			if (IsInPolygon(v, rock.collisionPoints()))
				return true;
		}
	}
	return false;
}

void Level::RenderBackground()
{
	std::vector<glm::vec2> squareVertices = {
		{-0.5f, -0.5f},
		{0.5f, -0.5f},
		{0.5f, 0.5f},
		{-0.5f, 0.5f}
	};

	// Walls
	{
		glm::vec3 wallColor = { 0.07f, 0.07f, 0.07f };

		Grafix::TransformComponent ceilTransform;
		ceilTransform.Scale = { 2000.0f,  m_WallThickness };
		ceilTransform.Translation = { m_Player.GetTransform().Translation.x, m_WallHeightOffset };
		Grafix::Renderer::DrawPolygon(ceilTransform, squareVertices, wallColor);

		Grafix::TransformComponent floorTransform;
		floorTransform.Scale = { 2000.0f,  m_WallThickness };
		floorTransform.Translation = { m_Player.GetTransform().Translation.x, -m_WallHeightOffset };
		Grafix::Renderer::DrawPolygon(floorTransform, squareVertices, wallColor);
	}

	//Fish
	{
		Grafix::TransformComponent fishTransform;
		float fishThickness = 30.0f;
		glm::vec3 fishColor = { 1.0f, 0.0f, 0.0f };

		fishTransform.Scale = { fishThickness,fishThickness };

		Grafix::CurveComponent fishCurve;
		fishCurve.ControlPoints = {
			{-0.8f,0.0f},
			{-0.4f,0.6f},
			{0.3f,0.1f},
		};
		fishCurve.LineWidth = 2.0f;
		fishCurve.Color = fishColor;

		Grafix::CurveComponent fishCurve2;
		fishCurve2.ControlPoints = {
			{0.3f,0.1f},
			{0.4f,0.15f},
			{0.6f,0.3f},
		};
		fishCurve2.LineWidth = 2.0f;
		fishCurve2.Color = fishColor;

		Grafix::CurveComponent fishCurve3;
		fishCurve3.ControlPoints = {
			{0.6f,0.3f},
			{0.8f,0.0f},
			{0.6f,-0.3f},
		};
		fishCurve3.LineWidth = 2.0f;
		fishCurve3.Color = fishColor;

		Grafix::CurveComponent fishCurve4;
		fishCurve4.ControlPoints = {
			{0.6f,-0.3f},
			{0.4f,-0.15f},
			{0.3f,-0.1f},
		};
		fishCurve4.LineWidth = 2.0f;
		fishCurve4.Color = fishColor;

		Grafix::CurveComponent fishCurve5;
		fishCurve5.ControlPoints = {
			{0.3f,-0.1f},
			{-0.4f,-0.6f},
			{-0.8f,0.0f},
		};
		fishCurve5.LineWidth = 2.0f;
		fishCurve5.Color = fishColor;

		Grafix::LineComponent line1, line2;
		line1.P0 = { -0.5f,0.27f };
		line1.P1 = { -0.3f,0.0f };
		line2.P0 = { -0.5f,-0.27f };
		line2.P1 = { -0.3f,0.0f };
		line1.LineWidth = 1.0f;
		line2.LineWidth = 1.0f;
		line1.Color = glm::vec3{ 1.0f };
		line2.Color = glm::vec3{ 1.0f };
		Grafix::CircleComponent eye;
		eye.Center = { -0.6f,0.0f };
		eye.Radius = 0.05f;
		eye.LineWidth = 1.0f;
		eye.Color = glm::vec3{ 0.0f };

		for (int i = 0; i < 20; i++)
		{
			fishTransform.Translation.x += 500;

			Grafix::Renderer::DrawCurve(fishTransform, fishCurve);
			Grafix::Renderer::DrawCurve(fishTransform, fishCurve2);
			Grafix::Renderer::DrawCurve(fishTransform, fishCurve3);
			Grafix::Renderer::DrawCurve(fishTransform, fishCurve4);
			Grafix::Renderer::DrawCurve(fishTransform, fishCurve5);
			Grafix::Renderer::DrawLine(fishTransform, line1);
			Grafix::Renderer::DrawLine(fishTransform, line2);
			Grafix::Renderer::DrawCircle(fishTransform, eye);
		}
	}
}