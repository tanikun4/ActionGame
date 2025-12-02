#include "EffectManager.h"
#include "Game.h"
using namespace std;

unique_ptr<EffectManager> EffectManager::m_Instance;

// コンストラクタ
EffectManager::EffectManager()
{

}

// デストラクタ
EffectManager::~EffectManager()
{
	//m_Instance = nullptr;
}

// 初期化
void EffectManager::Init()
{
	m_Instance = make_unique<EffectManager>();
	m_Instance->m_Effects.emplace_back(Game::GetInstance()->AddObject<EffectObject>());
	for(auto& e : m_Instance->m_Effects)
	{
		e->Init();
	}
}

// 更新
void EffectManager::Update()
{

}

// 描画
void EffectManager::Draw()
{

}

// 終了処理
void EffectManager::Uninit()
{

}

// インスタンスを取得
EffectManager* EffectManager::GetInstance()
{
	return m_Instance.get();
}
