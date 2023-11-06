#include "Level.h"
#include <random>

static bool IsInPolygon(glm::vec2 point, const std::vector<glm::vec2>& polygon) 
{ 
	int numOfIntersections = 0;

	for (int i = 0; i < polygon.size(); i++) 
	{
		auto& p1 = polygon[i];
		auto& p2 = polygon[(i + 1) % polygon.size()];

		if ((p1.y < point.y && p2.y >= point.y) || (p2.y < point.y && p1.y >= point.y)) 
			if (p1.x + (point.y - p1.y) / (p2.y - p1.y) * (p2.x - p1.x) < point.x) 
				++numOfIntersections;
	}

	return numOfIntersections & 1;
}


void Level::OnAttach()
{
	// Background color (ocean)
	Grafix::Renderer::SetClearColor({ 0.140f, 0.307f, 0.389f });

	Rock::Init();
	Fish::Init();

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
	
	for (auto& fish : m_Fishes)
		fish.OnUpdate(ts);
}

void Level::OnRender()
{
	for (auto& fish : m_Fishes)
		fish.OnRender();

	// Player
	for (auto& rock : m_Rocks)
		rock.OnRender();

	RenderWalls();

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

	m_Player.Reset();

	// TEMP
	GenerateRock();
	GenerateFish();
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

void Level::GenerateRock()
{
	// TEMP: To generate rocks randomly and call it in OnUpdate()

    m_Rocks.resize(20);
    float gap = 580.0f;
    float startX = 950.0f;

    for (int i = 0; i < m_Rocks.size(); i += 2)
    {
        float center = Grafix::Random::GenerateFloat(-180.0f, 180.0f);
        float verticalGap = Grafix::Random::GenerateFloat(90.0f, 120.0f);

        float bottomRockHeight = m_WallHeightOffset + center - verticalGap / 2.0f;
        float topRockHeight = 2.0f * m_WallHeightOffset - bottomRockHeight - verticalGap;

        Rock& bottomRock = m_Rocks[i];
        bottomRock.SetTranslation({ startX , -m_WallHeightOffset - 5.0f });
        bottomRock.SetScale({ 330.0f, bottomRockHeight });

        Rock& topRock = m_Rocks[i + 1];
        topRock.SetTranslation({ startX , m_WallHeightOffset + 5.0f });
        topRock.SetRotation(180.0f);
        topRock.SetScale({ 330.0f, topRockHeight });

        startX += gap;
    }
}

void Level::GenerateFish()
{
    m_Fishes.resize(10);
    float startX = -700.0f;

    for (auto& fish: m_Fishes)
    {
		float scale = Grafix::Random::GenerateFloat(10.0f, 35.0f);

        fish.SetTranslation({ startX , Grafix::Random::GenerateFloat(-300.0f, 300.0f) });
		fish.SetScale(glm::vec2(scale));

        float gap = Grafix::Random::GenerateFloat(100.0f, 600.0f);
        startX += gap;
    }
}

bool Level::IsPlayerDead()
{
	// Wall collision

	for (auto& v : m_Player.GetCollisionPoints())
	{
		if (std::abs(v.y) > m_WallHeightOffset + 3.0f)
			return true;
	}

	// Rock collision
	for (auto& rock : m_Rocks)
	{
		for (auto v : m_Player.GetCollisionPoints())
		{
			if (IsInPolygon(v, rock.GetCollisionPoints()))
				return true;
		}
	}
	return false;
}

void Level::RenderWalls()
{
	std::vector<glm::vec2> wallVertices = {
		{-0.5f, -0.5f},
		{0.5f, -0.5f},
		{0.5f, 0.5f},
		{-0.5f, 0.5f}
	};
    glm::vec3 wallColor = { 0.109f, 0.079f, 0.013f };

	// Walls
	{
		// Ceiling
		Grafix::TransformComponent ceilingTransform;
		ceilingTransform.Pivot = { 0.0f, -0.5f };
		ceilingTransform.Translation = { m_Player.GetTransform().Translation.x, m_WallHeightOffset };
		ceilingTransform.Scale = { 1800.0f,  m_WallThickness };
		Grafix::Renderer::DrawPolygon(ceilingTransform, wallVertices, wallColor);

		// Floor
		Grafix::TransformComponent floorTransform;
		ceilingTransform.Pivot = { 0.0f, 0.5f };
		ceilingTransform.Translation = { m_Player.GetTransform().Translation.x, -m_WallHeightOffset };
		ceilingTransform.Scale = { 1800.0f,  m_WallThickness };
		Grafix::Renderer::DrawPolygon(ceilingTransform, wallVertices, wallColor);
	}
}

void Level::RenderRocks()
{
}
