#include "BloomManager.h"
#include "BlurManager.h"
#include "PostProcessManager.h"
#include "Renderer.h"

bool BloomManager::Init(ID3D11Device* device, int width, int height)
{
    m_width = width;
    m_height = height;

    D3D11_TEXTURE2D_DESC desc{};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.BindFlags =
        D3D11_BIND_RENDER_TARGET |
        D3D11_BIND_SHADER_RESOURCE;

    D3D11_BUFFER_DESC budesc{};
    budesc.ByteWidth = sizeof(BloomParam);
    budesc.Usage = D3D11_USAGE_DYNAMIC;
    budesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    budesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    device->CreateBuffer(&budesc, nullptr, &m_cbBloom);

    device->CreateBuffer(&budesc, nullptr, &m_cbBrightPass);

    // ======================
    // Bright RT
    // ======================

    m_brightRT.Create(device,width,height);


    // ======================
    // Bloom RT
    // ======================


    m_bloomRT.Create(device, width, height);

    // ======================
    // Shaders
    // ======================

    Renderer::CreatePixelShader(&m_brightPS, "shader/PS_Luminance.cso");

    Renderer::CreatePixelShader(&m_combinePS, "shader/BloomCombinePS.cso");

    return true;
}

void BloomManager::Apply(RenderTarget* scene, RenderTarget* dst)
{
	auto& post = PostProcessManager::GetInstance();
	auto& m_blur = BlurManager::GetInstance();
	auto ctx = Renderer::GetDeviceContext();

    //--------------------------------
    // ‡@ BrightPass
    //--------------------------------

    ctx->OMSetRenderTargets(1, &m_brightRT.rtv ,nullptr);

    BloomParam param{};
    param.intensity = 0.75f;

    D3D11_MAPPED_SUBRESOURCE mapped{};
    ctx->Map(m_cbBrightPass, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    memcpy(mapped.pData, &param, sizeof(param));

    ctx->Unmap(m_cbBrightPass, 0);

    ctx->PSSetConstantBuffers(0, 1, &m_cbBrightPass);

    ctx->PSSetShaderResources(0, 1, &scene->srv);

    post.DrawFullscreenQuad(m_brightPS);

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    ctx->PSSetShaderResources(0, 1, nullSRV);


    //--------------------------------
    // ‡A GaussianBlur
    //--------------------------------

    m_blur.Blur(&m_brightRT, dst, &m_bloomRT, BlurManager::Mode::Gaussian);


    //--------------------------------
    // ‡B Combine
    //--------------------------------

    //BloomParam param{};
    param.intensity = 0.3f;   // Bloom‹­‚³

    //D3D11_MAPPED_SUBRESOURCE mapped{};
    ctx->Map(m_cbBloom, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

    memcpy(mapped.pData, &param, sizeof(param));

    ctx->Unmap(m_cbBloom, 0);

    ctx->PSSetConstantBuffers(0, 1, &m_cbBloom);

    ctx->OMSetRenderTargets(1, &dst->rtv, nullptr);

    ID3D11ShaderResourceView* srvs[2] =
    {
        scene->srv,
        m_bloomRT.srv
    };

    ctx->PSSetShaderResources(0, 2, srvs);

    post.DrawFullscreenQuad(m_combinePS);

  //  ctx->PSSetShaderResources(0, 1, nullSRV);
    ID3D11ShaderResourceView* nullSRV2[2] = { nullptr, nullptr };
    ctx->PSSetShaderResources(0, 2, nullSRV2);
}