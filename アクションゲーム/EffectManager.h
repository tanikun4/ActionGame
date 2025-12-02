#pragma once
#include "EffectObject.h"


//エフェクト管理クラス、シングルトンパターンで、エフェクトの初回読込、生成処理をまとめて行う
class EffectManager
{
private:

	static std::unique_ptr<EffectManager> m_Instance; // ゲームインスタンス
	std::vector<EffectBase*> m_Effects; // エフェクトオブジェクト配列

public:

	EffectManager(); // コンストラクタ
	~EffectManager(); // デストラクタ

	static void Init(); // 初期化
	static void Update(); // 更新
	static void Draw(); // 描画
	static void Uninit(); // 終了処理

	static EffectManager* GetInstance();

	void Play(int _id); // エフェクト再生関数
};