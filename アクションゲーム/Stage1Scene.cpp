#include "Stage1Scene.h"
#include "Game.h"
#include "Player.h"
#include "Enemy.h"
#include "Boss.h"
#include "Ground.h"
#include "Texture2D.h"
#include "Projectile.h"
#include "Bullet.h"
#include "Sword.h"
#include "EnemyManager.h"
#include "GroundManager.h"
#include "WallManager.h"
#include "ICollider.h"
#include "EffectManager.h"
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

	EnemyManager::GetInstance().Init();
	EnemyManager::GetInstance().AddEnemys();


	// プレイヤーの配置
	player = Game::GetInstance()->AddObject<Player>();
	m_MySceneObjects.emplace_back(player);
	player->SetDemoMode(false);

	// 敵の配置
	//EnemyManager::GetInstance().SetEnemy(5, { (groundsize.x - 50) * 0.5f,30,(groundsize.y - 50) * 0.5f });
	EnemyManager::GetInstance().SetTarget(player);

	// 敵の取得
	vector<Enemy*> enemy = Game::GetInstance()->GetObjects<Enemy>();
	m_MySceneObjects.insert(
		m_MySceneObjects.end(),    // 挿入位置
		enemy.begin(),        // 挿入する範囲の開始
		enemy.end()           // 挿入する範囲の終了
	);

	boss = Game::GetInstance()->AddObject<Boss>();
	m_MySceneObjects.emplace_back(boss);
	boss->SetTarget(player);
	boss->SetLive(false); // 最初は非表示

	//壁の設置
	WallManager::SetWall(ground->GetGroundSize(), m_MySceneObjects);

	// 弾の取得
	for (int i = 0; i < 3; i++) {
		m_MySceneObjects.emplace_back(Game::GetInstance()->AddObject<Bullet>()); //弾
		Bullet* bullet = dynamic_cast<Bullet*>(m_MySceneObjects.back()); //弾
		bullet->SetState(0); // //弾を非表示
	}

	// 武器の取得
	vector<Sword*> weapons = Game::GetInstance()->GetObjects<Sword>();
	m_MySceneObjects.insert(
		m_MySceneObjects.end(),    // 挿入位置
		weapons.begin(),        // 挿入する範囲の開始
		weapons.end()           // 挿入する範囲の終了
	);

	// 飛び道具の取得
	vector<Projectile*> projectiles = Game::GetInstance()->GetObjects<Projectile>();
	m_MySceneObjects.insert(
		m_MySceneObjects.end(),    // 挿入位置
		projectiles.begin(),        // 挿入する範囲の開始
		projectiles.end()           // 挿入する範囲の終了
	);

	// ここからUI関連

	// ゲージの設定
	player->SetGauge();
	boss->SetGauge();
	boss->SetGaugeLive(false);

	// UI(HP文字)
	Texture2D* pt1 = Game::GetInstance()->AddObject<Texture2D>();
	pt1->SetTexture("assets/texture/hp_ui.png"); // 画像を指定
	pt1->SetPosition(-580.0f, -320.0f, 0.0f); // 位置を設定
	pt1->SetScale(100.0f, 75.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(pt1);

	// UI(ボスHP文字)
	boss_hp_text = Game::GetInstance()->AddObject<Texture2D>();
	boss_hp_text->SetTexture("assets/texture/bosshp_ui.png"); // 画像を指定
	boss_hp_text->SetPosition(-300.0f, 335.0f, 0.0f); // 位置を設定
	boss_hp_text->SetScale(180.0f, 60.0f, 0.0f); // 大きさを指定
	boss_hp_text->SetUV(1, 1, 1, 1); //UVを指定
	m_MySceneObjects.emplace_back(boss_hp_text);
	boss_hp_text->SetLive(false); // 最初は非表示

	// UI(ガードゲージ文字)
	boss_guard_text = Game::GetInstance()->AddObject<Texture2D>();
	boss_guard_text->SetTexture("assets/texture/guard_ui.png"); // 画像を指定
	boss_guard_text->SetPosition(-100.0f, 305.0f, 0.0f); // 位置を設定
	boss_guard_text->SetScale(150.0f, 50.0f, 0.0f); // 大きさを指定
	boss_guard_text->SetUV(1, 1, 1, 1); //UVを指定
	m_MySceneObjects.emplace_back(boss_guard_text);
	boss_guard_text->SetLive(false); // 最初は非表示

	// UI(Wave1文字)
	wave1_text = Game::GetInstance()->AddObject<Texture2D>();
	wave1_text->SetTexture("assets/texture/Wave1.png"); // 画像を指定
	wave1_text->SetPosition(0.0f, 0.0f, 0.0f); // 位置を設定
	wave1_text->SetScale(600.0f, 200.0f, 0.0f); // 大きさを指定
	wave1_text->SetUV(1, 1, 1, 1); //UVを指定
	m_MySceneObjects.emplace_back(wave1_text);
	wave1_text->SetLive(true); // wave1は最初表示

	// UI(Wave2文字)
	wave2_text = Game::GetInstance()->AddObject<Texture2D>();
	wave2_text->SetTexture("assets/texture/Wave2.png"); // 画像を指定
	wave2_text->SetPosition(0.0f, 0.0f, 0.0f); // 位置を設定
	wave2_text->SetScale(600.0f, 200.0f, 0.0f); // 大きさを指定
	wave2_text->SetUV(1, 1, 1, 1); //UVを指定
	m_MySceneObjects.emplace_back(wave2_text);
	wave2_text->SetLive(false); // wave2は非表示

	// UI(FinalWave文字)
	finalwave_text = Game::GetInstance()->AddObject<Texture2D>();
	finalwave_text->SetTexture("assets/texture/FinalWave.png"); // 画像を指定
	finalwave_text->SetPosition(0.0f, 0.0f, 0.0f); // 位置を設定
	finalwave_text->SetScale(800.0f, 400.0f, 0.0f); // 大きさを指定
	finalwave_text->SetUV(1, 1, 1, 1); //UVを指定
	m_MySceneObjects.emplace_back(finalwave_text);
	finalwave_text->SetLive(false); // wave2は非表示

	// UI (操作説明)
	Texture2D* sousa_text = Game::GetInstance()->AddObject<Texture2D>();
	sousa_text->SetTexture("assets/texture/sousa_camera.png"); // 画像を指定
	sousa_text->SetPosition(300.0f, -300.0f, 0.0f); // 位置を設定
	sousa_text->SetScale(600.0f, 100.0f, 0.0f); // 大きさを指定
	sousa_text->SetUV(1, 1, 1, 1); //UVを指定
	m_MySceneObjects.emplace_back(sousa_text);

	// UI(プレイヤーHP)
	//Texture2D* pt4 = Game::GetInstance()->AddObject<Texture2D>();
	//pt4->SetTexture("assets/texture/number.png"); // 画像を指定
	//pt4->SetPosition(-485.0f, -300.0f, 0.0f); // 位置を設定
	//pt4->SetScale(65.0f, 45.0f, 0.0f); // 大きさを指定
	//pt4->SetUV(m_Par + 1, 1, 10, 1); //UVを指定
	//m_MySceneObjects.emplace_back(pt4);

	// UI(ボスHP 1桁目)
	//Texture2D* pt5 = Game::GetInstance()->AddObject<Texture2D>();
	//pt5->SetTexture("assets/texture/number.png"); // 画像を指定
	//pt5->SetPosition(565.0f, 300.0f, 0.0f); // 位置を設定
	//pt5->SetScale(95.0f, 72.0f, 0.0f); // 大きさを指定
	//pt5->SetUV(2, 1, 10, 1); //UVを指定
	//m_MySceneObjects.emplace_back(pt5);

	// UI(ボスHP 2桁目)
	//Texture2D* pt6 = Game::GetInstance()->AddObject<Texture2D>();
	//pt6->SetTexture("assets/texture/number.png"); // 画像を指定
	//pt6->SetPosition(485.0f, 300.0f, 0.0f); // 位置を設定
	//pt6->SetScale(95.0f, 72.0f, 0.0f); // 大きさを指定
	//pt6->SetUV(1, 1, 10, 1); //UVを指定
	//m_MySceneObjects.emplace_back(pt6);

	//vector<Texture2D*> gauge = player->GetGauge();
	//m_MySceneObjects.insert(
	//	m_MySceneObjects.end(),    // 挿入位置
	//	gauge.begin(),        // 挿入する範囲の開始
	//	gauge.end()           // 挿入する範囲の終了
	//);

	Sound::GetInstance()->Play(SOUND_BGM_MAIN);
	Sound::GetInstance()->SetVolume(SOUND_BGM_MAIN, 0.5f);
	Sound::GetInstance()->SetMasterVolume(0.3f);
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

	wave = 1;// 最初のWaveに設定
	m_waveState = WaveState::WaveEffect;
	framecount = 0;

	DebugUI::RedistDebugFunction([this]() {
		WallManager::DebugWallStatus();
		});
}

