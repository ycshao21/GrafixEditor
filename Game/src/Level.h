#pragma once

#include "GameObjects/Player.h"
#include "GameObjects/Bullet.h"
#include "GameObjects/Seamount.h"
#include "GameObjects/Fish.h"
#include "GameObjects/Coin.h"
#include "GameObjects/BulletItem.h"

class Level final
{
    using SeamountPair = std::pair<Seamount, Seamount>;
public:
    void Init();
    void OnUpdate(float ts);
    void OnRender();
    void OnDetach();

    static float GetGravity() { return 9.8f; }

    bool IsGameOver() const { return m_GameOver; }
    const Player& GetPlayer() const { return m_Player; }

    uint64_t GetScore() const { return m_Score; }

    void Reset();
private:
    void GenerateSeamount(int index, float x);
    void GenerateFish(int index, float x);
    void GenerateCoin();
    void GenerateBulletItem();

    ////bool IsMonsterKilled();

    // Collision detection
    bool IsOutOfScreen(GameObject& gameobject);
    bool IsItemCollected(GameObject& gameobject);
    bool IsPlayerDead();
    void BulletCollisionDetection();

    void RenderWalls();
private:
    uint64_t m_Score = 0;
    bool m_GameOver = false;

    Player m_Player;
    Bullet m_Bullet;

    std::vector<SeamountPair> m_Seamounts;
    const float m_SeamountGap = 520.0f;
    int m_IndexOfNextSeamount = 0;
    int m_IndexOfSeamountToGen = 0;
    float m_SeamountGenDetectX = 2.0f * m_SeamountGap;

    // Items
    Coin m_Coin;
    BulletItem m_BulletItem;


    std::vector<Fish> m_Fishes;
    int m_IndexOfFirstFish = 0;

    float m_WallThickness = 400.0f;
    float m_WallHeightOffset = 325.0f;
};
