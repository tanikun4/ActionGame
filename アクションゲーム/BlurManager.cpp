#include "BlurManager.h"
#include "dx11helper.h"
#include "PostProcessManager.h"
#include "Renderer.h"
#include "Application.h"

//void BlurManager::Init(ID3D11Device* device, ID3D11DeviceContext* context)
//{
//    m_device = device;
//    m_context = context;
//
//    // -----------------------------------
//   // フルスクリーン専用InputLayout
//   // -----------------------------------
//    D3D11_INPUT_ELEMENT_DESC layout[] =
//    {
//        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
//          D3D11_INPUT_PER_VERTEX_DATA, 0 },
//
//        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
//          D3D11_INPUT_PER_VERTEX_DATA, 0 },
//    };
//
//    // -----------------------------------
//    // フルスクリーンVS作成
//    // -----------------------------------
//    bool sts = CreateVertexShader(
//        device,
//        "shader/FullScreenVS.hlsl",  
//        "vs_main",                       
//        "vs_5_0",
//        layout,
//        2,
//        &m_fullScreenVS,
//        &m_inputLayout);
//
//    if (!sts)
//    {
//        MessageBox(nullptr, "FullScreen VS Create Error", "error", MB_OK);
//        return;
//    }
//
//    D3D11_SAMPLER_DESC sampDesc = {};
//    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
//    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
//    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
//
//    m_device->CreateSamplerState(&sampDesc, &m_sampler);
//
//
//    InitBuffers();
//}

//void BlurManager::InitBuffers()
//{
//    Vertex vertices[4] = {
//        { {-1,  1, 0}, {0, 0} },
//        { { 1,  1, 0}, {1, 0} },
//        { {-1, -1, 0}, {0, 1} },
//        { { 1, -1, 0}, {1, 1} }
//    };
//    D3D11_BUFFER_DESC vbd = {};
//    vbd.Usage = D3D11_USAGE_DEFAULT;
//    vbd.ByteWidth = sizeof(vertices);
//    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//    D3D11_SUBRESOURCE_DATA vinit = { vertices };
//    m_device->CreateBuffer(&vbd, &vinit, &m_vb);
//
//    unsigned short indices[6] = { 0, 1, 2, 2, 1, 3 };
//    D3D11_BUFFER_DESC ibd = {};
//    ibd.Usage = D3D11_USAGE_DEFAULT;
//    ibd.ByteWidth = sizeof(indices);
//    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
//    D3D11_SUBRESOURCE_DATA iinit = { indices };
//    m_device->CreateBuffer(&ibd, &iinit, &m_ib);
//
//    D3D11_BUFFER_DESC cbd = {};
//    cbd.Usage = D3D11_USAGE_DYNAMIC;
//    cbd.ByteWidth = sizeof(CBParam);
//    cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//    cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//
//    m_device->CreateBuffer(&cbd, nullptr, &m_cbParam);
//
//    cbd.ByteWidth = sizeof(CBBlur);
//    m_device->CreateBuffer(&cbd, nullptr, &m_cbBlur);
//}

//void BlurManager::InitRenderTargets(int screenW, int screenH)
//{
//    m_screenWidth = screenW;
//    m_screenHeight = screenH;
//
//    m_rtX.Create(m_device, screenW , screenH);  // 横ブラー用
//    m_rtY.Create(m_device, screenW, screenH );  // 縦ブラー用
//
//    // シーン描画用
//    m_rtScene.Create(m_device, screenW, screenH);
//
//    // ブラー後の最終出力用
//    m_rtFinal.Create(m_device, screenW, screenH);
//}

// ブラーシェーダーをセット
void BlurManager::Init()
{
    Renderer::CreatePixelShader(&m_blur, "shader/PS_GaussianBlur.cso");

    Renderer::CreatePixelShader(&m_average, "shader/PS_AverageBlur.cso");

}

//// フルスクリーンにクアッドを描画する共通関数
//void BlurManager::DrawFullScreenQuad(ID3D11PixelShader* ps)
//{
//    
//    UINT stride = sizeof(FullScreenVertex);
//    UINT offset = 0;
//
//    m_context->IASetInputLayout(m_inputLayout);
//    m_context->IASetVertexBuffers(0, 1, &m_vb, &stride, &offset);
//    m_context->IASetIndexBuffer(m_ib, DXGI_FORMAT_R16_UINT, 0);
//    m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//    // 必ずVSをセット
//    m_context->VSSetShader(m_fullScreenVS, nullptr, 0);
//
//    m_context->PSSetShader(ps, nullptr, 0);
//
//    m_context->DrawIndexed(6, 0, 0);
//}

