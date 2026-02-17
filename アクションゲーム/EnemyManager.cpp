#include "EnemyManager.h"
#include "Game.h"
#include "Enemy.h"
#include "RandomCommon.h"
#include "Player.h"
#include "DebugUI.h"

// static メンバの実体定義
std::unique_ptr<EnemyManager> EnemyManager::m_Instance = nullptr;

using namespace std;
using namespace DirectX::SimpleMath;

// 初期化処理、デバッグ関数を登録する
void EnemyManager::Init() {
    DebugUI::RedistDebugFunction([this]() { DebugEnemyManager(); });
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

int EnemyManager::GetLiveEnemy()
{
    return m_livecount;
}

void EnemyManager::EnemyDeath()
{
    --m_livecount;
    if(m_livecount < 0)
		m_livecount = 0;
}

// 敵の配置
void EnemyManager::SetEnemy(int num, const Vector3& spawnrange) {
    if (num > ENEMY_POOLSIZE)
        num = ENEMY_POOLSIZE;

    constexpr float MIN_DISTANCE = 40.0f; // 敵同士の最小距離
    constexpr int   MAX_TRY = 30;          // 引き直し上限

    std::vector<Vector3> placedPositions;
    placedPositions.reserve(num);

	// プレイヤーの位置も対象にする
    Player* player = Game::GetInstance()->GetObjects<Player>()[0];
    if (player)
    {
        placedPositions.emplace_back(player->GetPosition());
    }

    for (int i = 0; i < num; ++i)
    {
        Vector3 pos;
        bool placed = false;

        for (int t = 0; t < MAX_TRY; ++t)
        {
            pos.x = RandomRange(-spawnrange.x, spawnrange.x);
            pos.z = RandomRange(-spawnrange.z, spawnrange.z);
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
        ++m_livecount;// 生存数を増やす
    }

    // 攻撃タイミングを設定
    attackframe = baseInterval + RandomRange(-randomRange, randomRange);
}

const vector<Enemy*> EnemyManager::GetEnemies() {
	return m_enemies;
}
void EnemyManager::Update() {
    --attackframe;

    if (attackframe <= 0)
    {
		AttackEnemy();
        // 次の攻撃タイミングを設定
        attackframe = baseInterval + RandomRange(-randomRange, randomRange);
    }
}

void EnemyManager::Uninit() {
	m_enemies.clear();
}

void EnemyManager::AttackEnemy() {
    int current_attacker = 0;

    std::vector<Enemy*> candidates;
    for (auto* e : m_enemies)
    {

        // 生存していなければスキップ
        if (!e->GetLive())
            continue;

        if (e->IsAttacking()) {
            ++current_attacker;
            if (current_attacker >= max_attacker)
                return;
        }

        if (e->IsAttackable())
            candidates.emplace_back(e);
    }

    if (candidates.empty())
        return;

	// ランダムに攻撃する敵を選択
    Enemy* chosen = candidates[rand() % candidates.size()];
    chosen->Attack(); // 外部攻撃開始
}

// 敵同士で距離を取る関数
bool EnemyManager::NearDistance(
    const Enemy* self,
    Vector3* awaydir
) const
{
    bool found = false;
    Vector3 awaySum = Vector3::Zero;

    for (auto* e : m_enemies)
    {

		// 自分自身は無視
        if (e == self)
            continue;

		// 生存していなければ対象にしない
        if (!e->GetLive())
            continue;

		// 距離を取っている最中なら対象にしない
        if(e->IsAway())
			continue;

        Vector3 diff = self->GetPosition() - e->GetPosition();
        float distSq = diff.LengthSquared();

        if (distSq < ENEMY_MIN_DISTANCE * ENEMY_MIN_DISTANCE)
        {
            found = true;

            // 方向が欲しい場合だけ計算
            if (awaydir)
                awaySum += diff;
        }
    }

    // 方向出力が求められていて、敵がいた場合
    if (awaydir)
    {
        if (found && awaySum.LengthSquared() > 0.0001f)
        {
            awaySum.Normalize();
            *awaydir = awaySum;
        }
        else
        {
            *awaydir = Vector3::Zero;
        }
    }

    return found;
}

// プレイヤー状態の操作
void EnemyManager::DebugEnemyManager() {
    ImGui::Begin("EnemyStatus");

    if (ImGui::Button("Enemy Delete"))
    {
		// 全敵を倒す
        for (auto* e : m_enemies)
        {
			e->Death();
        }
    }

    ImGui::End();
}


