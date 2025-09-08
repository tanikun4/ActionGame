#include "Game.h"
#include "Renderer.h"
#include "sound.h"
#include "ICollider.h"
#include "Object.h"

Game* Game::m_Instance;

// コンストラクタ
Game::Game()
{
	m_Input = std::make_unique<Input>(); //入力処理を作成
	m_Camera = std::make_unique<Camera>(); //カメラを作成
	m_WireRenderer = std::make_unique<WireRenderer>(); //ワイヤーレンダラーを作成
}

// デストラクタ
Game::~Game()
{
	delete m_Scene;
}

// 初期化
void Game::Init()
{
	// オブジェクト作成
	m_Instance = new Game;

	Sound::SoundInit();
	Sound::GetInstance()->Init();//サウンド初期化

	// 描画初期化
	Renderer::Init();

	// オブジェクト配列作成
	//m_Instance->m_Objects.emplace_back(new GolfBall);
	//m_Instance->m_Objects.emplace_back(new Ground);

	// カメラ初期化
	m_Instance->m_Camera->Init();

	m_Instance->m_WireRenderer->Init();

	m_Instance->m_Scene = new TitleScene; //メモリを確保

	// オブジェクト初期化
	for (auto& o : m_Instance->m_Objects)
	{
		o->Init();
	}
}

// 更新
void Game::Update()
{
	// シーン更新
	m_Instance->m_Scene->Update();

	// カメラ更新
	m_Instance->m_Camera->Update();

	// 入力処理更新
	m_Instance->m_Input->Update();

	// オブジェクト更新
	for (auto& o : m_Instance->m_Objects)
	{
		o->Update();
	}
}

// 描画
void Game::Draw()
{
	// 描画前処理
	Renderer::Begin();

	// カメラ描画
	m_Instance->m_Camera->Draw();

	// オブジェクト描画
	for (auto& o : m_Instance->m_Objects)
	{
		o->Draw();
		if (Input::GetKeyPress(VK_X)) {
			auto col = dynamic_cast<ICollider*>(o.get());
			if (col) {
				m_Instance->m_WireRenderer->Draw(col->GetCollision());
			}
		}
	}

	// 描画後処理
	Renderer::End();
}

// 終了処理
void Game::Uninit()
{

	//オブジェクトを全て削除
	m_Instance->DeleteAllObject();


	// カメラ終了処理
	m_Instance->m_Camera->Uninit();

	// 描画終了処理
	Renderer::Uninit();
	Sound::GetInstance()->Uninit();
}

// インスタンスを取得
Game* Game::GetInstance()
{
	return m_Instance;
}

// カメラを取得する
Camera& Game::GetCamera()
{
	return *(m_Instance->m_Camera);
}

// シーンを切り替える
void Game::ChangeScene(SceneName sName)
{
	// 読み込み済みのシーンがあれば削除
	int score = 0;
	if (m_Instance->m_Scene != nullptr) 
	{
		// 消そうとしているシーンがStage1ならスコアを保存しておく
		if (Stage1Scene* sObj = dynamic_cast<Stage1Scene*>(m_Instance->m_Scene)) 
		{
			score = sObj->GetScore();
		}
		delete m_Instance->m_Scene;
		m_Instance->m_Scene = nullptr;
	}

	switch (sName)
	{
	case TITLE:
		m_Instance->m_Scene = new TitleScene; // メモリを確保
		break;
	case STAGE1:
		m_Instance->m_Scene = new Stage1Scene; // メモリを確保
		break;
	case RESULT:
		m_Instance->m_Scene = new ResultScene; // メモリを確保
		break;
	case GAMEOVER:
		m_Instance->m_Scene = new GameOverScene; // メモリを確保
		break;
	}
}

// オブジェクトを削除する
void Game::DeleteObject(Object* pt)
{
	if (pt == nullptr) return;

	// 要素を削除
	m_Instance->m_Objects.erase(
		std::remove_if(
			m_Instance->m_Objects.begin(),
			m_Instance->m_Objects.end(),
			[pt](const std::unique_ptr<Object>& element) {return element.get() == pt; }),
		m_Instance->m_Objects.end());

	m_Instance->m_Objects.shrink_to_fit();

	pt = nullptr;

}

// オブジェクトをすべて削除する
void Game::DeleteAllObject()
{
	// オブジェクト終了処理
	for (auto& o : m_Instance->m_Objects)
	{
		o->Uninit();
	}

	m_Instance->m_Objects.clear(); //全て削除
	m_Instance->m_Objects.shrink_to_fit();
}

