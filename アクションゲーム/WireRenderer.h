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

struct WireRenderer {
    VertexBuffer<VERTEX_3D> m_OBBVertexBuffer;
    IndexBuffer m_OBBIndexBuffer;

    VertexBuffer<VERTEX_3D> m_SphereVertexBuffer;
    IndexBuffer m_SphereIndexBuffer;

    Shader m_Shader;

    ID3D11RasterizerState* m_WireFrameState = nullptr;
    ID3D11RasterizerState* m_DefaultState = nullptr;

    void Init() {
        InitOBB();
        InitSphere();

        // ラスタライザステート（ワイヤーフレーム用）
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_NONE; // 球は裏面も見たい
        desc.DepthClipEnable = TRUE;

        ID3D11Device* device = Renderer::GetDevice();
        device->CreateRasterizerState(&desc, &m_WireFrameState);

        // デフォルトステート
        desc.FillMode = D3D11_FILL_SOLID;
        device->CreateRasterizerState(&desc, &m_DefaultState);

        // シェーダー
        m_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitWirePS.hlsl");
    }

    // -------------------------
    // OBB 初期化
    // -------------------------
    void InitOBB() {
        std::vector<VERTEX_3D> vertices(24);
        float hx = 1.0f, hy = 1.0f, hz = 1.0f;//ここが0.5fだったので、表示が小さくなっていた

        // 上面
        vertices[0].position = { -hx, hy, hz };
        vertices[1].position = { hx, hy, hz };
        vertices[2].position = { -hx, hy, -hz };
        vertices[3].position = { hx, hy, -hz };
        // 背面
        vertices[4].position = { -hx, hy, -hz };
        vertices[5].position = { hx, hy, -hz };
        vertices[6].position = { -hx, -hy, -hz };
        vertices[7].position = { hx, -hy, -hz };
        // 左面
        vertices[8].position = { -hx, -hy, hz };
        vertices[9].position = { -hx, hy, hz };
        vertices[10].position = { -hx, -hy, -hz };
        vertices[11].position = { -hx, hy, -hz };
        // 前面
        vertices[12].position = { hx, -hy, hz };
        vertices[13].position = { hx, hy, hz };
        vertices[14].position = { -hx, -hy, hz };
        vertices[15].position = { -hx, hy, hz };
        // 右面
        vertices[16].position = { hx, -hy, -hz };
        vertices[17].position = { hx, hy, -hz };
        vertices[18].position = { hx, -hy, hz };
        vertices[19].position = { hx, hy, hz };
        // 底面
        vertices[20].position = { hx, -hy, hz };
        vertices[21].position = { -hx, -hy, hz };
        vertices[22].position = { hx, -hy, -hz };
        vertices[23].position = { -hx, -hy, -hz };

        for (auto& v : vertices) {
            v.color = DirectX::SimpleMath::Color(1, 1, 1, 1);
            v.uv = { 0,0 };
            v.normal = { 0,1,0 };
        }

        m_OBBVertexBuffer.Create(vertices);

        std::vector<unsigned int> indices = {
            0,1,2, 1,3,2,
            4,5,6, 5,7,6,
            8,9,10, 9,11,10,
            12,13,14, 13,15,14,
            16,17,18, 17,19,18,
            20,21,22, 21,23,22
        };
        m_OBBIndexBuffer.Create(indices);
    }

    // -------------------------
    // Sphere 初期化
    // -------------------------
    void InitSphere() {
        const int slices = 16;
        const int stacks = 16;

        std::vector<VERTEX_3D> vertices;
        std::vector<unsigned int> indices;

        for (int i = 0; i <= stacks; ++i) {
            float phi = DirectX::XM_PI * i / stacks;
            for (int j = 0; j <= slices; ++j) {
                float theta = DirectX::XM_2PI * j / slices;
                VERTEX_3D v{};
                v.position.x = sinf(phi) * cosf(theta);
                v.position.y = cosf(phi);
                v.position.z = sinf(phi) * sinf(theta);
                v.color = DirectX::SimpleMath::Color(1, 1, 1, 1);
                v.normal = v.position;
                v.uv = { 0,0 };
                vertices.push_back(v);
            }
        }

        for (int i = 0; i < stacks; ++i) {
            for (int j = 0; j < slices; ++j) {
                int first = i * (slices + 1) + j;
                int second = first + slices + 1;

                indices.push_back(first);
                indices.push_back(second);
                indices.push_back(first + 1);

                indices.push_back(second);
                indices.push_back(second + 1);
                indices.push_back(first + 1);
            }
        }

        m_SphereVertexBuffer.Create(vertices);
        m_SphereIndexBuffer.Create(indices);
    }

    // -------------------------
    // Variant対応描画
    // -------------------------
    void Draw(const Collision::ColliderVariant& collider) {
        std::visit([&](auto&& col) {
            using T = std::decay_t<decltype(col)>;
            if constexpr (std::is_same_v<T, Collision::OBB>) {
                DrawOBB(col);
            }
            else if constexpr (std::is_same_v<T, Collision::Sphere>) {
                DrawSphere(col);
            }
            }, collider);
    }

    void DrawOBB(const Collision::OBB& obb) {
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

        ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();
        ctx->RSSetState(m_WireFrameState);
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_Shader.SetGPU();
        m_OBBVertexBuffer.SetGPU();
        m_OBBIndexBuffer.SetGPU();

        ctx->DrawIndexed(36, 0, 0);
        ctx->RSSetState(m_DefaultState);
    }

    void DrawSphere(const Collision::Sphere& sphere) {
        using namespace DirectX::SimpleMath;

        Matrix s = Matrix::CreateScale(sphere.radius);
        Matrix t = Matrix::CreateTranslation(sphere.center);
        Matrix world = s * t;
        Renderer::SetWorldMatrix(&world);

        ID3D11DeviceContext* ctx = Renderer::GetDeviceContext();
        ctx->RSSetState(m_WireFrameState);
        ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        m_Shader.SetGPU();
        m_SphereVertexBuffer.SetGPU();
        m_SphereIndexBuffer.SetGPU();

        ctx->DrawIndexed(1088, 0, 0);
        ctx->RSSetState(m_DefaultState);
    }
};


