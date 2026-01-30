#pragma once
#include "Manager.h"
#include "MathCommon.h"
#include <vector>
#include <memory>

constexpr int ENEMY_POOLSIZE = 5;
constexpr float ENEMY_MIN_DISTANCE = 20.0f;

class Object;
class Enemy;

class EnemyManager :
    public Manager<EnemyManager>
{
public:
    friend class Manager<EnemyManager>;
    int GetLiveEnemy();// “G‚Ì¶‘¶”‚ğ•Ô‚·
    void SetEnemy(int num,const DirectX::SimpleMath::Vector3& spawnrange);
	void EnemyDeath();//“G‚ª“|‚³‚ê‚½‚ÉŒÄ‚Ño‚·
    //static void Init();
    void Update();
    void Draw();
    void Uninit();
    //static EnemyManager* GetInstance();
    void AddEnemys();
    void SetTarget(Object* ta);
    const std::vector<Enemy*> GetEnemies();
    bool NearDistance(const Enemy* self ,DirectX::SimpleMath::Vector3* awaydir = nullptr) const;
private:
    //EnemyManager() {};
    //~EnemyManager() {};
    DirectX::XMFLOAT2 m_stagesize = { 0.0f,0.0f };
    std::vector<Enemy*> m_enemies;
    static std::unique_ptr<EnemyManager> m_Instance; // ƒQ[ƒ€ƒCƒ“ƒXƒ^ƒ“ƒX

	int max_attacker = 1; //“¯‚ÉUŒ‚‚·‚é“G‚ÌÅ‘å”
	int current_attacker = 0;// Œ»İUŒ‚’†‚Ì“G‚Ì”

	int m_livecount = 0;// ¶‘¶‚µ‚Ä‚¢‚é“G‚Ì”

	int attackframe = 0; // UŒ‚—pƒtƒŒ[ƒ€ƒJƒEƒ“ƒg
	int baseInterval = 240; // “G‚ÌUŒ‚ŠÔŠu‚ÌŠî€’l
	int randomRange = 60; // “G‚ÌUŒ‚ŠÔŠu‚Ìƒ‰ƒ“ƒ_ƒ€”ÍˆÍ
	void AttackEnemy();//UŒ‚w¦‚ğs‚¤
};

