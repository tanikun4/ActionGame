#include "EffectTrail.h"

using namespace std;
using namespace DirectX::SimpleMath;

EffectTrail::EffectTrail(Camera* cam)
    : Object(cam)
{
}
EffectTrail::~EffectTrail()
{
}
void EffectTrail::Init()
{
    m_Shader.Create(
        "shader/unlitTextureVS.hlsl",
        "shader/EffectTexturePS.hlsl"
    );

    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl{};
    mtrl.Diffuse = Color(1, 1, 1, 1);
    mtrl.TextureEnable = true;
    m_Materiale->Create(mtrl);
}

void EffectTrail::Start()
{
    m_Points.clear();
    m_Active = true;
}

void EffectTrail::End()
{
    m_Active = false;
}

// 更新処理
void EffectTrail::Update()
{
	bool del = false; // 削除フラグ

    // 寿命更新
    for (auto& pt : m_Points)
        pt.life--;

    // 先頭から寿命切れを削除
    while (!m_Points.empty() && m_Points.front().life <= 0) {
        m_Points.erase(m_Points.begin());
        del = true;
    }

    if(del)
		m_Change = true;

    // メッシュ更新（ポイントがある時のみ）
    if (m_Change) {
        BuildMesh();
		m_Change = false;
    }
}

void EffectTrail::AddPoint(const Vector3& base, const Vector3& tip)
{

    if (!m_Active) return;

    // 新しいポイント追加
    TrailPoint p;
    p.base = base;
    p.tip = tip;
    p.life = m_LifeTime;

    m_Points.push_back(p);
	m_Change = true;

}

void EffectTrail::BuildMesh()
{
    m_Vertices.clear();
    m_Indices.clear();

    if (m_Points.size() < 2)  return;

    int index = 0;

    for (size_t i = 0; i < m_Points.size(); i++)
    {
        float alpha = m_Points[i].life / m_LifeTime;

        VERTEX_3D v0, v1;

        v0.position = m_Points[i].base;
        v1.position = m_Points[i].tip;

        v0.color = Color(1, 1, 1, alpha);
        v1.color = Color(1, 1, 1, alpha);

        v0.uv = Vector2(0, (float)i / m_Points.size());
        v1.uv = Vector2(1, (float)i / m_Points.size());

        m_Vertices.push_back(v0);
        m_Vertices.push_back(v1);

        if (i > 0)
        {
            m_Indices.push_back(index - 2);
            m_Indices.push_back(index - 1);
            m_Indices.push_back(index);

            m_Indices.push_back(index);
            m_Indices.push_back(index - 1);
            m_Indices.push_back(index + 1);
        }

        index += 2;
    }

    if (!m_Vertices.empty())
    {
        m_VertexBuffer.Create(m_Vertices);
        m_IndexBuffer.Create(m_Indices);
    }
}

void EffectTrail::Draw()
{

    if (m_Vertices.size() < 4) return;

    Renderer::SetDepthEnable(false);

    m_Camera->SetCamera(0);
    Matrix world = Matrix::Identity;
    Renderer::SetWorldMatrix(&world);

    auto* dc = Renderer::GetDeviceContext();
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    m_Shader.SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    m_Texture.SetGPU();
    m_Materiale->SetGPU();

    Renderer::SetCullMode(false);

    dc->DrawIndexed(
        static_cast<UINT>(m_Indices.size()),
        0, 0
    );

    Renderer::SetCullMode(true);

    Renderer::SetDepthEnable(true);
}

void EffectTrail::SetTexture(const char* imgname)
{
    bool sts = m_Texture.Load(imgname);
    assert(sts);
}

void EffectTrail::SetColor(const Vector4& color)
{
    m_Materiale->SetDiffuse(color);
    m_Materiale->Update();
}

void EffectTrail::Uninit()
{
}


