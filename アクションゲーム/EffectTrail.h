#pragma once
#include <vector>
#include <deque>
#include <d3d11.h>
#include "Object.h"

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Material.h"

class EffectTrail : public Object
{
public:
    EffectTrail(Camera* cam);
    ~EffectTrail();

    void Init();
    void Start();
    void End();

	void Update();
    void AddPoint(const DirectX::SimpleMath::Vector3& base, const DirectX::SimpleMath::Vector3& tip);
    void Draw();
    void Uninit();

    void SetTexture(const char* imgname);
    void SetColor(const DirectX::SimpleMath::Vector4& color);

private:
    struct TrailPoint
    {
        DirectX::SimpleMath::Vector3 base;
        DirectX::SimpleMath::Vector3 tip;
        int  life = 0;
    };

	// メッシュを構築する
    void BuildMesh();

	// 前のポイントと新しいポイントの間に補間ポイントを追加する
    void AddInterpolatedPoint(const DirectX::SimpleMath::Vector3& prevBase, const DirectX::SimpleMath::Vector3& prevTip,
        const DirectX::SimpleMath::Vector3& newBase, const DirectX::SimpleMath::Vector3& newTip);

    void AddPointCatmullRom(const DirectX::SimpleMath::Vector3& newBase, const DirectX::SimpleMath::Vector3& newTip);

    void AddPointBezier(const DirectX::SimpleMath::Vector3& newBase, const DirectX::SimpleMath::Vector3& newTip);
    DirectX::SimpleMath::Vector3 BezierCubic(const DirectX::SimpleMath::Vector3& P0, const DirectX::SimpleMath::Vector3& P1, const  DirectX::SimpleMath::Vector3& P2, const  DirectX::SimpleMath::Vector3& P3, float t);
private:
    std::vector<TrailPoint> m_Points;

    std::vector<VERTEX_3D> m_Vertices;
    std::vector<unsigned int> m_Indices;

    VertexBuffer<VERTEX_3D>   m_VertexBuffer;
    IndexBuffer    m_IndexBuffer;
    Shader         m_Shader;
    Texture        m_Texture;
    std::unique_ptr<Material> m_Materiale;

    int   m_MaxPoint = 20;
    int m_LifeTime = 15;
    bool  m_Active = false;
	bool m_Change = false;//点が変更されたかどうか
};

