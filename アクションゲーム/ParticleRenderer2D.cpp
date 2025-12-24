#include "ParticleRenderer2D.h"
#include "ParticleStruct.h"
#include "EffectDataStruct.h"

using namespace std;
using namespace DirectX::SimpleMath;

void ParticleRenderer2D::Init()
{
    // 1枚板ポリのVB / IBを生成
    // ※ 既存の2D Billboardと共通でOK

    // Shader 読み込み（専用 or 既存流用）
}

void ParticleRenderer2D::Init(Camera* camera, SharedEffect2DData& data)
{
    // カメラ取得
    m_Camera = camera;

    // 頂点バッファ取得
    m_VertexBuffer = data.m_2DVertexBuffer.get();

    // インデックスバッファ取得
    m_IndexBuffer = data.m_2DIndexBuffer.get();

    //_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");
}

void ParticleRenderer2D::Draw(
    const std::vector<ParticleParam2D>& particles)
{
    if (particles.empty()) return;

    // カメラ設定（UI or World はここで切り替え可）
    m_Camera->SetCamera(0);

    // 深度設定（タイトル用なら false にしてもOK）
    Renderer::SetDepthEnable(true);

    // ビュー行列取得
    Matrix view = m_Camera->GetViewMatrix();

    // 平行移動を消す
    view.Translation(Vector3(0, 0, 0));

    // ビルボード行列作成
    Matrix billboard = view.Transpose();

    // GPUセットアップ（共通部分）
    ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();

    devicecontext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    //m_Shader.SetGPU();
    m_VertexBuffer->SetGPU();
    m_IndexBuffer->SetGPU();
    //m_Texture->SetGPU();
    //m_Material->SetGPU();

    // 各パーティクル描画
    for (const auto& p : particles)
    {
        // Z回転のみ
        Matrix selfRotZ = Matrix::CreateRotationZ(p.rot);

        // SRT
        Matrix s = Matrix::CreateScale(p.scale.x, p.scale.y, 1);
        Matrix t = Matrix::CreateTranslation(p.pos);

        Matrix world =
            s *
            selfRotZ *
            billboard *
            t;

        Renderer::SetWorldMatrix(&world);

        // UV計算
        //float u = static_cast<float>(p.numU - 1);
        //float v = static_cast<float>(p.numV - 1);
        float uw = 1.0f / p.maxuv.x;
        float vh = 1.0f / p.maxuv.y;

        Renderer::SetUV(p.uv.x - 1, p.uv.y - 1, uw, vh);

        // 描画
        devicecontext->DrawIndexed(4, 0, 0);
    }
}

void ParticleRenderer2D::Draw(const ParticleDrawData2D& data)
{
    if (!data.particles || data.particles->empty()) return;

    // UV分割（エミッタ単位で事前計算してもOK）
    const float uw = 1.0f / data.maxuv.x;
    const float vh = 1.0f / data.maxuv.y;

    // Camera、Depth 設定
    if (data.isUI)
    {
        m_Camera->SetCamera(1);
        Renderer::SetDepthEnable(false);
    }
    else
    {
        m_Camera->SetCamera(0);
        Renderer::SetDepthEnable(true);
    }

    // Billboard 行列（3Dのみ）
    Matrix billboard = Matrix::Identity;
    if (!data.isUI)
    {
        Matrix view = m_Camera->GetViewMatrix();
        view.Translation(Vector3(0, 0, 0));
        billboard = view.Transpose();
    }

    // GPU 共通セットアップ
    ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();

    devicecontext->IASetPrimitiveTopology(
        D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    data.shader->SetGPU();
    m_VertexBuffer->SetGPU();
    m_IndexBuffer->SetGPU();
    data.texture->SetGPU();
    data.material->SetGPU();

    // パーティクル描画
    for (const auto& p : *data.particles)
    {
        Matrix s = Matrix::CreateScale(p.scale.x, p.scale.y, 1.0f);
        Matrix r = Matrix::CreateRotationZ(p.rot);
        Matrix t = Matrix::CreateTranslation(p.pos);

        Matrix world;
        if (data.isUI)
        {
            // UI：そのまま
            world = s * r * t;
        }
        else
        {
            // 3D：Billboard
            world = s * r * billboard * t;
        }

        Renderer::SetWorldMatrix(&world);

        Renderer::SetUV(
            p.uv.x - 1,
            p.uv.y - 1,
            uw,
            vh);

        devicecontext->DrawIndexed(4, 0, 0);
    }

    // 後始末（UI描画後に深度を戻すなら）
    if (data.isUI)
    {
        Renderer::SetDepthEnable(true);
    }
}


ParticleRenderer2D* ParticleRenderer2D::GetInstance()
{
    static ParticleRenderer2D instance;
    return &instance;
}