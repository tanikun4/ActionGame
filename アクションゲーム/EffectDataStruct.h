#pragma once
#include <memory>
#include <vector>
#include "MathCommon.h"
#include "VertexBuffer.h"

//エフェクトで使うデータ構造体定義

// 前方宣言
class StaticMesh;
class Texture;
class Material;
class Shader;
class IndexBuffer;


//ロード済みのエフェクトデータを格納する構造体
struct LoadedEffectData {
	std::unique_ptr<StaticMesh> mesh;                   // メッシュ
	std::vector<std::unique_ptr<Texture>> textures;     // テクスチャ
	std::vector<std::unique_ptr<Material>> materials;   // マテリアル
	std::unique_ptr<Shader> shader;  // シェーダー
	Int2 texture_uv;// テクスチャのUV分割数

};

struct SharedEffect2DData {
	std::unique_ptr<VertexBuffer<VERTEX_3D>> m_2DVertexBuffer;
	std::unique_ptr<IndexBuffer> m_2DIndexBuffer;
};

// エフェクトパラメータ構造体、エフェクト再生時のパラメータをまとめて渡すために使用
//-FLT_MAXは変数が未代入であることを示す、この値のままの場合は変化させない
struct EffectParams
{
    DirectX::SimpleMath::Vector3 pos = {0,0,0};                                     // 開始位置
    DirectX::SimpleMath::Vector3 pos_amount = { 0,0,0 };                            // 座標変化量

    DirectX::SimpleMath::Vector3 rot = { 0,0,0 };                                   // 開始回転
    DirectX::SimpleMath::Vector3 rot_amount = { 0,0,0 };                            // 回転変化量(2Dで使う場合はzのみを使用)

    DirectX::SimpleMath::Vector3 scale = { 1,1,1 };                                 // 開始スケール
	DirectX::SimpleMath::Vector3 scale_amount = { 0,0,0 };                          // スケール変化量(2Dで使う場合はxyのみを使用)

	DirectX::SimpleMath::Vector4 color = { 1,1,1,1 }; // 色情報 

    int maxLife = 10;                             // 寿命
    int change_posFrame = 0;                     // 座標変化開始フレーム
    int change_rotFrame = 0;                     // 回転変化開始フレーム
    int change_scaleFrame = 0;                   // スケール変化開始フレーム
	bool endless = false;                                 // 無限再生フラグ
};
