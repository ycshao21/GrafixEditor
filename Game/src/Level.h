#pragma once

#include "GameObjects/Player.h"
#include "GameObjects/Bullet.h"
#include "GameObjects/Rock.h"
#include "GameObjects/Fish.h"

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
    // Generation
    void GenerateBullet();
    void GenerateRock();
    void GenerateFish();

    // Collision detection
    bool IsPlayerDead();
    void BulletCollisionDetection();

    ////bool IsMonsterKilled();
    void RenderWalls();
    void RenderRocks();
private:
    float m_Time = 0.0f;
    uint64_t m_Score = 0;
    bool m_GameOver = false;

    Player m_Player;
    Bullet m_Bullet;

    std::vector<Fish> m_Fishes;
    std::vector<Rock> m_Rocks;

    float m_WallThickness = 400.0f;
    float m_WallHeightOffset = 325.0f;
};
