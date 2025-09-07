#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include <d3d11.h>
#include <DirectXMath.h>
#include <SimpleMath.h>
#include <vector>
#include <cassert>
#include "Collision.h"

struct OBBWireRenderer {
    VertexBuffer<VERTEX_3D> m_VertexBuffer;
    IndexBuffer m_IndexBuffer;
    Shader m_Shader;

    ID3D11RasterizerState* m_WireFrameState = nullptr;
    ID3D11RasterizerState* m_DefaultState = nullptr;

    void Init() {
        InitOBB();

        // ラスタライザステート（ワイヤーフレーム用）
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_WIREFRAME;  // ワイヤーフレーム
        desc.CullMode = D3D11_CULL_BACK;
        desc.DepthClipEnable = TRUE;

        ID3D11Device* device = Renderer::GetDevice();
        device->CreateRasterizerState(&desc, &m_WireFrameState);

        // デフォルトステートも保持（塗りつぶし用）
        desc.FillMode = D3D11_FILL_SOLID;
        device->CreateRasterizerState(&desc, &m_DefaultState);
    }

    void InitOBB() {
        std::vector<VERTEX_3D> vertices(24);
        float hx = 0.5f, hy = 0.5f, hz = 0.5f;

        // 上面
        vertices[0].position = DirectX::SimpleMath::Vector3(-hx, hy, hz);
        vertices[1].position = DirectX::SimpleMath::Vector3(hx, hy, hz);
        vertices[2].position = DirectX::SimpleMath::Vector3(-hx, hy, -hz);
        vertices[3].position = DirectX::SimpleMath::Vector3(hx, hy, -hz);

        // 背面
        vertices[4].position = DirectX::SimpleMath::Vector3(-hx, hy, -hz);
        vertices[5].position = DirectX::SimpleMath::Vector3(hx, hy, -hz);
        vertices[6].position = DirectX::SimpleMath::Vector3(-hx, -hy, -hz);
        vertices[7].position = DirectX::SimpleMath::Vector3(hx, -hy, -hz);

        // 左面
        vertices[8].position = DirectX::SimpleMath::Vector3(-hx, -hy, hz);
        vertices[9].position = DirectX::SimpleMath::Vector3(-hx, hy, hz);
        vertices[10].position = DirectX::SimpleMath::Vector3(-hx, -hy, -hz);
        vertices[11].position = DirectX::SimpleMath::Vector3(-hx, hy, -hz);

        // 前面
        vertices[12].position = DirectX::SimpleMath::Vector3(hx, -hy, hz);
        vertices[13].position = DirectX::SimpleMath::Vector3(hx, hy, hz);
        vertices[14].position = DirectX::SimpleMath::Vector3(-hx, -hy, hz);
        vertices[15].position = DirectX::SimpleMath::Vector3(-hx, hy, hz);

        // 右面
        vertices[16].position = DirectX::SimpleMath::Vector3(hx, -hy, -hz);
        vertices[17].position = DirectX::SimpleMath::Vector3(hx, hy, -hz);
        vertices[18].position = DirectX::SimpleMath::Vector3(hx, -hy, hz);
        vertices[19].position = DirectX::SimpleMath::Vector3(hx, hy, hz);

        // 底面
        vertices[20].position = DirectX::SimpleMath::Vector3(hx, -hy, hz);
        vertices[21].position = DirectX::SimpleMath::Vector3(-hx, -hy, hz);
        vertices[22].position = DirectX::SimpleMath::Vector3(hx, -hy, -hz);
        vertices[23].position = DirectX::SimpleMath::Vector3(-hx, -hy, -hz);

        for (auto& v : vertices) {
            v.color = DirectX::SimpleMath::Color(1, 1, 1, 1);
            v.uv = DirectX::SimpleMath::Vector2(0, 0);
            v.normal = DirectX::SimpleMath::Vector3(0, 1, 0);
        }

        m_VertexBuffer.Create(vertices);

        std::vector<unsigned int> indices = {
            0,1,2, 1,3,2,
            4,5,6, 5,7,6,
            8,9,10, 9,11,10,
            12,13,14, 13,15,14,
            16,17,18, 17,19,18,
            20,21,22, 21,23,22
        };
        m_IndexBuffer.Create(indices);

        m_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");
    }

    void Draw(const Collision::OBB& obb) {
        using namespace DirectX::SimpleMath;

        Matrix r;
        r._11 = obb.m_axis[0].x; r._12 = obb.m_axis[0].y; r._13 = obb.m_axis[0].z;
        r._21 = obb.m_axis[1].x; r._22 = obb.m_axis[1].y; r._23 = obb.m_axis[1].z;
        r._31 = obb.m_axis[2].x; r._32 = obb.m_axis[2].y; r._33 = obb.m_axis[2].z;
        r._41 = 0; r._42 = 0; r._43 = 0; r._44 = 1;

        Matrix s = Matrix::CreateScale(obb.GetLen(0), obb.GetLen(1), obb.GetLen(2));
        Matrix t = Matrix::CreateTranslation(obb.m_pos);

        Matrix world = s * r * t;
        Renderer::SetWorldMatrix(&world);

        ID3D11DeviceContext* devicecontext = Renderer::GetDeviceContext();

        // ワイヤーフレームに変更
        devicecontext->RSSetState(m_WireFrameState);
        devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_Shader.SetGPU();
        m_VertexBuffer.SetGPU();
        m_IndexBuffer.SetGPU();

        devicecontext->DrawIndexed(36, 0, 0);

        // 描画後はデフォルトに戻す
        devicecontext->RSSetState(m_DefaultState);
    }
};


