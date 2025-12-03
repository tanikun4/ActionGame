#include "EffectObject.h"
#include "StaticMesh.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
EffectObject::EffectObject(Camera* cam) : EffectBase(cam)
{

}

EffectObject::EffectObject()
{

}

// デストラクタ
EffectObject::~EffectObject()
{

}

void EffectObject::Init(LoadedEffectData& data, int _maxlife, Vector3 ta_scale) {
	// メッシュ読み込み
	StaticMesh* staticmesh = data.mesh.get();

	m_MeshRenderer.Init(*staticmesh);

	// シェーダオブジェクト生成
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
		m_Materiales[i] = data.materials[i].get();
	}

	BaseInit(_maxlife,ta_scale);
}

void EffectObject::Update() {
	if (!m_live) return;//生存していなければ処理をスキップ

	//スケールを変化量分増加
	m_Scale += scale_changerate;

	//エフェクトのライフタイムを加算
	++m_lifeflame;

	//生存フレーム限界に達したら、存在フラグをfalseにする
	if(m_lifeflame >= m_maxlife) {
		m_live = false;
	}
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

	// カメラの設定を指定し、ビュー行列、プロジェクション行列を作成。
	m_Camera->SetCamera(0);

	//マテリアル数分ループ 
	for (int i = 0; i < m_subsets.size(); i++)
	{
		// マテリアルをセット(サブセット情報の中にあるマテリアルインデックスを使用)
		m_Materiales[m_subsets[i].MaterialIdx]->SetGPU();

		if (m_Materiales[m_subsets[i].MaterialIdx]->isTextureEnable())
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