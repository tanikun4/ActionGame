#pragma once

#include <vector>
#include "MathCommon.h"
#include "ParticleStruct.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"


struct LoadedEffectData;
class ParticleEmitter2D
{
public:
    ParticleEmitter2D();
    ~ParticleEmitter2D();
    
    void Init(const ParticleEmitterParam2D& param, 
              const LoadedEffectData& data);
    void Update();
	void Uninit();

    void Emit();
    void Play();   // 発生開始
    void Stop();   // 永続停止

    bool GetLive() { return m_live; };

    std::vector<ParticleParam2D>& GetParticles() { return m_particles; };

    ParticleDrawData2D GetDrawData();

private:
    ParticleEmitterParam2D m_param;

    std::vector<ParticleParam2D> m_particles;

	Shader* m_Shader; // シェーダオブジェクト
    Texture* m_Texture; // テクスチャ
    std::unique_ptr<Material> m_Materiale; //マテリアル

    int m_SplitX = 1;
    int m_SplitY = 1;

    int m_timer = 0;
    bool  m_live = false;
	bool  m_UI = false; // UI用かどうか
};
