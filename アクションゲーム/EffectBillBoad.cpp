#include "EffectBillBoad.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
EffectBillBoad::EffectBillBoad(Camera* cam) :EffectBase(cam)
{

}

EffectBillBoad::EffectBillBoad() : EffectBase()
{

}

// デストラクタ
EffectBillBoad::~EffectBillBoad()
{

}

//=======================================
// 初期化処理
//=======================================
void EffectBillBoad::Init()
{
	// 頂点データ
	std::vector<VERTEX_3D> vertices;

	vertices.resize(4);

	vertices[0].position = Vector3(-0.5f, 0.5f, 0);
	vertices[1].position = Vector3(0.5f, 0.5f, 0);
	vertices[2].position = Vector3(-0.5f, -0.5f, 0);
	vertices[3].position = Vector3(0.5f, -0.5f, 0);

	vertices[0].color = Color(1, 1, 1, 1);
	vertices[1].color = Color(1, 1, 1, 1);
	vertices[2].color = Color(1, 1, 1, 1);
	vertices[3].color = Color(1, 1, 1, 1);

	vertices[0].uv = Vector2(0, 0);
	vertices[1].uv = Vector2(1, 0);
	vertices[2].uv = Vector2(0, 1);
	vertices[3].uv = Vector2(1, 1);

	m_Vertices = vertices; // 保存

	// 頂点バッファ生成
	m_VertexBuffer.Create(vertices);

	// インデックスバッファ生成
	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	// インデックスバッファ生成
	m_IndexBuffer.Create(indices);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");

	// マテリアル情報取得
	m_Materiale = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = m_color;
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true; // テクスチャを使うか否かのフラグ
	m_Materiale->Create(mtrl);
}

//初期化処理
void EffectBillBoad::Init(LoadedEffectData& data, int _maxlife, DirectX::SimpleMath::Vector3 ta_scale = { -1,-1,-1 })
{
	// 頂点データ
	std::vector<VERTEX_3D> vertices;

	vertices.resize(4);

	vertices[0].position = Vector3(-0.5f, 0.5f, 0);
	vertices[1].position = Vector3(0.5f, 0.5f, 0);
	vertices[2].position = Vector3(-0.5f, -0.5f, 0);
	vertices[3].position = Vector3(0.5f, -0.5f, 0);

	vertices[0].color = Color(1, 1, 1, 1);
	vertices[1].color = Color(1, 1, 1, 1);
	vertices[2].color = Color(1, 1, 1, 1);
	vertices[3].color = Color(1, 1, 1, 1);

	vertices[0].uv = Vector2(0, 0);
	vertices[1].uv = Vector2(1, 0);
	vertices[2].uv = Vector2(0, 1);
	vertices[3].uv = Vector2(1, 1);

	m_Vertices = vertices; // 保存

	// 頂点バッファ生成
	m_VertexBuffer.Create(vertices);

	// インデックスバッファ生成
	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	// インデックスバッファ生成
	m_IndexBuffer.Create(indices);

	// シェーダオブジェクト取得
	m_Shader = *data.shader.get();

	// マテリアル情報取得
	m_Materiale = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = m_color;
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true; // テクスチャを使うか否かのフラグ
	m_Materiale->Create(mtrl);

	BaseInit(_maxlife, ta_scale);
}

//=======================================
// 更新処理
//=======================================
void EffectBillBoad::Update()
{
	//スケールを変化量分増加
	m_Scale += scale_changerate;

	//エフェクトのライフタイムを加算
	++m_lifeflame;

	//生存フレーム限界に達したら、存在フラグをfalseにする
	if (m_lifeflame >= m_maxlife) {
		m_live = false;
	}
}

//=======================================
// 描画処理
//=======================================
void EffectBillBoad::Draw()
{
	if (m_color.w <= 0.0f) { return; } //透明なら描画しない

	Renderer::SetDepthEnable(false); // 深度書き込みを無効にする

	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.x, m_Rotation.y, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_Shader.SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();

	m_Texture->SetGPU();
	m_Materiale->SetGPU();


	// UVの設定を指定
	float u = m_NumU - 1;
	float v = m_NumV - 1;
	float uw = 1 / m_SplitX;
	float vh = 1 / m_SplitY;

	Renderer::SetUV(u, v, uw, vh);

	// カメラの設定を指定
	m_Camera->SetCamera(1);

	devicecontext->DrawIndexed(
		4, // 描画するインデックス数（四角形なんで４）
		0, // 最初のインデックスバッファの位置
		0);

	Renderer::SetDepthEnable(true); // 深度書き込みを有効にする
}

//=======================================
// 終了処理
//=======================================
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

// 色を指定
void EffectBillBoad::SetColor(const DirectX::SimpleMath::Vector4& color)
{
	m_color = color;
	for (auto& v : m_Vertices)
		v.color = m_color;
	m_VertexBuffer.Modify(m_Vertices);
}