#pragma once

#include "GameObjects/Player.h"
#include "GameObjects/Bullet.h"
#include "GameObjects/Rock.h"

class Level final
{
public:
    void OnAttach();
    void OnUpdate(float ts);
    void OnRender();
    void OnDetach();

    static float GetGravity() { return 9.8f; }

    bool IsGameOver() const { return m_GameOver; }
    const Player& GetPlayer() const { return m_Player; }

    uint64_t GetScore() const { return m_Score; }

    void Reset();
private:
    // Player Actions
    void ShootBullet();

    // Generation
    void GenerateBullet();
    void CreateRock();

    // Collision detection
    bool IsPlayerDead();
    ////bool IsMonsterKilled();
    void RenderBackground();
private:
    float m_Time = 0.0f;
    uint64_t m_Score = 0;
    bool m_GameOver = false;

    Player m_Player;
    Bullet m_Bullet;
    std::vector<Rock> m_Rocks;

    float m_WallThickness = 550.0f;
    float m_WallHeightOffset = 600.0f;
};
