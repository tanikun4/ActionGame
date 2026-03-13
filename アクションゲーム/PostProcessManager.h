#pragma once
#include "Manager.h"
#include "RenderTarget.h"
class PostProcessManager : public Manager<PostProcessManager>
{
	friend class Manager<PostProcessManager>; // Manager Ç©ÇÁ new Ç≈Ç´ÇÈÇÊÇ§Ç…Ç∑ÇÈ
public:

    void Init(ID3D11Device* device, int w, int h);

    void Apply(RenderTarget* sceneRT,
        ID3D11RenderTargetView* backBuffer);

    void ApplyPS(
        RenderTarget* src,
        RenderTarget* dst,
        ID3D11PixelShader* ps);


    //--------------------------------
    // ã§í ï`âÊ
    //--------------------------------

    void DrawFullscreenQuad(ID3D11PixelShader* ps);

    ID3D11SamplerState* GetSampler() const { return m_sampler; }
    ID3D11PixelShader* GetCopyPS() const { return m_copyPS; }

    void EnableBlur(bool v) { m_enableBlur = v; }
    void EnableBloom(bool v) { m_enableBloom = v; }
    void EnableMono(bool v) { m_monocrome = v; }

private:

    void CreateFullscreenQuad(ID3D11Device* device);
    void DrawFullScreen(ID3D11PixelShader* ps);

    RenderTarget m_ping;
    RenderTarget m_pong;

    bool m_enableBlur = false;
    bool m_enableBloom = false;
	bool m_monocrome = false;


    //--------------------------------
   // ã§í ÉäÉ\Å[ÉX
   //--------------------------------

    ID3D11SamplerState* m_sampler = nullptr;
    ID3D11PixelShader* m_copyPS = nullptr;
    ID3D11PixelShader* m_monoPS = nullptr;

    ID3D11Buffer* m_fullScreenVB = nullptr;
    ID3D11VertexShader* m_fullScreenVS = nullptr;
    ID3D11InputLayout* m_inputLayout = nullptr;

};

