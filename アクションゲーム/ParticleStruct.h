#pragma once
#include "MathCommon.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
struct ParticleParam2D
{
    DirectX::SimpleMath::Vector3 pos;
    DirectX::SimpleMath::Vector3 velocity;
    DirectX::SimpleMath::Vector2 scale;
    float   rot;
    int   life;
    int   maxLife;
    Int2    uv = Int2(1,1);
    Int2    maxuv = Int2(1, 1);
	unsigned int color;

    int animframe = 0;
    int maxanimframe = 10;
};

struct ParticleEmitterParam2D
{
    // 発生
    DirectX::SimpleMath::Vector3 pos = DirectX::SimpleMath::Vector3(0,0,0);
    DirectX::SimpleMath::Vector3 pos_range = DirectX::SimpleMath::Vector3(0, 0, 0);        // ±範囲（ランダム幅）
    
    float rotMin = 0;
    float rotMax = 0;
    
    int     count = 10;        // 1回の発生数
    int   interval = 60;     // 発生間隔（frame）
    bool    loop = false;             // 永続発生

    // 初期速度
    DirectX::SimpleMath::Vector3 velocityMin = DirectX::SimpleMath::Vector3(0, 0, 0);
    DirectX::SimpleMath::Vector3 velocityMax = DirectX::SimpleMath::Vector3(0, 0, 0);

    // 寿命
    int   lifeMin = 1;
    int   lifeMax = 1;

    // 見た目
    float   scaleMin = 1;
    float   scaleMax = 1;
    uint32_t color = 0;

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
