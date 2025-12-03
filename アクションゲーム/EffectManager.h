#pragma once
#include "EffectObject.h"
#include "StaticMesh.h"
enum {
	TEST_EFFECT = 0,

	EFFECT_MAX,
};

//エフェクト管理クラス、シングルトンパターンで、エフェクトの初回読込、生成処理をまとめて行う
class EffectManager
{
private:
	//エフェクトデータ配列
	std::vector<LoadedEffectData> m_LoadData;

	static std::unique_ptr<EffectManager> m_Instance; // ゲームインスタンス
	std::vector<EffectBase*> m_Effects; // エフェクトオブジェクト配列

	LoadedEffectData LoadEffect(
	const std::string textureName,
	const std::string modelName,
	const int texDivX = 1,
	const int texDivY = 1,
	const std::string VSshaderName = "shader/litTextureVS.hlsl",
	const std::string PSshaderName = "shader/litTexturePS.hlsl"); // エフェクトリソース読込関数

	Camera* m_Camera;//自身のカメラ、Gameからポインタ参照を取ってくる
public:
	EffectManager(); // コンストラクタ
	~EffectManager(); // デストラクタ

	static void Init(); // 初期化
	static void Update(); // 更新
	static void Draw(); // 描画
	static void Uninit(); // 終了処理

	static EffectManager* GetInstance();

	void Play(
		int _id,
		int _maxlife = 60,
		DirectX::SimpleMath::Vector3 _first_scale = {1,1,1}, 
		DirectX::SimpleMath::Vector3 _ta_scale = { 0,0,0 }); // エフェクト再生関数
};