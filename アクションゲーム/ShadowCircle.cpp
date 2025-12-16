
#include "ShadowCircle.h"
#include "Renderer.h"

using namespace DirectX::SimpleMath;

ShadowCircle::ShadowCircle(Camera* cam)
    : Texture2D(cam)
{
}

ShadowCircle::~ShadowCircle()
{
}

void ShadowCircle::Init()
{
    Texture2D::Init();

    // 丸影テクスチャ
    SetTexture("assets/texture/shadow_circle.png");

    // 初期設定
    SetColor(Vector4(0, 0, 0, 0.6f));
    SetRotation(0.0f, 90.0f, 0.0f); // 地面に寝かせる
}

void ShadowCircle::Update()
{
    // 必要ならここに処理を書く
}

void ShadowCircle::Draw()
{

    // 深度：テストON / 書き込みOFF
	//if (m_color.w <= 0.0f) { return; } //透明なら描画しない

	Renderer::SetDepthEnable(false); // 深度書き込みを無効にする
	Renderer::SetBlendState(BS_ALPHABLEND);

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

	m_Texture.SetGPU();
	m_Materiale->SetGPU();


	// UVの設定を指定
	float u = m_NumU - 1;
	float v = m_NumV - 1;
	float uw = 1 / m_SplitX;
	float vh = 1 / m_SplitY;

	Renderer::SetUV(u, v, uw, vh);

	devicecontext->DrawIndexed(
		4, // 描画するインデックス数（四角形なんで４）
		0, // 最初のインデックスバッファの位置
		0);

    // 3Dモードで描画
    m_Camera->SetCamera(0);

	Renderer::SetDepthEnable(true); // 深度書き込みを有効にする

}

void ShadowCircle::UpdateShadow(
    const Vector3& ownerPos,
    float groundY)
{
    // 位置
    Vector3 pos = ownerPos;
    pos.y = groundY + 0.02f;
    SetPosition(pos);

    // 高さ
    float h = ownerPos.y - groundY;
    float t = std::clamp(h / m_MaxHeight, 0.0f, 1.0f);

    // スケール（高いほど小さく）
    float scale = std::lerp(m_BaseScale, m_BaseScale * 0.4f, t);
    SetScale(scale, scale, 1.0f);

    // α（高いほど薄く）
    Vector4 col = Vector4(0, 0, 0, std::lerp(0.6f, 0.2f, t));
	
    SetColor(col);
}
