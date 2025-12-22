#include "ParticleEmitter2D.h"
#include "RandomCommon.h"

using namespace DirectX::SimpleMath;


void ParticleEmitter2D::Init(const ParticleEmitterParam2D& param)
{
    m_param = param;
    m_timer = 0.0f;
    m_active = false;
}

void ParticleEmitter2D::Emit(std::vector<ParticleParam2D>& particles) 
{
    /*for (int i = 0; i < m_EmitCount; ++i)
    {
        ParticleParam2D p{};
        p.pos = m_Position;

        p.velocity = {
			RandomRange(1,3),
			RandomRange(1,3),
        };

        p.rot = 0.0f;
        p.scale = Vector2(10.0f,10.0f);
        p.maxLife = 60.0f;
        p.life = p.maxLife;
        p.color = 0xffffffff;

        particles.emplace_back(p);
    }*/
}

void ParticleEmitter2D::Emit()
{
    for (int i = 0; i < m_param.emitCount; ++i)
    {
        ParticleParam2D p;

        // 発生位置（±ランダム幅）
        p.pos = m_param.emitPos + (
            Vector3{ 
                RandomRange(-m_param.emitRange.x, m_param.emitRange.x),
            RandomRange(-m_param.emitRange.y, m_param.emitRange.y),
            RandomRange(-m_param.emitRange.z, m_param.emitRange.z)
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

        // 見た目（必要なら追加）
        p.scale.x = RandomRange(m_param.scaleMin, m_param.scaleMax);
        p.scale.y = p.scale.x;
        p.color = m_param.color;

        m_particles.push_back(p);
    }
}

void ParticleEmitter2D::Update(float dt)
{
    if (m_active)
    {
        if (m_param.loop)
        {
            m_timer += dt;
            if (m_timer >= m_param.emitInterval)
            {
                Emit();
                m_timer = 0.0f;
            }
        }
        else
        {
            // OneShot
            Emit();
            m_active = false;
        }
    }

    // パーティクル更新
    for (auto& p : m_particles)
    {
        p.life -= dt;
        p.pos += p.velocity * dt;
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

void ParticleEmitter2D::Play()
{
    m_active = true;
    m_timer = m_param.emitInterval; // 即発生させたい場合
}

void ParticleEmitter2D::Stop()
{
    m_active = false;
}
