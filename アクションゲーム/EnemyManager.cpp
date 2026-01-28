#include "EnemyManager.h"
#include "Game.h"
#include "Enemy.h"

// static メンバの実体定義
std::unique_ptr<EnemyManager> EnemyManager::m_Instance = nullptr;

using namespace std;
using namespace DirectX::SimpleMath;

void EnemyManager::Init() {
	if (m_Instance) return; // 二重初期化防止
	m_Instance = make_unique<EnemyManager>();
}

EnemyManager* EnemyManager::GetInstance() {
	return m_Instance.get();
}

// 敵オブジェクトを確保する
void EnemyManager::AddEnemys()
{
    m_enemies.clear();
    for (int i = 0; i < ENEMY_POOLSIZE; ++i) {
        m_enemies.emplace_back(Game::GetInstance()->AddObject<Enemy>());
        m_enemies.back()->SetLive(false);
    }
}

void EnemyManager::SetTarget(Object* ta)
{
    for (auto& e : m_enemies) {
         e->SetTarget(ta);
    }
}

int EnemyManager::EnemyCount()
{
	int count = 0;
	for (auto& e : m_enemies) {
		if (e->GetLive()) {
			++count;
		}
	}
	return count;
}

void EnemyManager::SetEnemy(int num, const Vector3& spawnrange) {
    if (num > ENEMY_POOLSIZE)
        num = ENEMY_POOLSIZE;

    constexpr float MIN_DISTANCE = 20.0f; // 敵同士の最小距離
    constexpr int   MAX_TRY = 30;          // 引き直し上限

    std::vector<Vector3> placedPositions;
    placedPositions.reserve(num);

    for (int i = 0; i < num; ++i)
    {
        Vector3 pos;
        bool placed = false;

        for (int t = 0; t < MAX_TRY; ++t)
        {
            pos.x = RandRange(-spawnrange.x, spawnrange.x);
            pos.z = RandRange(-spawnrange.z, spawnrange.z);
            pos.y = spawnrange.y;

            bool tooClose = false;
            for (const auto& p : placedPositions)
            {
                if ((p - pos).Length() < MIN_DISTANCE)
                {
                    tooClose = true;
                    break;
                }
            }

            if (!tooClose)
            {
                placed = true;
                break;
            }
        }

        if (!placed)
            continue; // 置けなかった敵はスキップ or 妥協

        placedPositions.emplace_back(pos);

        // 敵の配置・初期化
        m_enemies[i]->SetPosition(pos);
        m_enemies[i]->ReInit();
    }
}

const vector<Enemy*> EnemyManager::GetEnemies() {
	return m_enemies;
}