#include "ParticleRenderer2D.h"
#include "ParticleParamStruct.h"
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

    m_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");
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

    m_Shader.SetGPU();
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
        //float uw = 1.0f / m_SplitX;
        //float vh = 1.0f / m_SplitY;

        //Renderer::SetUV(u, v, uw, vh);

        // 描画
        devicecontext->DrawIndexed(4, 0, 0);
    }
}


ParticleRenderer2D* ParticleRenderer2D::GetInstance()
{
    static ParticleRenderer2D instance;
    return &instance;
}