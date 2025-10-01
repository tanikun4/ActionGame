#include "Stage1Scene.h"
#include "Game.h"
//#include "sound.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Ground.h"
#include "Texture2D.h"
#include "Arrow.h"
#include "Bullet.h"
#include "Pole.h"
#include "EnemyManager.h"
#include "GroundManager.h"
#include "ICollider.h"
#include "Skydome.h"
#include "Fade.h"
#include "imgui.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
Stage1Scene::Stage1Scene()
{
	Init();
}

// デストラクタ
Stage1Scene::~Stage1Scene()
{
	Uninit();
}

// 初期化
void Stage1Scene::Init()
{
	srand((unsigned)time(NULL));

	// オブジェクトを作成

	// 背景
	Texture2D* background = Game::GetInstance()->AddObject<Texture2D>();
	background->SetTexture("assets/texture/sky.png"); // 画像を指定
	background->SetPosition(-0.0f, 0.0f, 0.0f); // 位置を設定
	background->SetScale(1280.0f, 720.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(background);

	ground = Game::GetInstance()->AddObject<Ground>();
	m_MySceneObjects.emplace_back(ground);
	groundsize = ground->GetGroundSize();

	GroundManager::GetInstance().Init();
	//m_MySceneObjects.emplace_back(Game::GetInstance()->AddObject<Pole>()); // ポール

	boss = Game::GetInstance()->AddObject<Boss>();
	m_MySceneObjects.emplace_back(boss);

	//for (int i = 0; i < 9; i++) {
	//	int r1 = rand();
	//	int r2 = rand();
	//	m_MySceneObjects.emplace_back(Game::GetInstance()->AddObject<Enemy>()); // 敵
	//	Enemy* enemy = dynamic_cast<Enemy*>(m_MySceneObjects[m_MySceneObjects.size() - 1]); // 敵
	//	if (r1 % 2 == 0) {
	//		r1 *= -1;
	//	}
	//	if (r2 % 2 == 0) {
	//		r2 *= -1;
	//	}
	//	r1 %= (int)groundsize.x / 2;
	//	r2 %= (int)groundsize.y / 2;
	//	if (r1 < 50 && r2 < 50) {
	//		switch (rand() % 4) {
	//		case 0:
	//			r1 += 50;
	//			break;
	//		case 1:
	//			r2 += 50;
	//			break;
	//		case 3:
	//			r1 -= 50;
	//			break;
	//		case 4:
	//			r2 -= 50;
	//			break;
	//		}
	//	}
	//	enemy->SetPosition({(float)r1 ,50.0f,(float)r2});
	//	enemy->SetStageSize(groundsize / 2);
	//}

	player = Game::GetInstance()->AddObject<Player>();
	m_MySceneObjects.emplace_back(player);
	player->SetState(0);

	for (int i = 0; i < 3; i++) {
		m_MySceneObjects.emplace_back(Game::GetInstance()->AddObject<Bullet>()); // 矢印
		Bullet* bullet = dynamic_cast<Bullet*>(m_MySceneObjects[m_MySceneObjects.size() - 1]); // 矢印
		bullet->SetState(0); // 矢印を非表示
	}

	std::vector<Weapon*> weapons = Game::GetInstance()->GetObjects<Weapon>();
	m_MySceneObjects.insert(
		m_MySceneObjects.end(),    // 挿入位置
		weapons.begin(),        // 挿入する範囲の開始
		weapons.end()           // 挿入する範囲の終了
	);


	// UI(HP)
	Texture2D* pt1 = Game::GetInstance()->AddObject<Texture2D>();
	pt1->SetTexture("assets/texture/ui_back.png"); // 画像を指定
	pt1->SetPosition(-560.0f, -300.0f, 0.0f); // 位置を設定
	pt1->SetScale(100.0f, 100.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(pt1);
	
	// UI(敵の数)
	Texture2D* pt2 = Game::GetInstance()->AddObject<Texture2D>();
	pt2->SetTexture("assets/texture/ui_BossHP.png"); // 画像を指定
	pt2->SetPosition(350.0f, 300.0f, 0.0f); // 位置を設定
	pt2->SetScale(150.0f, 75.0f, 0.0f); // 大きさを指定
	pt2->SetUV(1, 1, 1, 1); //UVを指定
	m_MySceneObjects.emplace_back(pt2);

	// UI(プレイヤーHP)
	Texture2D* pt4 = Game::GetInstance()->AddObject<Texture2D>();
	pt4->SetTexture("assets/texture/number.png"); // 画像を指定
	pt4->SetPosition(-485.0f, -300.0f, 0.0f); // 位置を設定
	pt4->SetScale(65.0f, 45.0f, 0.0f); // 大きさを指定
	pt4->SetUV(m_Par + 1, 1, 10, 1); //UVを指定
	m_MySceneObjects.emplace_back(pt4);

	// UI(敵の数 1桁目)
	Texture2D* pt5 = Game::GetInstance()->AddObject<Texture2D>();
	pt5->SetTexture("assets/texture/number.png"); // 画像を指定
	pt5->SetPosition(565.0f, 300.0f, 0.0f); // 位置を設定
	pt5->SetScale(95.0f, 72.0f, 0.0f); // 大きさを指定
	pt5->SetUV(2, 1, 10, 1); //UVを指定
	m_MySceneObjects.emplace_back(pt5);

	// UI(敵の数 2桁目)
	Texture2D* pt6 = Game::GetInstance()->AddObject<Texture2D>();
	pt6->SetTexture("assets/texture/number.png"); // 画像を指定
	pt6->SetPosition(485.0f, 300.0f, 0.0f); // 位置を設定
	pt6->SetScale(95.0f, 72.0f, 0.0f); // 大きさを指定
	pt6->SetUV(1, 1, 10, 1); //UVを指定
	m_MySceneObjects.emplace_back(pt6);


	Sound::GetInstance()->Play(SOUND_BGM_MAIN);
	Fade::GetInstance()->StartFadeIn();

}

//更新
void Stage1Scene::Update()
{
	//Arrow* arrow = dynamic_cast<Arrow*>(m_MySceneObjects[2]); // 矢印
	// 数を更新
	Texture2D* count[3];
	vector<Texture2D*> texture2D = Game::GetInstance()->GetObjects<Texture2D>();
	count[0] = texture2D[texture2D.size() - 3];//プレイヤーHP
	count[1] = texture2D[texture2D.size() - 2];//ボスのHP1桁目
	count[2] = texture2D[texture2D.size() - 1];//ボスのHP2桁目

	count[0]->SetUV(player->GetHP() + 1, 1, 10, 1); // プレイヤーHPのUVを指定
	count[1]->SetUV((boss->GetHP() % 10) + 1, 1, 10, 1); // ボスHP1桁目のUVを指定
	count[2]->SetUV((boss->GetHP() / 10) + 1, 1, 10, 1); // ボスHP2桁目のUVを指定
	// 各桁を後ろから取得していく
	//vector<Enemy*> enemy = Game::GetInstance()->GetObjects<Enemy>();
	//int enemycount = 0;
	//for (auto& en : enemy) {
	//	if (en->GetLive()) {
	//		enemycount++;
	//	}
	//}
	//for (int i = 0; i < 2; i++) {
	//	int cnt = enemycount % (int)pow(10, i + 1) / (int)pow(10, i); // 1桁取り出す

	//	count[i + 1]->SetUV(cnt + 1, 1, 10, 1);  // 敵の数のUVを指定
	//}
	//count[1]->SetUV(EnemyManager::GetInstance().EnemyCount() + 1, 1, 10, 1); // 敵の数のUVを指定

	Collision();

	if(boss->GetHP() <= 0){
		Game::GetInstance()->ChangeSceneFadeOut(RESULT);
		Sound::GetInstance()->Stop(SOUND_BGM_MAIN);
		count[1]->SetUV(1, 1, 10, 1);
		count[2]->SetUV(1, 1, 10, 1);
	}else if (player->GetHP() <= 0) {
		Game::GetInstance()->ChangeSceneFadeOut(GAMEOVER);
		Sound::GetInstance()->Stop(SOUND_BGM_MAIN);
	}
}

// 終了処理
void Stage1Scene::Uninit()
{

	for (auto& o : m_MySceneObjects) {//ループ中にポインタを削除するとバグるので、終了処理のみを先に行う
		o->Uninit();
	}
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance()->DeleteObject(o);
		o = nullptr;
	}
	m_MySceneObjects.clear();
}

// スコアを取得
int Stage1Scene::GetScore()
{
	// 現在打数から標準打数ｗｐ引いた数値をreturn
	return (m_StrokeCount - m_Par);
}

void Stage1Scene::Collision() {
	for (size_t i = 0; i < m_MySceneObjects.size(); ++i) {
		for (size_t j = i + 1; j < m_MySceneObjects.size(); ++j) {
			auto a = m_MySceneObjects[i];
			auto b = m_MySceneObjects[j];

			if (!(a->GetLive() && b->GetLive())) { continue; };

			auto col_a = dynamic_cast<ICollider*>(a);
			auto col_b = dynamic_cast<ICollider*>(b);
			if (col_a && col_b) {
				if (Collision::CheckHit(col_a->GetCollision(), col_b->GetCollision())) {
					a->HitObject(b);
					b->HitObject(a);
				}
			}
		}
	}
}