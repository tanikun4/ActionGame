#include "EffectObject.h"
#include "StaticMesh.h"
#include <iostream>

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
EffectObject::EffectObject(Camera* cam) : EffectBase(cam)
{
	m_live = false;
}

EffectObject::EffectObject()
{
	m_live = false;
}

// デストラクタ
EffectObject::~EffectObject()
{

}

void EffectObject::Init(LoadedEffectData& data, int _maxlife,
	DirectX::SimpleMath::Vector3 _pos_amount, int _pos_changeframe,
	DirectX::SimpleMath::Vector3 _rot_amount, int _rot_changeframe,
	DirectX::SimpleMath::Vector3 _scale_amount, int _scale_changeframe)//初期化処理 
{
	// メッシュ読み込み
	StaticMesh* staticmesh = data.mesh.get();

	m_MeshRenderer.Init(*staticmesh);

	// シェーダオブジェクト取得
	m_Shader = *data.shader.get();

	// サブセット情報取得
	m_subsets = staticmesh->GetSubsets();

	// テクスチャ情報取得
	m_Textures.resize(data.textures.size());
	for (size_t i = 0; i < data.textures.size(); i++) {
		m_Textures[i] = data.textures[i].get();
	}

	// マテリアル情報取得
	m_Materiales.resize(data.materials.size());
	for (size_t i = 0; i < data.materials.size(); i++) {
		m_Materiales[i] = *data.materials[i].get();
	}

	BaseInit(_maxlife, _pos_amount, _pos_changeframe, _rot_amount, _rot_changeframe, _scale_amount, _scale_changeframe);
}

void EffectObject::Update() 
{
	BaseUpdate();
	
}

void EffectObject::Draw()
{
	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	m_Shader.SetGPU();

	// インデックスバッファ・頂点バッファをセット
	m_MeshRenderer.BeforeDraw();

	// カメラの設定を指定
	m_Camera->SetCamera(0);

	//マテリアル数分ループ 
	for (int i = 0; i < m_subsets.size(); i++)
	{
		// マテリアルをセット(サブセット情報の中にあるマテリアルインデックスを使用)
		m_Materiales[m_subsets[i].MaterialIdx].SetGPU();

		if (m_Materiales[m_subsets[i].MaterialIdx].isTextureEnable())
		{
			m_Textures[m_subsets[i].MaterialIdx]->SetGPU();
		}

		m_MeshRenderer.DrawSubset(
			m_subsets[i].IndexNum,		// 描画するインデックス数
			m_subsets[i].IndexBase,		// 最初のインデックスバッファの位置	
			m_subsets[i].VertexBase);	// 頂点バッファの最初から使用
	}
}


void EffectObject::Uninit()
{

}