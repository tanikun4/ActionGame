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
    m_live = true;
}

void EffectTrail::End()
{
    m_live = false;
    m_Points.clear();
    m_Vertices.clear();
    m_Indices.clear();
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

// ポイント追加
void EffectTrail::AddPoint(const Vector3& base, const Vector3& tip)
{
    if (!m_live) return;


    // ポイントを追加する
    TrailPoint p;
    p.base = base;
    p.tip = tip;
    p.life = m_LifeTime;
    m_Points.emplace_back(p);
    m_Change = true;
}

Vector3 EffectTrail::CatmullRom(
    const Vector3& p0,const Vector3& p1,const Vector3& p2,const Vector3& p3, float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * ((2 * p1) +(-p0 + p2) * t +(2 * p0 - 5 * p1 + 4 * p2 - p3) 
        * t2 +(-p0 + 3 * p1 - 3 * p2 + p3) * t3);
}


void EffectTrail::BuildMesh()
{
    if (m_Points.size() < 4) return;

    m_Vertices.clear();
    m_Indices.clear();

    int index = 0;

    const int SUBDIV = 4; // 補間分割数

    for (size_t i = 0; i < m_Points.size() - 3; i++)
    {
        auto& p0 = m_Points[i];
        auto& p1 = m_Points[i + 1];
        auto& p2 = m_Points[i + 2];
        auto& p3 = m_Points[i + 3];

        for (int j = 0; j <= SUBDIV; j++)
        {
            float t = j / (float)SUBDIV;

            Vector3 base = CatmullRom(p0.base, p1.base, p2.base, p3.base, t);
            Vector3 tip = CatmullRom(p0.tip, p1.tip, p2.tip, p3.tip, t);

            float alpha = (float)p1.life / (float)m_LifeTime;

            VERTEX_3D v0, v1;

            v0.position = base;
            v1.position = tip;

            v0.color = Color(1, 1, 1, alpha);
            v1.color = Color(1, 1, 1, alpha);

            v0.uv = Vector2(0, 0);
            v1.uv = Vector2(1, 0);

            m_Vertices.emplace_back(v0);
            m_Vertices.emplace_back(v1);

            if (index > 0)
            {
                m_Indices.emplace_back(index - 2);
                m_Indices.emplace_back(index - 1);
                m_Indices.emplace_back(index);

                m_Indices.emplace_back(index + 1);
                m_Indices.emplace_back(index);
                m_Indices.emplace_back(index - 1);
            }

            index += 2;
        }
    }

    m_VertexBuffer.Create(m_Vertices);
    m_IndexBuffer.Create(m_Indices);
}

// 前のポイントと新しいポイントの間に補間ポイントを追加する
void EffectTrail::AddInterpolatedPoint(const Vector3& prevBase, const Vector3& prevTip,
    const Vector3& newBase, const Vector3& newTip)
{
    float distance = (newBase - prevBase).Length();

    // 補間の間隔、これより距離があれば間にポイントを追加
    const float step = 1.0f; // 適宜調整

    if (distance > step)
    {
        int steps = static_cast<int>(distance / step);

        for (int i = 1; i <= steps; i++)
        {
            float t = i / static_cast<float>(steps + 1);

            TrailPoint p;
            p.base = prevBase + (newBase - prevBase) * t;
            p.tip = prevTip + (newTip - prevTip) * t;
            p.life = m_LifeTime;

            m_Points.emplace_back(p);
        }
    }

    // 最後に本来のポイントも追加
    TrailPoint p;
    p.base = newBase;
    p.tip = newTip;
    p.life = m_LifeTime;
    m_Points.emplace_back(p);

    m_Change = true;
}

void EffectTrail::Draw()
{
    if (!m_live) return;
    if (m_Vertices.size() < 4) return;

    Renderer::SetDepthEnable(false);

    m_Camera->SetCamera(0);
    Matrix world = Matrix::Identity;
    Renderer::SetWorldMatrix(&world);

    auto* dc = Renderer::GetDeviceContext();
    //dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    dc->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_Shader.SetGPU();
    m_VertexBuffer.SetGPU();
    m_IndexBuffer.SetGPU();
    m_Texture.SetGPU();
    m_Materiale->SetGPU();

    Renderer::SetCullMode(false);

    //dc->Draw(static_cast<UINT>(m_Vertices.size()), 0);
    dc->DrawIndexed(static_cast<UINT>(m_Indices.size()),0, 0);

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


