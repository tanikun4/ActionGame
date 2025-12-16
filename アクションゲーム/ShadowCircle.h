#pragma once
#include "Texture2D.h"

class ShadowCircle : public Texture2D
{
public:
    ShadowCircle(Camera* cam);
    ~ShadowCircle();

    void Init() override;
    void Update() override;
    void Draw() override;

    // 影の更新（毎フレーム呼ぶ）
    void UpdateShadow(
        const DirectX::SimpleMath::Vector3& ownerPos,
        float groundY
    );

    // 設定
    void SetBaseScale(float s) { m_BaseScale = s; }
    void SetMaxHeight(float h) { m_MaxHeight = h; }

private:
    float m_BaseScale = 1.0f;
    float m_MaxHeight = 5.0f;   // この高さで最小になる
};