//更新
void Stage1Scene::Update()
{
	Game::GetInstance()->CollisionObject(m_MySceneObjects);
	EnemyManager::GetInstance().Update();

	if(boss->GetHP() <= 0){
		Game::GetInstance()->ChangeSceneFadeOut(RESULT);
		Sound::GetInstance()->Stop(SOUND_BGM_MAIN);
	}else if (player->GetHP() <= 0) {
		Game::GetInstance()->ChangeSceneFadeOut(GAMEOVER);
		Sound::GetInstance()->Stop(SOUND_BGM_MAIN);
	}

	WaveChange();

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
	EnemyManager::GetInstance().Uninit();
	m_MySceneObjects.clear();
}

// Wave変更処理
void Stage1Scene::WaveChange()
{
	switch (m_waveState)
	{
	case WaveState::None:
		UpdateWaveCheck();
		break;

	case WaveState::WaveEffect:
		UpdateWaveEffect();
		break;

	case WaveState::SpawnEnemy:
		UpdateSpawnEnemy();
		break;

	}
}

void Stage1Scene::UpdateWaveCheck()
{
	if (boss->GetLive()) {
		m_waveState = WaveState::BossBattle;
		return;
	}

	if (EnemyManager::GetInstance().GetLiveEnemy() > 0) {
		return;
	}

	++wave;
	if (wave == 2) {
		wave2_text->SetLive(true);
	}
	else if (wave == maxwave) {
		finalwave_text->SetLive(true);
		// ボス出現、行動はしない
		boss->SetNotUpdate(true);
		boss->ReInit();
		boss->SetGaugeLive(true);
		boss_hp_text->SetLive(true);
		boss_guard_text->SetLive(true);
	}
	m_waveState = WaveState::WaveEffect;
	framecount = 0;
}

void Stage1Scene::UpdateWaveEffect()
{
	//++framecount;
	if (++framecount >= 120){
		if (wave == 1) {
			wave1_text->SetLive(false);
		}
		else if (wave == 2) {
			wave2_text->SetLive(false);
		}else if(wave == maxwave){
			finalwave_text->SetLive(false);
		}
		framecount = 0;
		m_waveState = WaveState::SpawnEnemy;
	}
}

void Stage1Scene::UpdateSpawnEnemy()
{
	if (wave < maxwave) {
		EnemyManager::GetInstance().SetEnemy(5,{ (groundsize.x - 50) * 0.5f, 30, (groundsize.y - 50) * 0.5f });
	}
	else if (wave == maxwave) {
		//	ボス行動開始
		boss->SetNotUpdate(false); 
	}
	m_waveState = WaveState::None;
}

