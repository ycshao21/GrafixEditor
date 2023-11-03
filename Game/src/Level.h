#pragma once

#include "GameObjects/Player.h"
#include "GameObjects/Bullet.h"
#include "GameObjects/Tube.h"

class Level final
{
public:
    void OnAttach();
    void OnUpdate(float ts);
    void OnRender();
    void OnDetach();

    bool IsGameOver() const { return m_GameOver; }
    const Player& GetPlayer() const { return m_Player; }

    uint64_t GetScore() const { return m_Score; }

    void Reset();
private:
    void CreateBullet();
    void CreateTube();

    // Collision detection
    bool CollisionDetection();

    void RenderBackground();
private:
    float m_Time = 0.0f;
    uint64_t m_Score = 0;
    bool m_GameOver = false;

    Player m_Player;
    Bullet m_Bullet;
    std::vector<Tube> m_Tubes;

    float m_WallThickness = 500.0f;
    float m_WallHeightOffset = 600.0f;
};
