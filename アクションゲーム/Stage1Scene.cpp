#include "Stage1Scene.h"
#include "Game.h"
//#include "sound.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Ground.h"
#include "Texture2D.h"
#include "Arrow.h"
#include "Pole.h"
#include "EnemyManager.h"

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
	m_Par = 4;// パー(標準打数)を設定
	m_StrokeCount = 0; // 現在打数を初期化

	// オブジェクトを作成
	ground = Game::GetInstance()->AddObject<Ground>();
	m_MySceneObjects.emplace_back(ground);
	groundsize = ground->GetGroundSize();

	//m_MySceneObjects.emplace_back(Game::GetInstance()->AddObject<Pole>()); // ポール

	for (int i = 0; i < 15; i++) {
		m_MySceneObjects.emplace_back(Game::GetInstance()->AddObject<Arrow>()); // 矢印
		Arrow* arrow = dynamic_cast<Arrow*>(m_MySceneObjects[m_MySceneObjects.size() - 1]); // 矢印
		arrow->SetState(0); // 矢印を非表示
	}
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
	// UI(HP)
	Texture2D* pt1 = Game::GetInstance()->AddObject<Texture2D>();
	pt1->SetTexture("assets/texture/ui_back.png"); // 画像を指定
	pt1->SetPosition(-560.0f, -300.0f, 0.0f); // 位置を設定
	pt1->SetScale(100.0f, 100.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(pt1);
	
	// UI(敵の数)
	Texture2D* pt2 = Game::GetInstance()->AddObject<Texture2D>();
	pt2->SetTexture("assets/texture/ui_string.png"); // 画像を指定
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

	player = Game::GetInstance()->AddObject<Player>();
	m_MySceneObjects.emplace_back(player);
	player->SetState(0);

	Sound::GetInstance()->Play(SOUND_BGM_MAIN);
}

//更新
void Stage1Scene::Update()
{
	//Arrow* arrow = dynamic_cast<Arrow*>(m_MySceneObjects[2]); // 矢印
	// 数を更新
	Texture2D* count[1];
	vector<Texture2D*> texture2D = Game::GetInstance()->GetObjects<Texture2D>();
	count[0] = texture2D[texture2D.size() - 3];//プレイヤーHP
	//count[1] = texture2D[texture2D.size() - 2];//敵の数2桁目
	//count[2] = texture2D[texture2D.size() - 1];//敵の数1桁目

	count[0]->SetUV(player->GetHP() + 1, 1, 10, 1); // プレイヤーHPのUVを指定
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

	// 状態ごとに処理
	switch (m_State) {

	//	ボール移動中
	case 0:
		// ボールが静止したら
		//if (ball->GetState() == 1)
		//{
		//	m_State = 1;
		//	arrow->SetState(m_State);

		//	// 打数を更新
		//	Texture2D* count[2];
		//	count[0] = dynamic_cast<Texture2D*>(m_MySceneObjects[8]);//現在打数の数値 一の位
		//	count[1] = dynamic_cast<Texture2D*>(m_MySceneObjects[9]);//現在打数の数値 十の位
		//	m_StrokeCount++; //現在打数をカウントアップ

		//	// 各桁を後ろから取得していく
		//	for (int i = 0; i < 2; i++) {
		//		int cnt = m_StrokeCount % (int)pow(10, i + 1) / (int)pow(10, i); // 1桁取り出す

		//		count[i]->SetUV(cnt + 1, 1, 10, 1); // UVを指定
		//	}
		//}
		//// ボールがカップインしたらリザルトへ
		//if (ball->GetState() == 2)
		//{
		//	Game::GetInstance()->ChangeScene(RESULT);
		//}
		break;
	// 方向選択中
	case 1:
		// スペースキーでパワー選択へ
		if (Input::GetKeyTrigger(VK_SPACE))
		{
			m_State = 2;
			//arrow->SetState(m_State);
		}
		break;
	// パワー選択中
	case 2:
		// スペースキーでショット
		/*if (Input::GetKeyTrigger(VK_SPACE))
		{
			m_State = 0;
			ball->SetState(m_State);
			arrow->SetState(m_State);

			Vector3 v = arrow->GetVector();
			ball->Shot(v);
		}*/
		break;
	}

	//if (enemycount <= 0) {
	boss->HitCheckPole(player->GetWeapon());
	player->CheckHitPole(boss->GetWeapon());
	if(boss->GetHP() <= 0){
		Game::GetInstance()->ChangeScene(RESULT);
		Sound::GetInstance()->Stop(SOUND_BGM_MAIN);
	}else if (player->GetHP() <= 0) {
		Game::GetInstance()->ChangeScene(GAMEOVER);
		Sound::GetInstance()->Stop(SOUND_BGM_MAIN);
	}
}

// 終了処理
void Stage1Scene::Uninit()
{
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance()->DeleteObject(o);
	}
	Game::GetInstance()->DeleteAllObject();
}

// スコアを取得
int Stage1Scene::GetScore()
{
	// 現在打数から標準打数ｗｐ引いた数値をreturn
	return (m_StrokeCount - m_Par);
}

void Stage1Scene::Collision() {
	
}