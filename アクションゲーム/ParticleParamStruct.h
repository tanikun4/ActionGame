#pragma once
#include "MathCommon.h"
struct ParticleParam2D
{
    DirectX::SimpleMath::Vector3 pos;
    DirectX::SimpleMath::Vector3 velocity;
    DirectX::SimpleMath::Vector2 scale;
    float   rot;
    float   life;
    float   maxLife;
    Int2    uv;
	unsigned int color;
	int textureID;
};

struct ParticleEmitterParam2D
{
    // 発生
    Vector2 emitPos;
    Vector2 emitRange;        // ±範囲（ランダム幅）
    int     emitCount;        // 1回の発生数
    float   emitInterval;     // 発生間隔（frame or 秒）
    bool    loop;             // 永続発生

    // 初期速度
    Vector2 velocityMin;
    Vector2 velocityMax;

    // 寿命
    float   lifeMin;
    float   lifeMax;

    // 見た目
    float   scaleMin;
    float   scaleMax;
    uint32_t color;

    // テクスチャ
    int textureID; // Renderer 側で引く
};
