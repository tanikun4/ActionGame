#include "Game.h"
#include "Renderer.h"
#include "sound.h"
#include "Object.h"
#include "Fade.h"
#include "DebugUI.h"
#include "ActionInput.h"

#include "CollisionHelper.h"

#include "EffectManager.h"


Game* Game::m_Instance;

// コンストラクタ
Game::Game()
{
	//m_Input = std::make_unique<Input>(); //入力処理を作成
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

	// デバッグUIの初期化
	DebugUI::Init(Renderer::GetDevice(), Renderer::GetDeviceContext());

	// カメラ初期化
	m_Instance->m_Camera->Init();

	// ワイヤーフレーム表示クラス初期化
	m_Instance->m_WireRenderer->Init();

	//エフェクトマネージャ初期化
	EffectManager::Init();

	//入力初期化
	ActionInput::GetInstance().Init();


	// オブジェクト初期化
	for (auto& o : m_Instance->m_Objects)
	{
		o->Init();
	}

	m_Instance->m_Scene = new TitleScene; //メモリを確保
	
	Fade::GetInstance()->Init(m_Instance->m_Camera.get());
}

// 更新
void Game::Update()
{
	// シーン更新
	m_Instance->m_Scene->Update();

	// 入力処理更新
	//m_Instance->m_Input->Update();
	ActionInput::GetInstance().Update();

	// フェード更新
	Fade::GetInstance()->Update();


	// 更新可否判定（最初に1回）
	bool updateObjects = true;
	bool updateEffects = true;

	// ストップ処理
	if (m_Instance->stop)
	{
		updateObjects = false;
		updateEffects = false;

		++m_Instance->stop_frame;
		if (m_Instance->stop_frame >= m_Instance->max_stop_frame)
		{
			m_Instance->stop = false;
			m_Instance->stop_frame = 0;
		}
	}
	else if (m_Instance->slow)//スローモーション処理
	{
		//偶数フレームでのみ更新
		updateObjects = (m_Instance->slow_frame & 1) == 0;
		updateEffects = updateObjects;

		++m_Instance->slow_frame;
		if (m_Instance->slow_frame >= m_Instance->max_slow_frame)
		{
			m_Instance->slow = false;
			m_Instance->slow_frame = 0;
		}
	}

	// エフェクト更新処理
	if (updateEffects) EffectManager::Update();

	// オブジェクト更新
	if (updateObjects)
	{
		for (auto& o : m_Instance->m_Objects)
		{
			o->Update();
		}
	}

	if (ActionInput::GetInstance().IsTrigger(Action::Debug)) { // デバッグモード切り替え
		m_Instance->debugmode = !m_Instance->debugmode;
	}

	if(m_Instance->change_request && Fade::GetInstance()->FinishedFadeOut()) {// フェードアウト完了後にシーンを変更する
		m_Instance->change_request = false;
		m_Instance->ChangeScene(m_Instance->m_NextScene);
	}

	// カメラ更新、何があっても一番最後にする。
	m_Instance->m_Camera->Update();

}

// 描画
void Game::Draw()
{
	// 描画前処理
	Renderer::Begin();

	// カメラ描画
	//m_Instance->m_Camera->Draw();

	// オブジェクト描画
	for (auto& o : m_Instance->m_Objects)
	{

		if (!o->GetLive()) continue;// 生存フラグがオフなら非表示
		// 描画レイヤーがWorld3Dのオブジェクトのみ描画
		if (o->GetLayer() == DrawLayer::World3D)
			o->Draw();
		
		if (m_Instance->debugmode) {
			auto col = dynamic_cast<ICollider*>(o.get());
			if (col) {
				m_Instance->m_WireRenderer->Draw(col->GetCollision());
			}
		}
	}

	//エフェクトマネージャ描画	
	EffectManager::Draw();

	Renderer::PostProcess(false,false,false);

	// オブジェクト描画
	for (auto& o : m_Instance->m_Objects)
	{
		if (!o->GetLive()) continue;// 生存フラグがオフなら非表示
		// 描画レイヤーがUIのオブジェクトのみ描画
		if (o->GetLayer() == DrawLayer::UI) {
			o->Draw();
		}
	}

	// フェード描画
	Fade::GetInstance()->Draw();

	// デバッグUIの描画
	if(m_Instance->debugmode) DebugUI::Render();

	// 描画後処理
	Renderer::End();
}

// 終了処理
void Game::Uninit()
{

	// デバッグUIの終了処理
	DebugUI::DisposeUI();

	Fade::GetInstance()->Uninit();

	//エフェクトマネージャ終了処理
	EffectManager::Uninit();

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
	if (m_Instance->m_Scene != nullptr) 
	{
		delete m_Instance->m_Scene;
		m_Instance->m_Scene = nullptr;

		EffectManager::EffectsReset();// エフェクトをリセット

		DebugUI::Clear();
	}
	m_Camera->ResetTarget();
	m_Camera->Init();

	//EffectManager::GetInstance()->SetCamera(&GetCamera());

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

void Game::HitStop(int _maxstop)
{
	stop = true;
	stop_frame = 0;
	max_stop_frame = _maxstop;
	m_Camera->StartVibration(4.0f, PI * 0.5f, 3);//	カメラを揺らす
}

void Game::SlowMotion(int _maxslow)
{
	slow = true;
	slow_frame = 0;
	max_slow_frame = _maxslow;
}

void Game::ChangeSceneFadeOut(SceneName sName)// フェードアウト完了後にシーンを変更するための準備をする
{
	if (!Fade::GetInstance()->FinishedFadeIn()) return;// フェードイン中は無効
	m_NextScene = sName;
	change_request = true;
	Fade::GetInstance()->StartFadeOut();

}

// 渡されたオブジェクト全ての衝突判定を行う
void Game::CollisionObject(std::vector<Object*>& Object)
{
	for (size_t i = 0; i < Object.size(); ++i) {
		auto a = Object[i];
		if (!a->GetLive()) { continue; };
		auto col_a = a->GetCollider();
		if (!col_a) { continue; }

		for (size_t j = i + 1; j < Object.size(); ++j) {
			auto b = Object[j];
			if (!b->GetLive()) { continue; };
			auto col_b = b->GetCollider();
			if (!col_b) { continue; }

			if (Collision::CheckHit(*col_a, *col_b)) {
				a->HitObject(b);
				b->HitObject(a);
			}

		}
	}

}