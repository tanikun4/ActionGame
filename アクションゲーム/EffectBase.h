#pragma once
#include "Object.h"

//エフェクト基底クラス
class EffectBase : public Object
{
protected:
	float m_lifeflame = 0;//エフェクトの現在フレーム
	float m_maxlife = 180;//エフェクトの最大生存フレーム
	bool m_scalechange = false;//スケール変更するかどうかのフラグ

	DirectX::SimpleMath::Vector3 scale_changerate = DirectX::SimpleMath::Vector3::One;//スケール変化率
public:
	EffectBase();
	EffectBase(Camera* cam);
	~EffectBase();
    virtual void Update() override = 0;
    virtual void Draw() override = 0;

	void SetMaxLife(float life) { m_maxlife = life; }
	void ResetLife() { m_lifeflame = 0; m_live = true; }
	void SetLifeFlame(float flame) { m_lifeflame = flame; }
	void PlayEffect(
		int _id, DirectX::SimpleMath::Vector3 _pos,
		DirectX::SimpleMath::Vector3 _rot,
		DirectX::SimpleMath::Vector3 _scale,
		int _maxlife = 60,
		DirectX::SimpleMath::Vector3 _ta_scale = { -1, -1, -1 });
};