// ガウシアンブラーの重みを計算
void BlurManager::GaussianWeights(float* weights, int count, float sigma)
{
    float total = 0.0f;
    for (int i = 0; i < count; i++)
    {
        weights[i] = std::expf(-0.5f * (i * i) / sigma);
        total += (i == 0) ? weights[i] : 2.0f * weights[i];
    }
    for (int i = 0; i < count; i++) weights[i] /= total;
}

// ブラー処理
void BlurManager::Blur(
    RenderTarget* src,
    RenderTarget* temp,
    RenderTarget* dst,
    Mode mode)
{
    auto ctx = m_context;

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };

    D3D11_VIEWPORT vp{};
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

	auto& post = PostProcessManager::GetInstance();
    switch (mode)
    {

        //--------------------------------
        // Average
        //--------------------------------

    case Mode::Average:
    {
        // src → temp
        ctx->OMSetRenderTargets(1, &temp->rtv, nullptr);

        vp.Width = (FLOAT)temp->width;
        vp.Height = (FLOAT)temp->height;
        ctx->RSSetViewports(1, &vp);

        ctx->PSSetShaderResources(0, 1, &src->srv);
        post.DrawFullscreenQuad(m_average);

        ctx->PSSetShaderResources(0, 1, nullSRV);

    }
    break;

    //--------------------------------
    // Gaussian
    //--------------------------------

    case Mode::Gaussian:
    {
        // 横ブラー

        SetBlurDirection(1, 0, 2, 5);

        ctx->OMSetRenderTargets(1, &temp->rtv, nullptr);

        vp.Width = (FLOAT)temp->width;
        vp.Height = (FLOAT)temp->height;
        ctx->RSSetViewports(1, &vp);

        ctx->PSSetShaderResources(0, 1, &src->srv);
        post.DrawFullscreenQuad(m_blur);

        ctx->PSSetShaderResources(0, 1, nullSRV);

        // 縦ブラー

        SetBlurDirection(0, 1, 2, 5);

        ctx->OMSetRenderTargets(1, &dst->rtv, nullptr);

        vp.Width = (FLOAT)dst->width;
        vp.Height = (FLOAT)dst->height;
        ctx->RSSetViewports(1, &vp);

        ctx->PSSetShaderResources(0, 1, &temp->srv);
        post.DrawFullscreenQuad(m_blur);

        ctx->PSSetShaderResources(0, 1, nullSRV);
    }
    break;
    }
    // SRVを解除
    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    m_context->PSSetShaderResources(0, 1, nullSRV);
}

//void BlurManager::ClearRenderTargets(float r, float g, float b, float a)
//{
//    m_rtX.Clear(m_context, r, g, b, a);
//    m_rtY.Clear(m_context, r, g, b, a);
//}

void BlurManager::SetBlurDirection(float x, float y, int count, float sigma)
{
    CBParam cb{};
    cb.texSize = { Application::GetWidth(), Application::GetHeight() };
    cb.blurDir = { x, y };
    
    CBBlur cbb{};
    cbb.weight[0] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f };
    cbb.weight[1] = { 0.016216f, 0.0f, 0.0f, 0.0f };

    //GaussianWeights(cbb.weight, count, sigma);


    D3D11_MAPPED_SUBRESOURCE mapped;
    m_context->Map(m_cbParam, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &cb, sizeof(cb));
    m_context->Unmap(m_cbParam, 0);

    m_context->PSSetConstantBuffers(0, 1, &m_cbParam);

    cbb.weight[0] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f };
    cbb.weight[1] = { 0.016216f, 0.0f, 0.0f, 0.0f };

    m_context->Map(m_cbBlur, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
    memcpy(mapped.pData, &cbb, sizeof(cbb));
    m_context->Unmap(m_cbBlur, 0);

    m_context->PSSetConstantBuffers(1, 1, &m_cbBlur);  // b1
}