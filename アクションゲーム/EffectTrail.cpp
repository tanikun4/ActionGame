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

  //  if (!m_Points.empty())
  //  {
		//// 前のポイントと新しいポイントの間に補間ポイントを追加
  //      //const TrailPoint& last = m_Points.back();
  //      //AddInterpolatedPoint(last.base, last.tip, base, tip);

  //      // Catmull-Rom補間付きでポイントを追加
  //      AddPointCatmullRom(base, tip);

		//// Bezier曲線補間付きでポイントを追加
		////AddPointBezier(base, tip);
  //  }
  //  else
  //  {
  //      // 最初のポイントはそのまま
  //      TrailPoint p;
  //      p.base = base;
  //      p.tip = tip;
  //      p.life = m_LifeTime;
  //      m_Points.emplace_back(p);
  //      m_Change = true;
  //  }

    // 最初のポイントはそのまま
    TrailPoint p;
    p.base = base;
    p.tip = tip;
    p.life = m_LifeTime;
    m_Points.emplace_back(p);
    m_Change = true;
}

Vector3 EffectTrail::CatmullRom(
    const Vector3& p0,
    const Vector3& p1,
    const Vector3& p2,
    const Vector3& p3,
    float t)
{
    float t2 = t * t;
    float t3 = t2 * t;

    return 0.5f * (
        (2 * p1) +
        (-p0 + p2) * t +
        (2 * p0 - 5 * p1 + 4 * p2 - p3) * t2 +
        (-p0 + 3 * p1 - 3 * p2 + p3) * t3
        );
}

//void EffectTrail::BuildMesh()
//{
//    m_Vertices.clear();
//    m_Indices.clear();
//
//    if (m_Points.size() < 2)  return;
//
//    int index = 0;
//
//    for (size_t i = 0; i < m_Points.size(); i++)
//    {
//        float alpha = (float)m_Points[i].life / (float)m_LifeTime;
//
//        VERTEX_3D v0, v1;
//
//        v0.position = m_Points[i].base;
//        v1.position = m_Points[i].tip;
//
//        v0.color = Color(1, 1, 1, alpha);
//        v1.color = Color(1, 1, 1, alpha);
//
//        //v0.uv = Vector2(0, (float)i / m_Points.size());
//        //v1.uv = Vector2(1, (float)i / m_Points.size());
//
//        v0.uv = Vector2(0, (float)i / (m_Points.size() - 1));
//        v1.uv = Vector2(1, (float)i / (m_Points.size() - 1));
//
//        m_Vertices.emplace_back(v0);
//        m_Vertices.emplace_back(v1);
//
//        if (i > 0)
//        {
//            //m_Indices.emplace_back(index - 2);
//            //m_Indices.emplace_back(index - 1);
//            //m_Indices.emplace_back(index);
//            //          
//            //m_Indices.emplace_back(index);
//            //m_Indices.emplace_back(index - 1);
//            //m_Indices.emplace_back(index + 1);
//
//            int idx = static_cast<int>(i * 2);
//            m_Indices.emplace_back(idx - 2);
//            m_Indices.emplace_back(idx - 1);
//            m_Indices.emplace_back(idx);
//                      
//            m_Indices.emplace_back(idx);
//            m_Indices.emplace_back(idx - 1);
//            m_Indices.emplace_back(idx + 1);
//        }
//
//        index += 2;
//    }
//
//    if (!m_Vertices.empty())
//    {
//        m_VertexBuffer.Create(m_Vertices);
//        m_IndexBuffer.Create(m_Indices);
//    }
//}

