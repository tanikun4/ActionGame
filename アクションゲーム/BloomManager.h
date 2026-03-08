#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <cmath>
#include "Manager.h"
class BloomManager :
    public Manager<BloomManager>
{
    friend class Manager<BloomManager>; // Manager ‚©‚ç new ‚Å‚«‚é‚æ‚¤‚É‚·‚é
public:

    bool Initialize(
        ID3D11Device* device,
        int width,
        int height);

    void Apply(
        ID3D11DeviceContext* context,
        ID3D11ShaderResourceView* sceneSRV,
        ID3D11RenderTargetView* backBufferRTV);

private:

    int m_width;
    int m_height;

    // RenderTarget
    ID3D11Texture2D* m_brightTex = nullptr;
    ID3D11Texture2D* m_tempTex = nullptr;
    ID3D11Texture2D* m_bloomTex = nullptr;

    ID3D11RenderTargetView* m_brightRTV = nullptr;
    ID3D11RenderTargetView* m_tempRTV = nullptr;
    ID3D11RenderTargetView* m_bloomRTV = nullptr;

    ID3D11ShaderResourceView* m_brightSRV = nullptr;
    ID3D11ShaderResourceView* m_tempSRV = nullptr;
    ID3D11ShaderResourceView* m_bloomSRV = nullptr;

    // shaders
    ID3D11PixelShader* m_brightPS = nullptr;
    ID3D11PixelShader* m_combinePS = nullptr;

};

