#pragma once
#include <vector>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Camera.h"

class SharedEffect2DData;
class ParticleParam2D;
class ParticleDrawData2D;

class ParticleRenderer2D
{
public:

    static ParticleRenderer2D* GetInstance();

    void Init();
    void Init(
        Camera* camera,
        SharedEffect2DData& data);


    void Draw(const std::vector<ParticleParam2D>& particles);
    void Draw(const std::vector<ParticleParam2D>& particles, int textureID);


    void Draw(const ParticleDrawData2D& data);

    ParticleRenderer2D() = default;

private:
    // 共有リソース
    VertexBuffer<VERTEX_3D>* m_VertexBuffer = nullptr;
    IndexBuffer* m_IndexBuffer = nullptr;

    ID3D11VertexShader* m_vs = nullptr;
    ID3D11PixelShader* m_ps = nullptr;

    Camera* m_Camera;
};
