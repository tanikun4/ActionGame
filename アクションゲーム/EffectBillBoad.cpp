#include "EffectBillBoad.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
EffectBillBoad::EffectBillBoad(Camera* cam) :EffectBase(cam)
{
	// マテリアル情報作成
	m_Materiale = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = { 1,1,1,1 };
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true; // テクスチャを使うか否かのフラグ
	m_Materiale->Create(mtrl);
}

EffectBillBoad::EffectBillBoad() : EffectBase()
{
	// マテリアル情報作成
	m_Materiale = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = { 1,1,1,1 };
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true; // テクスチャを使うか否かのフラグ
	m_Materiale->Create(mtrl);
}

// デストラクタ
EffectBillBoad::~EffectBillBoad()
{

}

// 初期化処理(ダミー定義)
void EffectBillBoad::Init()
{

}

//初期化処理(ダミー定義)
void EffectBillBoad::Init(LoadedEffectData& data, int _maxlife, DirectX::SimpleMath::Vector3 ta_scale)
{

}

//初期化処理(多分これしか使わない)
void EffectBillBoad::Init(LoadedEffectData& data, SharedEffect2DData& shared_data, int _maxlife, DirectX::SimpleMath::Vector3 ta_scale)
{
	// 頂点バッファ取得
	m_VertexBuffer = shared_data.m_2DVertexBuffer.get();

	// インデックスバッファ取得
	m_IndexBuffer = shared_data.m_2DIndexBuffer.get();

	// シェーダオブジェクト取得
	m_Shader = *data.shader.get();

	// テクスチャ情報取得(単体)
	m_Texture = data.textures.back().get();

	// UV分割数取得
	m_SplitX = data.texture_uv.x;
	m_SplitY = data.texture_uv.y;

	BaseInit(_maxlife, ta_scale);
}


// 更新処理
void EffectBillBoad::Update()
{
	//スケールを変化量分増加
	m_Scale += scale_changerate;

	//エフェクトのライフタイムを加算
	++m_lifeflame;

	//生存フレーム限界に達したら、存在フラグをfalseにする
	if (m_lifeflame >= m_maxlife) {
		m_live = false;
		//テスト用
		m_NumU++;
		if (m_NumU > m_SplitX) {
			m_NumU = 1;
		}
	}
}

// 描画処理
void EffectBillBoad::Draw()
{
	// 深度書き込みを有効にする（Billboard でも ON にする）
	Renderer::SetDepthEnable(true);

	// BillBoard（カメラの方向を向く行列）作成
	Matrix invView = m_Camera->GetViewMatrix();
	invView.Translation(Vector3(0, 0, 0));  // 平行移動成分を消す

	// 回転成分だけ反転したものがビルボード行列
	Matrix billboard = invView;

	//自身の Y 回転を作成
	Matrix selfRotY = Matrix::CreateRotationY(m_Rotation.y);

	// SRT 行列作成
	Matrix s = Matrix::CreateScale(m_Scale);
	Matrix t = Matrix::CreateTranslation(m_Position);

	// ビルボードなので自身の回転はyのみ反映
	Matrix worldmtx = s * selfRotY * billboard * t;

	Renderer::SetWorldMatrix(&worldmtx);

	// 描画セットアップ
	ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();

	devicecontext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_Shader.SetGPU();
	m_VertexBuffer->SetGPU();
	m_IndexBuffer->SetGPU();

	m_Texture->SetGPU();
	m_Materiale->SetGPU();

	// UV 設定
	float u = m_NumU - 1;
	float v = m_NumV - 1;
	float uw = 1.0f / m_SplitX;
	float vh = 1.0f / m_SplitY;

	Renderer::SetUV(u, v, uw, vh);

	// カメラ設定
	m_Camera->SetCamera(0);

	// 描画
	devicecontext->DrawIndexed(4, 0, 0);
}


// 終了処理
void EffectBillBoad::Uninit()
{

}

// テクスチャを指定
void EffectBillBoad::SetTexture(const char* imgname)
{
	// テクスチャロード
	bool sts = m_Texture->Load(imgname);
	assert(sts == true);
}

// UV座標を指定
void EffectBillBoad::SetUV(const float& nu, const float& nv, const float& sx, const float& sy)
{
	m_NumU = nu;
	m_NumV = nv;
	m_SplitX = sx;
	m_SplitY = sy;
}

// 色を指定(現状は機能していない)
void EffectBillBoad::SetColor(const DirectX::SimpleMath::Vector4& color)
{
	m_Materiale->SetDiffuse(color);
}