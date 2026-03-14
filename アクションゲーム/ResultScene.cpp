#include "ResultScene.h"
#include "Game.h"
#include "Texture2D.h"
#include "Fade.h"
#include "ActionInput.h"

// コンストラクタ
ResultScene::ResultScene()
{
	Init();
}

// デストラクタ
ResultScene::~ResultScene()
{
	Uninit();
}

// 初期化
void ResultScene::Init()
{
	// 背景画像オブジェクトを作成
	Texture2D* pt = Game::GetInstance()->AddObject<Texture2D>();
	pt->SetTexture("assets/texture/result.png"); // 画像を指定
	pt->SetScale(1280.0f, 720.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(pt);
	Fade::GetInstance()->StartFadeIn();
}

// 更新
void ResultScene::Update()
{
	// エンターキーを押してタイトルへ
	if (ActionInput::GetInstance().IsTrigger(Action::Enter))
	{
		Game::GetInstance()->ChangeScene(TITLE);
	}
}

// 終了処理
void ResultScene::Uninit()
{
	// このシーンのオブジェクトを削除する
	for (auto& o : m_MySceneObjects) {
		Game::GetInstance()->DeleteObject(o);
	}
}
