#include "Stage1Scene.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Ground.h"
#include "Texture2D.h"
#include "Projectile.h"
#include "Bullet.h"
#include "Pole.h"
#include "EnemyManager.h"
#include "GroundManager.h"
#include "ICollider.h"
//#include "Skydome.h"
#include "Fade.h"

#include "DebugUI.h"

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
	Sound::GetInstance()->SetMasterVolume(1.0f); // 全体音量を100%に

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

	boss = Game::GetInstance()->AddObject<Boss>();
	m_MySceneObjects.emplace_back(boss);
	boss->SetDEF(0);//無敵状態解除

	//通常敵の配置
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
	player->SetDemoMode(false);

	for (int i = 0; i < 3; i++) {
		m_MySceneObjects.emplace_back(Game::GetInstance()->AddObject<Bullet>()); //弾
		Bullet* bullet = dynamic_cast<Bullet*>(m_MySceneObjects.back()); //弾
		bullet->SetState(0); // //弾を非表示
	}

	// 武器の取得
	std::vector<Pole*> weapons = Game::GetInstance()->GetObjects<Pole>();
	m_MySceneObjects.insert(
		m_MySceneObjects.end(),    // 挿入位置
		weapons.begin(),        // 挿入する範囲の開始
		weapons.end()           // 挿入する範囲の終了
	);

	// 飛び道具の取得
	std::vector<Projectile*> projectiles = Game::GetInstance()->GetObjects<Projectile>();
	m_MySceneObjects.insert(
		m_MySceneObjects.end(),    // 挿入位置
		projectiles.begin(),        // 挿入する範囲の開始
		projectiles.end()           // 挿入する範囲の終了
	);

	//壁の設置
	SetWall();

	// UI(HP)
	Texture2D* pt1 = Game::GetInstance()->AddObject<Texture2D>();
	pt1->SetTexture("assets/texture/ui_back.png"); // 画像を指定
	pt1->SetPosition(-560.0f, -300.0f, 0.0f); // 位置を設定
	pt1->SetScale(100.0f, 100.0f, 0.0f); // 大きさを指定d
	m_MySceneObjects.emplace_back(pt1);

	// UI(ボスHP文字)
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

	// UI(ボスHP 1桁目)
	Texture2D* pt5 = Game::GetInstance()->AddObject<Texture2D>();
	pt5->SetTexture("assets/texture/number.png"); // 画像を指定
	pt5->SetPosition(565.0f, 300.0f, 0.0f); // 位置を設定
	pt5->SetScale(95.0f, 72.0f, 0.0f); // 大きさを指定
	pt5->SetUV(2, 1, 10, 1); //UVを指定
	m_MySceneObjects.emplace_back(pt5);

	// UI(ボスHP 2桁目)
	Texture2D* pt6 = Game::GetInstance()->AddObject<Texture2D>();
	pt6->SetTexture("assets/texture/number.png"); // 画像を指定
	pt6->SetPosition(485.0f, 300.0f, 0.0f); // 位置を設定
	pt6->SetScale(95.0f, 72.0f, 0.0f); // 大きさを指定
	pt6->SetUV(1, 1, 10, 1); //UVを指定
	m_MySceneObjects.emplace_back(pt6);

	Sound::GetInstance()->Play(SOUND_BGM_MAIN);
	Sound::GetInstance()->SetVolume(SOUND_BGM_MAIN, 0.5f);
	Sound::GetInstance()->SetMasterVolume(0.0f);
	Fade::GetInstance()->StartFadeIn();
	Game::GetInstance()->GetCamera().SetTarget(*player);

	// 以前のカメラ開始位置（ターゲットから見た相対位置）
	Vector3 initialOffset(0.0f, 80.0f, -180.0f);
	initialOffset.Normalize();

	// yaw, pitch を計算
	Vector2 CameraDirection;
	CameraDirection.x = 0;//PI - atan2(initialOffset.x, initialOffset.z); // yaw
	CameraDirection.y = -2.2f;//PI + asin(initialOffset.y);                   // pitch

	Game::GetInstance()->GetCamera().SetDirection(CameraDirection);//カメラ方向設定
	Game::GetInstance()->GetCamera().SetInputFg(true);//カメラ操作有効化

	DebugUI::RedistDebugFunction([this]() {
		DebugWallStatus();
		});
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

	Game::GetInstance()->CollisionObject(m_MySceneObjects);

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
		if (o) o->Uninit();
	}
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		if(o) Game::GetInstance()->DeleteObject(o);
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

