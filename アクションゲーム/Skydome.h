#pragma once

#include "StaticMesh.h"
#include "Texture.h"
#include "Material.h"
#include "MeshRenderer.h"
#include "Object.h"

class Skydome : public Object
{

    // 描画の為の情報（メッシュに関わる情報）
    MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

    // 描画の為の情報（メッシュに関わる情報）
    IndexBuffer	 m_IndexBuffer; // インデックスバッファ
    VertexBuffer<VERTEX_3D>	m_VertexBuffer; // 頂点バッファ

    // 描画の為の情報（見た目に関わる部分）
    std::vector<std::unique_ptr<Material>> m_Materiales;
    std::vector<SUBSET> m_subsets;
    std::vector<std::unique_ptr<Texture>> m_Textures; // テクスチャ
	

public:
    Skydome(Camera* cam);//コンストラクタ
    Skydome();
    ~Skydome();//デストラクタ

	void Init();
	void Update();
	void Draw();
    void Uninit();
};