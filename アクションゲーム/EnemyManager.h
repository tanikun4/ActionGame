#pragma once
#include "Manager.h"
#include "MathCommon.h"
#include <vector>
#include <memory>

constexpr int ENEMY_POOLSIZE = 5;

class Object;
class Enemy;

class EnemyManager :
    public Manager<EnemyManager>
{
public:
    friend class Manager<EnemyManager>;
    int EnemyCount();
    void SetEnemy(int num,const DirectX::SimpleMath::Vector3& spawnrange);
    static void Init();
    void Update();
    void Draw();
    void Uninit();
    static EnemyManager* GetInstance();
    void SetTarget(Object* ta);
    const std::vector<std::unique_ptr<Enemy>>& GetEnemies();
private:
    //EnemyManager() {};
    //~EnemyManager() {};
    DirectX::XMFLOAT2 m_stagesize = { 0.0f,0.0f };
    std::vector<std::unique_ptr<Enemy>> m_enemies;
    static std::unique_ptr<EnemyManager> m_Instance; // ゲームインスタンス
};

