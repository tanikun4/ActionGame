#pragma once
#include <d3d11.h>
#include <DirectXMath.h>
#include <cmath>
#include "Manager.h"
#include "RenderTarget.h"
class BloomManager :
    public Manager<BloomManager>
{
    friend class Manager<BloomManager>; // Manager ‚©‚ç new ‚Å‚«‚é‚æ‚¤‚É‚·‚é
public:

    bool Init(
        ID3D11Device* device,
        int width,
        int height);

    void Apply(RenderTarget* scene, RenderTarget* dst);

private:
    struct BloomParam
    {
        float intensity;
        float padding[3];
    };

    int m_width;
    int m_height;

    // RenderTarget
    ID3D11Texture2D* m_brightTex = nullptr;
    ID3D11Texture2D* m_bloomTex = nullptr;

	RenderTarget m_brightRT;
	RenderTarget m_bloomRT;

    ID3D11Buffer* m_cbBloom = nullptr;
    ID3D11Buffer* m_cbBrightPass = nullptr;

	float m_bright = 0.5f;
	float m_bloom = 0.15f;

    //ID3D11RenderTargetView* m_brightRTV = nullptr;
    //ID3D11RenderTargetView* m_bloomRTV = nullptr;

    //ID3D11ShaderResourceView* m_brightSRV = nullptr;
    //ID3D11ShaderResourceView* m_bloomSRV = nullptr;

    // shaders
    ID3D11PixelShader* m_brightPS = nullptr;
    ID3D11PixelShader* m_combinePS = nullptr;

};

