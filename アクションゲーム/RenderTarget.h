#pragma once
#include <d3d11.h>
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

struct FullScreenVertex
{
    float x, y, z;
    float u, v;
};
