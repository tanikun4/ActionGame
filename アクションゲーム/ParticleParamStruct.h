#pragma once
#include "MathCommon.h"
struct ParticleParam2D
{
    DirectX::SimpleMath::Vector3 pos;
    DirectX::SimpleMath::Vector2 scale;
    float   rot;
    float   life;
    float   maxLife;
    Int2    uv;
};