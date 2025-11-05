#pragma once
#include <SimpleMath.h>

struct CollisionResult
{
    bool hit = false;           // 衝突したかどうか
    DirectX::SimpleMath::Vector3 normal = { 0, 0, 0 }; // 法線ベクトル（押し戻し方向）
    DirectX::SimpleMath::Vector3 contactPoint = { 0, 0, 0 }; // 接触点
    float penetration = 0.0f;   // めり込み量
};

