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

void Level::Init()
{
	// Background color (ocean)
	Grafix::Renderer::SetClearColor({ 0.140f, 0.307f, 0.389f });

	Seamount::Init();
	Fish::Init();
	Bullet::Init();

    m_Seamounts.resize(4);
    m_Fishes.resize(5);
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
		{
            m_Player.UseBullet();
            m_Bullet.Activate(m_Player.GetPosition(), m_Player.GetRotation());
		}
	}

	if (m_Bullet.IsAlive())
	{
		BulletCollisionDetection();
        m_Bullet.OnUpdate(ts);
	}

	float playerX = m_Player.GetPosition().x;
	if (playerX > m_Seamounts[m_IndexOfNextSeamount].first.GetPosition().x)
	{
        ++m_Score;
        m_IndexOfNextSeamount = (m_IndexOfNextSeamount + 1) % m_Seamounts.size();
    }

	if (playerX > m_SeamountGenDetectX)
	{
        GenerateSeamount(m_IndexOfSeamountToGen, m_SeamountGenDetectX + 2.0f * m_SeamountGap);
		m_IndexOfSeamountToGen = (m_IndexOfSeamountToGen + 1) % m_Seamounts.size();
        m_SeamountGenDetectX += m_SeamountGap;
	}

	if (playerX > m_Fishes[m_IndexOfFirstFish].GetPosition().x + 500.0f)
	{
		GenerateFish(m_IndexOfFirstFish, playerX + Grafix::Random::GenerateFloat(800.0f, 1300.0f));
        m_IndexOfFirstFish = (m_IndexOfFirstFish + 1) % m_Fishes.size();
    }
	
	for (auto& fish : m_Fishes)
		fish.OnUpdate(ts);
}

void Level::OnRender()
{
	for (auto& fish : m_Fishes)
		fish.OnRender();

	// Player
	for (auto& [bottomSeamount, topSeamount] : m_Seamounts)
	{
		topSeamount.OnRender();
		bottomSeamount.OnRender();
	}

	RenderWalls();

	if(m_Bullet.IsAlive())
        m_Bullet.OnRender();

	m_Player.OnRender();
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
    
	m_IndexOfNextSeamount = 0;
	m_IndexOfSeamountToGen = 0;
    m_SeamountGenDetectX = 2.0f * m_SeamountGap;
	for (int i = 0; i < m_Seamounts.size(); ++i)
		GenerateSeamount(i, i * m_SeamountGap);

    m_IndexOfFirstFish = 0;
	float lastFishX = -700.0f;
	for (int i = 0; i < m_Fishes.size(); ++i)
	{
		float currentFishX = lastFishX + Grafix::Random::GenerateFloat(100.0f, 500.0f);
        GenerateFish(i, currentFishX);
		lastFishX = currentFishX;
	}
}

void Level::GenerateBullet()
{
	// TODO: Generate bullet randomly
}

void Level::GenerateSeamount(int index, float x)
{
    float center = Grafix::Random::GenerateFloat(-190.0f, 190.0f);
    float verticalGap = Grafix::Random::GenerateFloat(70.0f, 130.0f);

    float bottomSeamountHeight = m_WallHeightOffset + center - verticalGap / 2.0f;
    float topSeamountHeight = 2.0f * m_WallHeightOffset - bottomSeamountHeight - verticalGap;

    GF_INFO("New Seamount: x = {0}", x);

    auto& [bottomSeamount, topSeamount] = m_Seamounts[index];
	constexpr float seamountWidth = 290.0f;

    bottomSeamount.SetPosition({ x, -m_WallHeightOffset - 5.0f });
    bottomSeamount.SetScale({ seamountWidth, bottomSeamountHeight });

    topSeamount.SetPosition({ x, m_WallHeightOffset + 5.0f });
    topSeamount.SetRotation(180.0f);
    topSeamount.SetScale({ seamountWidth, topSeamountHeight });
}

void Level::GenerateFish(int index, float x)
{
	Fish& fish = m_Fishes[index];

    fish.SetPosition({ x, Grafix::Random::GenerateFloat(-300.0f, 300.0f)});

    float scale = Grafix::Random::GenerateFloat(12.0f, 25.0f);
    fish.SetScale(glm::vec2(scale));
}

bool Level::IsPlayerDead()
{
	// Wall collision
	for (auto& v : m_Player.GetCollisionPoints())
	{
		if (std::abs(v.y) > m_WallHeightOffset + 3.0f)
			return true;
	}

	// Seamount collision
    for (auto v : m_Player.GetCollisionPoints())
    {
        auto& [bottomSeamount, topSeamount] = m_Seamounts[m_IndexOfNextSeamount];
        if (IsInPolygon(v, topSeamount.GetCollisionPoints()) || IsInPolygon(v, bottomSeamount.GetCollisionPoints()))
            return true;
    }
	return false;
}

void Level::BulletCollisionDetection()
{
	glm::vec2 cp = m_Bullet.GetCollisionPoint();

    if (std::abs(cp.y) > m_WallHeightOffset)
    {
        m_Bullet.Deactivate();
        return;
    }

    for (auto& rock : m_Seamounts)
    {
        auto& [bottomSeamount, topSeamount] = rock;
        if (IsInPolygon(cp, bottomSeamount.GetCollisionPoints()) || IsInPolygon(cp, topSeamount.GetCollisionPoints()))
        {
            m_Bullet.Deactivate();
            return;
        }
    }
}

void Level::RenderWalls()
{
	std::vector<glm::vec2> wallVertices = {
		{ -0.5f, -0.5f },
		{  0.5f, -0.5f },
		{  0.5f,  0.5f },
		{ -0.5f,  0.5f }
	};
    glm::vec3 wallColor = { 0.159f, 0.109f, 0.063f };

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
		floorTransform.Pivot = { 0.0f, 0.5f };
		floorTransform.Translation = { m_Player.GetTransform().Translation.x, -m_WallHeightOffset };
		floorTransform.Scale = { 1800.0f,  m_WallThickness };
		Grafix::Renderer::DrawPolygon(floorTransform, wallVertices, wallColor);
	}
}
