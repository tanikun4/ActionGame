#include "TitleScene.h"
#include "Game.h"
#include "Texture2D.h"
#include "EffectManager.h"

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
	// 背景画像オブジェクトを作成
	//Texture2D* pt = Game::GetInstance()->AddObject<Texture2D>();
	//pt->SetTexture("assets/texture/title.png"); // 画像を指定
	//pt->SetPosition(0.0f, 0.0f, 0.0f); // 位置を指定
	//pt->SetRotation(0.0f, 0.0f, 0.0f); // 角度を指定
	//pt->SetScale(1280.0f, 720.0f, 0.0f); // 大きさを指定
	//m_MySceneObjects.emplace_back(pt);
	EffectParams param;
	param.scale = DirectX::SimpleMath::Vector3(110.0f, 64.0f, 0.0f);
	param.maxLife = 150;
	param.endless = true;
	EffectManager::GetInstance()->Play(TITLE_UGOKU,param);
}

// 更新
void TitleScene::Update()
{
	// スペースキーを押してステージ1へ
	if (Input::GetKeyTrigger(VK_RETURN))
	{
		Game::GetInstance()->ChangeSceneFadeOut(STAGE1);
	}
}

// 終了処理
void TitleScene::Uninit()
{
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance()->DeleteObject(o);
	}
}
