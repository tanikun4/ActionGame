#include "EnemyManager.h"
#include "Game.h"
#include "Enemy.h"

using namespace std;

int EnemyManager::EnemyCount()
{
	int count = 0;
	vector<Enemy*>  en = Game::GetInstance()->GetObjects<Enemy>();
	for (auto& e : m_enemies) {
		if (e->GetLive()) {
			count++;
		}
	}
	return count;
}

void EnemyManager::SetEnemy(int num) {
	for (int i = 0; i < num; ++i) {
		//Game‘¤‚ÌInitˆ—‚Æ‹£‡‚·‚é‰Â”\«‘åAŽg‚¤Û‚ÍŒ©’¼‚·‚±‚Æ
		m_enemies.emplace_back(Game::GetInstance()->AddObject<Enemy>());
		//m_enemies.back()->SetStageSize(m_stagesize);
		m_enemies.back()->Init();
	}
}

const vector<unique_ptr<Enemy>>& EnemyManager::GetEnemies() {
	return m_enemies;
}