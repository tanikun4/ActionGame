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

	static std::unique_ptr<EffectManager> m_Instance; // ゲームインスタンス
	std::vector<EffectBase*> m_Effects; // エフェクトオブジェクト配列

	void LoadEffect(std::string modelfilename, std::string texturefilename); // エフェクトリソース読込関数

	//ロード済みエフェクトデータ構造体
	struct LoadEffectData {
		std::shared_ptr<StaticMesh> mesh;                    // メッシュ
		std::vector<std::shared_ptr<Texture>> textures;      // テクスチャ
		std::vector<std::shared_ptr<Material>> materials;    // マテリアル
		std::unique_ptr<Shader> shader;						 // シェーダー
	};
	//エフェクトデータ配列
	std::vector<LoadEffectData> m_LoadData;

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