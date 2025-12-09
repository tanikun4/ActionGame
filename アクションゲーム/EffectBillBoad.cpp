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
	m_live = false;
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
	m_live = false;
}

// デストラクタ
EffectBillBoad::~EffectBillBoad()
{

}

//初期化処理
void EffectBillBoad::Init(LoadedEffectData& data, SharedEffect2DData& shared_data, int _maxlife,
	DirectX::SimpleMath::Vector3 _pos_amount, int _pos_changeframe,
	DirectX::SimpleMath::Vector3 _rot_amount, int _rot_changeframe,
	DirectX::SimpleMath::Vector3 _scale_amount, int _scale_changeframe)
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

	m_aminframe =  _maxlife / (int)(m_SplitX * m_SplitY); //アニメーション遷移フレーム数設定

	BaseInit(_maxlife, _pos_amount, _pos_changeframe, _rot_amount, _rot_changeframe, _scale_amount, _scale_changeframe);

	// ビルボード用に座標変化量と回転変化量、スケール変化量を調整
	// 3D移動ベクトルに変換
	Vector3 new_pos_changerate =
		pos_changerate.x * m_Camera->GetRightVector() +
		pos_changerate.y * m_Camera->GetUpVector() +
		pos_changerate.z * m_Camera->GetForwardVector();

	pos_changerate = new_pos_changerate;

	rot_changerate.x = 0;
	rot_changerate.y = 0;
	scale_changerate.z = 0;
}


// 更新処理
void EffectBillBoad::Update()
{
	BaseUpdate();

	if(m_lifeframe % m_aminframe == 0) {//アニメーション用フレームカウントが最大値に達したら
		++m_NumU;
		if (m_NumU > m_SplitX) {//U座標が最大値を超えたら
			m_NumU = 1;
			++m_NumV;
			if (m_NumV > m_SplitY) {//V座標が最大値を超えたら
				m_NumV = 1;
			}
		}
	}
}

// 描画処理
void EffectBillBoad::Draw()
{
	// カメラ設定
	m_Camera->SetCamera(0);

	// 深度書き込みを有効にする（Billboard でも ON にする）
	Renderer::SetDepthEnable(true);

	// ビュー行列取得
	Matrix view = m_Camera->GetViewMatrix();

	// ビュー行列から回転成分を取り出す（平行移動は消す）
	view.Translation(Vector3(0, 0, 0));

	// 回転成分の逆回転を作るために転置する
	Matrix billboard = view.Transpose();

	//自身の Z 回転を作成
	Matrix selfRotZ = Matrix::CreateRotationZ(m_Rotation.z);

	// SRT 行列作成
	Matrix s = Matrix::CreateScale(m_Scale);
	Matrix t = Matrix::CreateTranslation(m_Position);

	// ビルボードなので自身の回転はyのみ反映
	Matrix worldmtx = s * selfRotZ * billboard * t;

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