//void EffectTrail::BuildMesh()
//{
//    if (m_Points.size() < 2) return;
//
//    m_Vertices.clear();
//    m_Indices.clear();
//
//    // TRIANGLESTRIP 用のインデックス
//    int index = 0;
//
//    for (size_t i = 0; i < m_Points.size(); ++i)
//    {
//        float alpha =  static_cast<float>(m_Points[i].life) / static_cast<float>(m_LifeTime);
//
//        // base と tip の頂点を作成
//        VERTEX_3D vBase, vTip;
//        vBase.position = m_Points[i].base;
//        vTip.position = m_Points[i].tip;
//
//        switch (i % 3) {
//        case 0:
//            vBase.color = Color(1, 0, 0, alpha);
//            vTip.color = Color(1, 0, 0, alpha);
//            break;
//        case 1:
//            vBase.color = Color(0, 1, 0, alpha);
//            vTip.color = Color(0, 1, 0, alpha);
//            break;
//        case 2:
//            vBase.color = Color(0, 0, 1, alpha);
//            vTip.color = Color(0, 0, 1, alpha);
//            break;
//        }
//
//        if (i == m_Points.size() - 1) {
//            int a = 0;
//        }
//
//        // UV は長さに沿って割り振り
//        vBase.uv = Vector2(0, static_cast<float>(i) / static_cast<float>(m_Points.size() - 1));
//        vTip.uv = Vector2(1, static_cast<float>(i) / static_cast<float>(m_Points.size() - 1));
//
//        m_Vertices.push_back(vBase);
//        m_Vertices.push_back(vTip);
//
//        // TRIANGLESTRIP のインデックス生成
//        if (i > 0)
//        {
//            // 直前の 2頂点と今回の 2頂点で四角形を作る
//            m_Indices.push_back(index - 2);
//            m_Indices.push_back(index - 1);
//            m_Indices.push_back(index);
//
//            m_Indices.push_back(index + 1);
//            m_Indices.push_back(index);
//            m_Indices.push_back(index - 1);
//        }
//
//        index += 2;
//    }
//
//    // GPU に転送
//    if (!m_Vertices.empty())
//    {
//        m_VertexBuffer.Create(m_Vertices);
//        m_IndexBuffer.Create(m_Indices);
//    }
//}

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

            m_Vertices.push_back(v0);
            m_Vertices.push_back(v1);

            if (index > 0)
            {
                m_Indices.push_back(index - 2);
                m_Indices.push_back(index - 1);
                m_Indices.push_back(index);

                m_Indices.push_back(index + 1);
                m_Indices.push_back(index);
                m_Indices.push_back(index - 1);
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

// Catmull-Rom補間付きでポイントを追加
//void EffectTrail::AddPointCatmullRom(const Vector3& newBase, const Vector3& newTip)
//{
//    // 新しいポイント用
//    TrailPoint newP;
//    newP.base = newBase;
//    newP.tip = newTip;
//    newP.life = m_LifeTime;
//
//    size_t count = m_Points.size();
//
//    // ポイントが2つ以上ある場合のみ補間
//    if (count >= 2)
//    {
//        const int split = 20; // 1区間に何ポイント追加するか
//        TrailPoint& prev = m_Points[count - 1];
//        TrailPoint& prev2 = m_Points[count - 2];
//
//        Vector3 p0Base = (count >= 3) ? m_Points[count - 3].base : prev2.base;
//        Vector3 p0Tip = (count >= 3) ? m_Points[count - 3].tip : prev2.tip;
//
//        Vector3 p1Base = prev2.base;
//        Vector3 p1Tip = prev2.tip;
//        Vector3 p2Base = prev.base;
//        Vector3 p2Tip = prev.tip;
//        Vector3 p3Base = newBase;
//        Vector3 p3Tip = newTip;
//
//        for (int i = 1; i <= split; ++i)
//        {
//            float t = i / static_cast<float>(split + 1);
//
//            TrailPoint interp;
//            interp.base = DirectX::XMVectorCatmullRom(
//                DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&p0Base)),
//                DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&p1Base)),
//                DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&p2Base)),
//                DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&p3Base)),
//                t
//            );
//
//            interp.tip = DirectX::XMVectorCatmullRom(
//                DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&p0Tip)),
//                DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&p1Tip)),
//                DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&p2Tip)),
//                DirectX::XMLoadFloat3(reinterpret_cast<DirectX::XMFLOAT3*>(&p3Tip)),
//                t
//            );
//
//            interp.life = m_LifeTime;
//            m_Points.emplace_back(interp);
//        }
//    }
//
//    // 新しいポイントを追加
//    m_Points.emplace_back(newP);
//    m_Change = true;
//}

// Catmull-Rom補間付きでポイントを追加（滑らかな軌跡）
void EffectTrail::AddPointCatmullRom(const Vector3& newBase, const Vector3& newTip)
{
    if (!m_live) return;

    // 新しいポイント
    TrailPoint newP;
    newP.base = newBase;
    newP.tip = newTip;
    newP.life = m_LifeTime;

    size_t count = m_Points.size();

    if (count >= 2)
    {
        // 過去3点＋新しい点を使う
        Vector3 p0Base = (count >= 3) ? m_Points[count - 3].base : m_Points[count - 2].base;
        Vector3 p0Tip = (count >= 3) ? m_Points[count - 3].tip : m_Points[count - 2].tip;

        Vector3 p1Base = m_Points[count - 2].base;
        Vector3 p1Tip = m_Points[count - 2].tip;

        Vector3 p2Base = m_Points[count - 1].base;
        Vector3 p2Tip = m_Points[count - 1].tip;

        Vector3 p3Base = newBase;
        Vector3 p3Tip = newTip;

        // 区間距離に応じて分割数を決める（距離が大きいほど補間点を増やす）
        float distance = (p2Base - p1Base).Length();
        int split = std::max(2, static_cast<int>(distance / 1.0f)); // 1.0fは1ユニットごとに1点追加

        for (int i = 1; i <= split; ++i)
        {
            float t = i / static_cast<float>(split + 1);

            TrailPoint interp;

            // Catmull-Rom補間（DirectX::SimpleMath::Vector3 は XMVector を介して）
            interp.base = DirectX::XMVectorCatmullRom(
                DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&p0Base)),
                DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&p1Base)),
                DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&p2Base)),
                DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&p3Base)),
                t
            );
            interp.tip = DirectX::XMVectorCatmullRom(
                DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&p0Tip)),
                DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&p1Tip)),
                DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&p2Tip)),
                DirectX::XMLoadFloat3(reinterpret_cast<const DirectX::XMFLOAT3*>(&p3Tip)),
                t
            );

            interp.life = m_LifeTime;
            m_Points.emplace_back(interp);
        }
    }

    // 新しいポイントを追加
    m_Points.emplace_back(newP);
    m_Change = true;
}

