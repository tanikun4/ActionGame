#pragma once
#include "EffectObject.h"
#include "EffectBillBoad.h"

constexpr int EFFECT_POOLSIZE_3D = 5;
constexpr int EFFECT_POOLSIZE_2D = 5;

enum {
	TEST_EFFECT3D = 0,
	TEST_EFFECT2D,

	EFFECT_MAX,
};



//エフェクト管理クラス、シングルトンパターンで、エフェクトの初回読込、生成処理をまとめて行う
class EffectManager
{
private:
	
	//エフェクトデータ配列
	std::vector<LoadedEffectData> m_LoadData;

	static std::unique_ptr<EffectManager> m_Instance; // ゲームインスタンス
	std::vector<EffectObject*> m_Effects3D; // 3Dエフェクトオブジェクト配列
	std::vector<EffectBillBoad*> m_Effects2D; // 2Dエフェクトオブジェクト配列

	SharedEffect2DData m_Shared2D_Data;//2Dエフェクト共通データ

	Camera* m_Camera;//自身のカメラ、Gameからポインタ参照を取ってくる

	LoadedEffectData LoadEffect( // エフェクトリソース読込関数
		const std::string textureName,
		const std::string modelName,
		const int texDivX,
		const int texDivY,
		std::string VSshaderName,
		std::string PSshaderName);

	SharedEffect2DData Init2D();// 2Dエフェクト用共通変数の初期化処理
public:
	EffectManager(); // コンストラクタ
	~EffectManager(); // デストラクタ

	static void Init(); // 初期化
	static void Update(); // 更新
	static void Draw(); // 描画
	static void Uninit(); // 終了処理
	static void EffectsReset(); //エフェクトをリセット

	static EffectManager* GetInstance();

	// エフェクト再生関数
	static void Play(
		int _id,// エフェクトID
		int _maxlife = 60,// エフェクト最大生存フレーム
		DirectX::SimpleMath::Vector3 _pos = { 0,0,0 }, // エフェクト再生位置
		DirectX::SimpleMath::Vector3 _rot = { 0,0,0 }, // エフェクト再生角度
		DirectX::SimpleMath::Vector3 _first_scale = { 1,1,1 },  // エフェクト初期スケール
		DirectX::SimpleMath::Vector3 _ta_scale = { 0,0,0 }); // エフェクト最終スケール
};