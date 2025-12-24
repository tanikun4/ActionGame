#include "ParticleEmitter2D.h"
#include "RandomCommon.h"
#include "EffectDataStruct.h"

#pragma message("ParticleEmitter2D.cpp is compiled")

using namespace std;
using namespace DirectX::SimpleMath;


ParticleEmitter2D::ParticleEmitter2D() 
{
    m_Materiale = std::make_unique<Material>();
    MATERIAL mtrl;
    mtrl.Diffuse = { 1,1,1,1 };
    mtrl.Shiness = 1;
    mtrl.TextureEnable = true; // テクスチャを使うか否かのフラグ
    m_Materiale->Create(mtrl);
    m_live = false;
}
ParticleEmitter2D::~ParticleEmitter2D() 
{

}

void ParticleEmitter2D::Init(const ParticleEmitterParam2D& param, 
                             const LoadedEffectData& data)
{
    m_param = param;
    m_timer = 0.0f;
    m_live = true;

    // シェーダオブジェクト取得
    m_Shader = data.shader.get();

    // テクスチャ情報取得(単体)
	m_Texture = data.textures.back().get();

    // UV分割数取得
    m_SplitX = data.texture_uv.x;
    m_SplitY = data.texture_uv.y;

}

void ParticleEmitter2D::Emit()
{
	m_live = true;
    for (int i = 0; i < m_param.count; ++i)
    {
        ParticleParam2D p;

        // 発生位置（±ランダム幅）
        p.pos = m_param.pos + (
            Vector3{ 
                RandomRange(-m_param.pos_range.x, m_param.pos_range.x),
            RandomRange(-m_param.pos_range.y, m_param.pos_range.y),
            RandomRange(-m_param.pos_range.z, m_param.pos_range.z)
            });

        // 初期速度
        p.velocity = { 
            RandomRange(m_param.velocityMin.x,m_param.velocityMax.x),
            RandomRange(m_param.velocityMin.y,m_param.velocityMax.y),
            RandomRange(m_param.velocityMin.z,m_param.velocityMax.z) 
        };

        // 寿命
        p.life = p.maxLife =
            RandomRange(m_param.lifeMin, m_param.lifeMax);

		p.maxuv = Int2(m_SplitX, m_SplitY);
        const int animFrameCount = m_SplitX * m_SplitY;
        p.maxanimframe = p.maxLife / animFrameCount; //アニメーション遷移フレーム数設定

        p.rot = RandomRange(m_param.rotMin, m_param.rotMax);

        // 見た目（必要なら追加）
        p.scale.x = RandomRange(m_param.scaleMin, m_param.scaleMax);
        p.scale.y = p.scale.x;
		p.uv = Int2(1, 1);
        //p.color = m_param.color;

        m_particles.emplace_back(p);
    }
}

void ParticleEmitter2D::Update()
{

    if (m_param.loop)
    {
        ++m_timer;
        if (m_timer >= m_param.interval)
        {
            Emit();
            m_timer = 0.0f;
        }
    }
    else
    {
        // OneShot
        Emit();
        m_live = false;
    }
   

    // パーティクル更新
    for (auto& p : m_particles)
    {
        --p.life;
        p.pos += p.velocity;
		// アニメーションフレーム更新
		++p.animframe;
        if(p.animframe >= p.maxanimframe)
        {
            p.animframe = 0;
            ++p.uv.x;
            if(p.uv.x > p.maxuv.x)
            {
                p.uv.x = 1;
                ++p.uv.y;
                if (p.uv.y > p.maxuv.y)
                {
                    p.uv.y = 1;
                }
			}
		}
    }

    // 死亡削除
    m_particles.erase(
        std::remove_if(
            m_particles.begin(),
            m_particles.end(),
            [](const ParticleParam2D& p)
            {
                return p.life <= 0.0f;
            }),
        m_particles.end());
}

void ParticleEmitter2D::Uninit()
{
    m_particles.clear();
}

void ParticleEmitter2D::Play()
{
    m_live = true;
    m_timer = m_param.interval; // 即発生させたい場合
}

void ParticleEmitter2D::Stop()
{
    m_live = false;
}

ParticleDrawData2D ParticleEmitter2D::GetDrawData()
{
    ParticleDrawData2D drawdata;
    drawdata.particles = &m_particles;
    drawdata.shader = m_Shader;
    drawdata.texture = m_Texture;
    drawdata.material = m_Materiale.get();
    drawdata.isUI = m_param.UI;
	drawdata.maxuv = Int2(m_SplitX, m_SplitY);
    return drawdata;
}