// Cubic Bezier 補間でポイントを追加
//void EffectTrail::AddPointBezier(const Vector3& newBase, const Vector3& newTip)
//{
//    if (!m_live) return;
//
//    TrailPoint newP;
//    newP.base = newBase;
//    newP.tip = newTip;
//    newP.life = m_LifeTime;
//
//    size_t count = m_Points.size();
//
//    // 最低2ポイントないと補間できない
//    if (count >= 2)
//    {
//        const int split = 10; // 1区間に何ポイント追加するか
//        const TrailPoint& prev = m_Points[count - 1];
//        const TrailPoint& prev2 = m_Points[count - 2];
//
//        // 制御点を前後の速度ベクトルで作る
//        Vector3 tangentBase = (prev.base - prev2.base) * 0.5f;
//        Vector3 tangentTip = (prev.tip - prev2.tip) * 0.5f;
//
//        Vector3 p0Base = prev2.base;
//        Vector3 p1Base = prev2.base + tangentBase; // 制御点1
//        Vector3 p2Base = prev.base - tangentBase; // 制御点2
//        Vector3 p3Base = newBase;
//
//        Vector3 p0Tip = prev2.tip;
//        Vector3 p1Tip = prev2.tip + tangentTip;
//        Vector3 p2Tip = prev.tip - tangentTip;
//        Vector3 p3Tip = newTip;
//
//        for (int i = 1; i <= split; ++i)
//        {
//            float t = i / static_cast<float>(split + 1);
//
//            TrailPoint interp;
//            interp.base = BezierCubic(p0Base, p1Base, p2Base, p3Base, t);
//            interp.tip = BezierCubic(p0Tip, p1Tip, p2Tip, p3Tip, t);
//            interp.life = m_LifeTime;
//
//            m_Points.emplace_back(interp);
//        }
//    }
//
//    // 新しいポイントを追加
//    m_Points.emplace_back(newP);
//    m_Change = true;
//}

// ----------------------------------------------------------------------------
// Bezier補間付きでポイントを追加（距離に応じて分割）
// ----------------------------------------------------------------------------
void EffectTrail::AddPointBezier(const Vector3& newBase, const Vector3& newTip)
{
    TrailPoint newP;
    newP.base = newBase;
    newP.tip = newTip;
    newP.life = m_LifeTime;

    if (!m_Points.empty())
    {
        const TrailPoint& last = m_Points.back();

        // 前後ポイントの距離を計算（base/tipで最大を使用）
        float distBase = (newBase - last.base).Length();
        float distTip = (newTip - last.tip).Length();
        float dist = std::max(distBase, distTip);

        const float step = 1.0f; // 1ユニットごとに補間
        if (dist > step)
        {
            int steps = static_cast<int>(dist / step);

            // 過去のポイントを取得（2つ前まで）
            Vector3 p0Base = (m_Points.size() >= 2) ? m_Points[m_Points.size() - 2].base : last.base;
            Vector3 p0Tip = (m_Points.size() >= 2) ? m_Points[m_Points.size() - 2].tip : last.tip;
            Vector3 p1Base = last.base;
            Vector3 p1Tip = last.tip;
            Vector3 p2Base = newBase;
            Vector3 p2Tip = newTip;

            // 制御点 C1, C2 を Catmull-Rom のように自動計算して曲線にする
            Vector3 c1Base = p1Base + (p2Base - p0Base) * 0.25f; // 前後差分で自然なカーブ
            Vector3 c1Tip = p1Tip + (p2Tip - p0Tip) * 0.25f;
            Vector3 c2Base = p2Base - (p2Base - p0Base) * 0.25f;
            Vector3 c2Tip = p2Tip - (p2Tip - p0Tip) * 0.25f;

            // 補間
            for (int i = 1; i <= steps; ++i)
            {
                float t = i / static_cast<float>(steps + 1);

                TrailPoint interp;

                auto Bezier = [](const Vector3& P0, const Vector3& C1, const Vector3& C2, const Vector3& P3, float t)
                    {
                        float u = 1 - t;
                        return u * u * u * P0 + 3 * u * u * t * C1 + 3 * u * t * t * C2 + t * t * t * P3;
                    };

                interp.base = Bezier(p1Base, c1Base, c2Base, p2Base, t);
                interp.tip = Bezier(p1Tip, c1Tip, c2Tip, p2Tip, t);
                interp.life = m_LifeTime;

                m_Points.emplace_back(interp);
            }
        }
    }

    // 最後に本来のポイントも追加
    m_Points.emplace_back(newP);
    m_Change = true;
}

// Cubic Bezier計算
Vector3 EffectTrail::BezierCubic(const Vector3& P0, const Vector3& P1, const Vector3& P2, const Vector3& P3, float t)
{
    float u = 1.0f - t;
    return u * u * u * P0 + 3 * u * u * t * P1 + 3 * u * t * t * P2 + t * t * t * P3;
}

void EffectTrail::Draw()
{

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


