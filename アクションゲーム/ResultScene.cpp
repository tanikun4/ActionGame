#include "ResultScene.h"
#include "Game.h"
#include "Texture2D.h"
#include "Fade.h"

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
	pt->SetTexture("assets/texture/background2.png"); // 画像を指定
	pt->SetScale(1280.0f, 720.0f, 0.0f); // 大きさを指定
	m_MySceneObjects.emplace_back(pt);
	Fade::GetInstance()->StartFadeIn();
}

// 更新
void ResultScene::Update()
{
	// スペースキーを押してタイトルへ
	if (Input::GetKeyTrigger(VK_SPACE))
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

// スコアを設定
void ResultScene::SetScore(int c)
{
	// リザルト文字列オブジェクト
	Texture2D* stringObj = dynamic_cast<Texture2D*>(m_MySceneObjects[1]);
	switch (c) {
	case -4:
		stringObj->SetUV(1, 2, 1, 13); // -4 コンドル
		break;
	case -3:
		stringObj->SetUV(1, 3, 1, 13); // -3 アルバトロス
		break;
	case -2:
		stringObj->SetUV(1, 4, 1, 13); // -2 イーグル
		break;
	case -1:
		stringObj->SetUV(1, 5, 1, 13); // -1 バーディ
		break;
	case 0:
		stringObj->SetUV(1, 6, 1, 13); // パー
		break;
	case 1:
		stringObj->SetUV(1, 7, 1, 13); // +1 ボギー
		break;
	case 2:
		stringObj->SetUV(1, 8, 1, 13); // +2 ダブルボギー
		break;
	case 3:
		stringObj->SetUV(1, 9, 1, 13); // +3 トリプルボギー
		break;
	case 4:
		stringObj->SetUV(1, 10, 1, 13); // +4
		break;
	case 5:
		stringObj->SetUV(1, 11, 1, 13); // +5
		break;
	case 6:
		stringObj->SetUV(1, 12, 1, 13); // +6
		break;
	case 7:
		stringObj->SetUV(1, 13, 1, 13); // +7以上
		break;
	}
}
