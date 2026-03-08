#include "PostProcessManager.h"
#include "BlurManager.h"
#include "Renderer.h"
#include "Application.h"

void PostProcessManager::Init(ID3D11Device* device, int w, int h)
{
    m_ping.Create(device, w, h);
    m_pong.Create(device, w, h);

    //--------------------------------
    // sampler
    //--------------------------------

    D3D11_SAMPLER_DESC samp{};
    samp.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samp.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    samp.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    samp.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

    device->CreateSamplerState(&samp, &m_sampler);

    //--------------------------------
    // copy shader
    //--------------------------------

    Renderer::CreatePixelShader(&m_copyPS, "shader/PS_TexColor.cso");

    //--------------------------------
    // fullscreen quad
    //--------------------------------

    CreateFullscreenQuad(device);
}

void PostProcessManager::Apply(
    RenderTarget* sceneRT,
    ID3D11RenderTargetView* backBuffer)
{
    auto ctx = Renderer::GetDeviceContext();

    RenderTarget* src = sceneRT;
    RenderTarget* dst = &m_ping;

    // Blur

    if (m_enableBlur)
    {
        BlurManager::GetInstance().Blur(src, dst, &m_pong,BlurManager::Mode::Gaussian);
        std::swap(src, dst);
    }

    //--------------------------------
    // Bloom
    //--------------------------------

    if (m_enableBloom)
    {
        //BloomManager::GetInstance().Apply(src, dst);
        std::swap(src, dst);
    }

    ID3D11ShaderResourceView* nullSRV[8] = {};
    ctx->PSSetShaderResources(0, 8, nullSRV);

    //--------------------------------
    // 最終出力
    //--------------------------------

    ctx->OMSetRenderTargets(1, &backBuffer, nullptr);

    D3D11_VIEWPORT vp{};
    vp.Width = (float)Application::GetWidth();
    vp.Height = (float)Application::GetHeight();
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;

    ctx->RSSetViewports(1, &vp);

    ID3D11ShaderResourceView* srv = src->srv;
    ctx->PSSetShaderResources(0, 1, &srv);

    ctx->PSSetSamplers(0, 1, &m_sampler);

    DrawFullscreenQuad(m_copyPS);

    ID3D11ShaderResourceView* nullSRV2[1] = { nullptr };
    ctx->PSSetShaderResources(0, 1, nullSRV2);
}
void PostProcessManager::DrawFullscreenQuad(ID3D11PixelShader* ps)
{
    auto ctx = Renderer::GetDeviceContext();

    UINT stride = sizeof(FullScreenVertex);
    UINT offset = 0;

    ctx->IASetVertexBuffers(0, 1, &m_fullScreenVB, &stride, &offset);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    ctx->PSSetShader(ps, nullptr, 0);

    ctx->Draw(4, 0);
}

void PostProcessManager::CreateFullscreenQuad(ID3D11Device* device)
{
    //--------------------------------
    // フルスクリーン頂点
    //--------------------------------

    FullScreenVertex vertices[4] =
    {
        { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f }, // 左上
        {  1.0f,  1.0f, 0.0f, 1.0f, 0.0f }, // 右上
        { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f }, // 左下
        {  1.0f, -1.0f, 0.0f, 1.0f, 1.0f }, // 右下
    };

    //--------------------------------
    // 頂点バッファ
    //--------------------------------

    D3D11_BUFFER_DESC bd{};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(vertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA init{};
    init.pSysMem = vertices;

    device->CreateBuffer(&bd, &init, &m_fullScreenVB);
}

void PostProcessManager::DrawFullScreen(ID3D11PixelShader* ps)
{
    auto ctx = Renderer::GetDeviceContext();

    UINT stride = sizeof(FullScreenVertex);
    UINT offset = 0;

    ctx->IASetVertexBuffers(0, 1, &m_fullScreenVB, &stride, &offset);
    ctx->IASetInputLayout(m_inputLayout);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    ctx->VSSetShader(m_fullScreenVS, nullptr, 0);
    ctx->PSSetShader(ps, nullptr, 0);

    ctx->Draw(4, 0);
}