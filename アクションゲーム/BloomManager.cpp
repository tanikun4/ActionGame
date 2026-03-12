#include "BloomManager.h"
#include "BlurManager.h"
bool BloomManager::Initialize(ID3D11Device* device, int width, int height)
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

    device->CreateTexture2D(&desc, nullptr, &m_brightTex);
    device->CreateTexture2D(&desc, nullptr, &m_tempTex);
    device->CreateTexture2D(&desc, nullptr, &m_bloomTex);

    device->CreateRenderTargetView(m_brightTex, nullptr, &m_brightRTV);
    device->CreateRenderTargetView(m_tempTex, nullptr, &m_tempRTV);
    device->CreateRenderTargetView(m_bloomTex, nullptr, &m_bloomRTV);

    device->CreateShaderResourceView(m_brightTex, nullptr, &m_brightSRV);
    device->CreateShaderResourceView(m_tempTex, nullptr, &m_tempSRV);
    device->CreateShaderResourceView(m_bloomTex, nullptr, &m_bloomSRV);

    return true;
}

//void BloomManager::Apply(RenderTarget* scene, RenderTarget* dst)
//{
//	auto& m_blur = BlurManager::GetInstance();
//
//    //--------------------------------
//    // ‡@ BrightPass
//    //--------------------------------
//
//    m_context->OMSetRenderTargets(1, &m_brightRTV ,nullptr);
//
//    m_context->PSSetShaderResources(0, 1, &scene->srv);
//
//    m_blur.DrawFullScreenQuad(m_brightPS);
//
//    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
//    m_context->PSSetShaderResources(0, 1, nullSRV);
//
//
//    //--------------------------------
//    // ‡A GaussianBlur
//    //--------------------------------
//
//    m_blur.Blur(&m_rtBright, &m_bloomRTV, BlurManager::Mode::Gaussian);
//
//
//    //--------------------------------
//    // ‡B Combine
//    //--------------------------------
//
//    m_context->OMSetRenderTargets(1, &dst->rtv, nullptr);
//
//    ID3D11ShaderResourceView* srvs[2] =
//    {
//        scene->srv,
//        m_rtBloom.srv
//    };
//
//    m_context->PSSetShaderResources(0, 2, srvs);
//
//    m_blur.DrawFullScreenQuad(m_combinePS);
//
//    m_context->PSSetShaderResources(0, 1, nullSRV);
//}