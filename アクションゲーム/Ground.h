#pragma once
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Texture.h"
#include "Object.h"
#include "Material.h"

//-----------------------------------------------------------------------------
//Groundクラス
//-----------------------------------------------------------------------------
class Ground : public Object
{

	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer	 m_IndexBuffer; // インデックスバッファ
	VertexBuffer<VERTEX_3D>	m_VertexBuffer; // 頂点バッファ

	// 描画の為の情報（見た目に関わる部分）
	Texture m_Texture; // テクスチャ
	std::unique_ptr<Material> m_Materiale; //マテリアル

	int m_SizeX; //横サイズ
	int m_SizeZ; //縦サイズ
	std::vector<VERTEX_3D> m_Vertices; //頂点情報

public:

	Ground(Camera* cam); // コンストラクタ
	~Ground(); // デストラクタ

	void Init();
	void Draw();
	void Update();
	void Uninit();

	//頂点情報を取得
	std::vector<VERTEX_3D> GetVertices();
	DirectX::XMFLOAT2 GetGroundSize();//地面の大きさを返す
};