void Stage1Scene::SetWall() {
	// Ground のサイズ
	float halfX = groundsize.x / 2.0f;
	float halfZ = groundsize.y / 2.0f;

	// Cube の厚み（Z方向とX方向で壁として使う分）
	float thickness = 1.0f; // 例：10 units
	//Cubeの縦の厚み
	float scaleY = 1.5f;
	//初期色
	Vector4 color{ 1,1,1,0.5f };//透明度を下げて見やすいようにしておく

	// 上辺（+Z側）
	TestCube* top = Game::GetInstance()->AddObject<TestCube>();
	top->SetScale(Vector3(groundsize.x * 0.05f, scaleY, thickness)); // X幅は地面いっぱい、Zは薄く
	top->SetPosition(Vector3(-halfX * 0.1f, scaleY * 10.0f, halfZ + halfZ * 0.1f)); // Yは高さ調整
	top->SetColor(color);
	m_MySceneObjects.emplace_back(top);

	// 下辺（-Z側）
	TestCube* bottom = Game::GetInstance()->AddObject<TestCube>();
	bottom->SetScale(Vector3(groundsize.x * 0.05f, scaleY, thickness));
	bottom->SetPosition(Vector3(-halfX * 0.1f, scaleY * 10.0f, -halfZ));
	bottom->SetColor(color);
	m_MySceneObjects.emplace_back(bottom);

	// 右辺（+X側）
	TestCube* right = Game::GetInstance()->AddObject<TestCube>();
	right->SetScale(Vector3(thickness, scaleY, groundsize.y * 0.05f));
	right->SetPosition(Vector3(halfX, scaleY * 10.0f, 0.0f));
	right->SetColor(color);
	m_MySceneObjects.emplace_back(right);

	// 左辺（-X側）
	TestCube* left = Game::GetInstance()->AddObject<TestCube>();
	left->SetScale(Vector3(thickness, scaleY, groundsize.y * 0.05f));
	left->SetPosition(Vector3(-halfX - halfX * 0.15f, scaleY * 10.0f, 0.0f));
	left->SetColor(color);
	m_MySceneObjects.emplace_back(left);
}

void Stage1Scene::DebugWallStatus() {//壁の大きさや位置を操作する
	ImGui::Begin("WallStatus");

	// 元の値を保持する static 配列
	static bool initialized = false;
	static std::vector<Vector3> originalScale;
	static std::vector<Vector3> originalPos;

	// TestCube を取得
	auto cubes = Game::GetInstance()->GetObjects<TestCube>();

	// 初期値を保存
	if (!initialized) {
		originalScale.clear();
		originalPos.clear();
		for (auto& c : cubes) {
			originalScale.push_back(c->GetScale());
			originalPos.push_back(c->GetPosition());
		}
		initialized = true;
	}

	static Vector3 wall_size{};
	ImGui::SliderFloat3("WallSize", &wall_size.x, -10.0f, 10.0f);

	static Vector3 wall_pos{};
	ImGui::SliderFloat3("WallPosition", &wall_pos.x, -100.0f, 100.0f);

	static Vector4 wall_color {1,1,1,0.5f};
	ImGui::SliderFloat4("WallColor", &wall_color.x, 0.0f, 1.0f);

	if (ImGui::Button("Reset Status")) {
		wall_size = Vector3(0, 0, 0);
		wall_pos = Vector3(0, 0, 0);
		wall_color = Vector4(1, 1, 1, 0.5f);
	}

	vector<TestCube*> cube = Game::GetInstance()->GetObjects<TestCube>();

	// 元の値にスライダー分だけ加える
	for (size_t i = 0; i < cubes.size(); ++i) {
		cubes[i]->SetScale(originalScale[i] + wall_size);
		cubes[i]->SetPosition(originalPos[i] + wall_pos);
		cubes[i]->SetColor(wall_color);
	}

	ImGui::End();
}