#pragma once
#include <memory>
#include <vector>

// 前方宣言
class StaticMesh;
class Texture;
class Material;
class Shader;

//ロード済みのエフェクトデータを格納する構造体
struct LoadedEffectData {
	std::unique_ptr<StaticMesh> mesh;                   // メッシュ
	std::vector<std::unique_ptr<Texture>> textures;     // テクスチャ
	std::vector<std::unique_ptr<Material>> materials;   // マテリアル
	std::unique_ptr<Shader> shader;                     // シェーダー
};
//エフェクトに渡すデータの構造体定義(こちらは没)
//struct EffectData
//{
//	std::string model = ""; // モデル名
//	std::string texture = ""; // テクスチャ名
//	int maxlife = 60; // 最大生存フレーム
//	DirectX::SimpleMath::Vector3 _ta_scale = { -1, -1, -1 };//最終スケール
//};