#pragma once
#include "Object.h"
#include "LoadedEffectData.h"

//エフェクト基底クラス
class EffectBase : public Object
{
protected:
	int m_lifeflame = 0;//エフェクトの現在生存フレーム
	int m_maxlife = 0;  //エフェクトの最大生存フレーム
	bool m_scalechange = false;//スケール変更するかどうかのフラグ

	DirectX::SimpleMath::Vector3 scale_changerate = { 0, 0, 0};//スケール変化率
public:
	EffectBase();
	EffectBase(Camera* cam);
	~EffectBase();
    virtual void Update() override = 0;
    virtual void Draw() override = 0;

	//エフェクト用の初期化(継承必須)
	virtual void Init(LoadedEffectData& data, int _maxlife, DirectX::SimpleMath::Vector3 ta_scale = { -1,-1,-1 }) = 0;
	void BaseInit(int _maxlife, DirectX::SimpleMath::Vector3 ta_scale = { -1,-1,-1 });

	void ResetLife() { m_lifeflame = 0; m_live = true; }
	void SetLifeFlame(float flame) { m_lifeflame = flame; }


	/*void PlayEffect(
		int _id, DirectX::SimpleMath::Vector3 _pos,
		DirectX::SimpleMath::Vector3 _rot,
		DirectX::SimpleMath::Vector3 _scale,
		int _maxlife = 60,
		DirectX::SimpleMath::Vector3 _ta_scale = { -1, -1, -1 });*/
};

