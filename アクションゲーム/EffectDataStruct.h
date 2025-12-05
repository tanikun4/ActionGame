#pragma once
#include <memory>
#include <vector>
#include <SimpleMath.h>
#include "VertexBuffer.h"

//エフェクトで使うデータ構造体定義

// 前方宣言
class StaticMesh;
class Texture;
class Material;
class Shader;
class IndexBuffer;


struct Int2 {
	int x;
	int y;
};

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