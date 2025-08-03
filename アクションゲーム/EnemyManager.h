#pragma once
#include "Manager.h"
#include <DirectXMath.h>
#include <vector>
#include <memory>

class Enemy;

class EnemyManager :
    public Manager<EnemyManager>
{
public:
    friend class Manager<EnemyManager>;
    int EnemyCount();
    void SetEnemy(int num);
    void Update();
    void Draw();
    void Uninit();
    const std::vector<std::unique_ptr<Enemy>>& GetEnemies();
private:
    //EnemyManager() {};
    //~EnemyManager() {};
    DirectX::XMFLOAT2 m_stagesize = { 0.0f,0.0f };
    std::vector<std::unique_ptr<Enemy>> m_enemies;
};

