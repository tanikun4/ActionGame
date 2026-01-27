#include "TitleScene.h"
#include "Game.h"
#include "EffectManager.h"
#include "WallManager.h"

#include "Player.h"
#include "Boss.h"
#include "Ground.h"
#include "Bullet.h"
#include "Projectile.h"
#include "Pole.h"
#include "GroundManager.h"
#include "ICollider.h"
#include "Fade.h"

#include "DebugUI.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
TitleScene::TitleScene()
{
	Init();
}

// デストラクタ
TitleScene::~TitleScene()
{
	Uninit();
}





// 初期化
void TitleScene::Init()
{
	Sound::GetInstance()->SetMasterVolume(0.0f); // 全体音量を0%に

	srand((unsigned)time(NULL));

	// オブジェクトを作成

	// 背景
	Texture2D* background = Game::GetInstance()->AddObject<Texture2D>();
	background->SetTexture("assets/texture/sky.png"); // 画像を指定
	background->SetPosition(-0.0f, 0.0f, 0.0f); // 位置を設定
	background->SetScale(1280.0f, 720.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(background);

	Ground* ground = Game::GetInstance()->AddObject<Ground>();
	m_MySceneObjects.emplace_back(ground);

	GroundManager::GetInstance().Init();

	Boss* boss = Game::GetInstance()->AddObject<Boss>();
	m_MySceneObjects.emplace_back(boss);
	boss->SetDEF(99);//無敵状態にする

	Player* player = Game::GetInstance()->AddObject<Player>();
	m_MySceneObjects.emplace_back(player);
	player->SetState(0);
	player->SetDemoMode(true);
	

	for (int i = 0; i < 3; i++) {
		m_MySceneObjects.emplace_back(Game::GetInstance()->AddObject<Bullet>()); //弾
		Bullet* bullet = dynamic_cast<Bullet*>(m_MySceneObjects.back()); //弾
		bullet->SetState(0); // //弾を非表示
	}

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
	WallManager::SetWall(ground->GetGroundSize(), m_MySceneObjects);

	// カメラ開始位置
	Vector3 initialOffset(0.0f, 80.0f, -180.0f);
	initialOffset.Normalize();

	// yaw, pitch を計算
	Vector2 CameraDirection;
	CameraDirection.x = 0;//PI - atan2(initialOffset.x, initialOffset.z); // yaw
	CameraDirection.y = -2.14f;//PI + asin(initialOffset.y);                   // pitch

	Game::GetInstance()->GetCamera().SetDirection(CameraDirection);//カメラ方向設定
	Game::GetInstance()->GetCamera().SetInputFg(false);//カメラ操作有効化
	Game::GetInstance()->GetCamera().SetPosition(Vector3(0,150,200));

	DebugUI::RedistDebugFunction([this]() {
		WallManager::DebugWallStatus();
		});

	//背景画像オブジェクトを作成
	Texture2D* pt = Game::GetInstance()->AddObject<Texture2D>();
	pt->SetTexture("assets/texture/title.png"); // 画像を指定
	pt->SetPosition(0.0f, 0.0f, 0.0f); // 位置を指定
	pt->SetRotation(0.0f, 0.0f, 0.0f); // 角度を指定
	pt->SetScale(1280.0f, 720.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(pt);

	//press_enterkey画像オブジェクトを作成
	press_enterkey = Game::GetInstance()->AddObject<Texture2D>();
	press_enterkey->SetTexture("assets/texture/2DEffect/press_enterkey.png"); // 画像を指定
	press_enterkey->SetPosition(0.0f, -300.0f, 0.0f); // 位置を指定
	press_enterkey->SetScale(640.0f, 128.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(press_enterkey);
	press_enterkey->SetLive(false);
}

// 更新
void TitleScene::Update()
{

	if (count > 120) {
		count = 0;
		press_enterkey->SetLive(true);
	}
	else {
		++count;
		if (count == 60) {
			press_enterkey->SetLive(false);

			//エフェクト版press enter key

			//EffectParams param;
			//param.scale = DirectX::SimpleMath::Vector3(114.0f, 64.0f, 0.0f);
			//param.maxLife = 60;
			//param.pos = DirectX::SimpleMath::Vector3(0.0f, -27.0f, 0.0f);
			//param.scale = DirectX::SimpleMath::Vector3(80.0f, 16.0f, 0.0f);
			//EffectManager::GetInstance()->Play(PRESS_ENTERKEY, param);
		}
	}

	Game::GetInstance()->CollisionObject(m_MySceneObjects);

	// スペースキーを押してステージ1へ
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		Game::GetInstance()->ChangeSceneFadeOut(STAGE1);
	}
}

// 終了処理
void TitleScene::Uninit()
{

	for (auto& o : m_MySceneObjects) {//ループ中にポインタを削除するとバグるので、終了処理のみを先に行う
		if (o) o->Uninit();
	}
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		if (o) Game::GetInstance()->DeleteObject(o);
		o = nullptr;
	}
	m_MySceneObjects.clear();
}

