#pragma once
#include "Manager.h"
#include <DirectXMath.h>
#include <cmath>
#include "RenderTarget.h"


struct CBParam
{
    DirectX::XMFLOAT2 texSize;
    DirectX::XMFLOAT2 blurDir;
};

struct CBBlur
{
    DirectX::XMFLOAT4 weight[2];
};

class BlurManager : public Manager<BlurManager>
{
    friend class Manager<BlurManager>; // Manager から new できるようにする

public:
    enum class Mode { Simple, Average, Gaussian };

    void Init();
    void InitBuffers();
    //void InitRenderTargets(int screenW, int screenH);
    //void SetShaders(ID3D11PixelShader* blurX, ID3D11PixelShader* blurY, ID3D11PixelShader* average, ID3D11PixelShader* copy);

    void Blur(RenderTarget* src,RenderTarget* temp,RenderTarget* dst,Mode mode);
    
    //void ClearRenderTargets(float r, float g, float b, float a);
    //void DrawFullScreenQuad(ID3D11PixelShader* ps);
    void SetBlurDirection(float x, float y, int count, float sigma);

    //RenderTarget* GetSceneRT() { return &m_rtScene; }
    //RenderTarget* GetFinalRT() { return &m_rtFinal; }
    //ID3D11PixelShader* GetCopyPS() { return m_copy; }
    //ID3D11SamplerState* GetSampler() { return m_sampler; }

private:
    void GaussianWeights(float* weights, int count, float sigma);

private:
    ID3D11Device* m_device = nullptr;
    ID3D11DeviceContext* m_context = nullptr;

    //ID3D11Buffer* m_vb = nullptr;
    //ID3D11Buffer* m_ib = nullptr;

    ID3D11PixelShader* m_blur = nullptr;
    ID3D11PixelShader* m_average = nullptr;
    //ID3D11PixelShader* m_copy = nullptr;

    //RenderTarget m_rtScene;
    //RenderTarget m_rtFinal;

    //RenderTarget m_rtX;
    //RenderTarget m_rtY;
    int m_screenWidth = 0;
    int m_screenHeight = 0;

    ID3D11Buffer* m_cbParam = nullptr; // blur用定数バッファ
    ID3D11Buffer* m_cbBlur = nullptr;

    //ID3D11VertexShader* m_fullScreenVS = nullptr;
    //ID3D11InputLayout* m_inputLayout = nullptr;
    //ID3D11SamplerState* m_sampler = nullptr;
};

