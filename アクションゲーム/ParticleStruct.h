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
    Int2    maxuv;
	unsigned int color;

    int animframe = 0;
    int maxanimframe = 10;
};

struct ParticleEmitterParam2D
{
    // 発生
    DirectX::SimpleMath::Vector3 emitPos;
    DirectX::SimpleMath::Vector3 emitRange;        // ±範囲（ランダム幅）
    int     emitCount;        // 1回の発生数
    float   emitInterval;     // 発生間隔（frame or 秒）
    bool    loop;             // 永続発生

    // 初期速度
    DirectX::SimpleMath::Vector3 velocityMin;
    DirectX::SimpleMath::Vector3 velocityMax;

    // 寿命
    float   lifeMin;
    float   lifeMax;

    // 見た目
    float   scaleMin;
    float   scaleMax;
    uint32_t color;

    // テクスチャ
    int textureID; // Renderer 側で引く

	bool UI = false; // UI用かどうか
};

struct ParticleDrawData2D
{
    std::vector<ParticleParam2D>* particles;

    Shader* shader;
    Texture* texture;
    Material* material;

    Int2 maxuv;
    bool isUI;
};
