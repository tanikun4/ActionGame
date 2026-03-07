#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <cmath>
#include "Manager.h"

struct RenderTarget
{
    ID3D11Texture2D* texture = nullptr;
    ID3D11RenderTargetView* rtv = nullptr;
    ID3D11ShaderResourceView* srv = nullptr;

    int width = 0;
    int height = 0;

    void Create(ID3D11Device* device, int w, int h, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM)
    {
        width = w;
        height = h;

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = w;
        desc.Height = h;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        device->CreateTexture2D(&desc, nullptr, &texture);
        device->CreateRenderTargetView(texture, nullptr, &rtv);
        device->CreateShaderResourceView(texture, nullptr, &srv);
    }

    void Clear(ID3D11DeviceContext* context, float r, float g, float b, float a)
    {
        float color[4] = { r, g, b, a };
        context->ClearRenderTargetView(rtv, color);
    }
};

struct Vertex
{
    DirectX::XMFLOAT3 pos;
    DirectX::XMFLOAT2 uv;
};

struct CBParam
{
    DirectX::XMFLOAT2 texSize;      // テクスチャサイズ
    DirectX::XMFLOAT2 blurDir;      // ブラー方向
    float weights[32];              // 最大32サンプル
    int sampleCount;                // 実際に使うサンプル数
    float pad[3];                   // 16バイト境界調整
};

class BlurManager : public Manager<BlurManager>
{
    friend class Manager<BlurManager>; // Manager から new できるようにする

public:
    enum class Mode { Simple, Average, Gaussian };

    void Init(ID3D11Device* device, ID3D11DeviceContext* context);
    void InitBuffers();
    void InitRenderTargets(int screenW, int screenH);
    void SetShaders(ID3D11PixelShader* blurX, ID3D11PixelShader* blurY, ID3D11PixelShader* average, ID3D11PixelShader* copy);

    void Blur(RenderTarget* src, RenderTarget* dst, Mode mode, ID3D11BlendState* blendState);
    void ClearRenderTargets(float r, float g, float b, float a);
    void DrawFullScreenQuad(ID3D11PixelShader* ps);
    void SetBlurDirection(float x, float y, int count, float sigma);

    RenderTarget* GetSceneRT() { return &m_rtScene; }
    RenderTarget* GetFinalRT() { return &m_rtFinal; }
    ID3D11PixelShader* GetCopyPS() { return m_copy; }
    ID3D11SamplerState* GetSampler() { return m_sampler; }

private:
    void GaussianWeights(float* weights, int count, float sigma);

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    ID3D11Buffer* m_vb = nullptr;
    ID3D11Buffer* m_ib = nullptr;

    ID3D11PixelShader* m_blurX = nullptr;
    ID3D11PixelShader* m_blurY = nullptr;
    ID3D11PixelShader* m_average = nullptr;
    ID3D11PixelShader* m_copy = nullptr;

    RenderTarget m_rtScene;
    RenderTarget m_rtFinal;

    RenderTarget m_rtX;
    RenderTarget m_rtY;
    int m_screenWidth = 0;
    int m_screenHeight = 0;

    ID3D11Buffer* m_cbParam = nullptr; // blur用定数バッファ

    ID3D11VertexShader* m_fullScreenVS = nullptr;
    ID3D11InputLayout* m_inputLayout = nullptr;
    ID3D11SamplerState* m_sampler = nullptr;
};

