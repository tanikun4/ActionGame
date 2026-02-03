#include "EffectBase.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
EffectBase::EffectBase(Camera* cam) : Object(cam)
{

}

EffectBase::EffectBase() 
{

}

// デストラクタ
EffectBase::~EffectBase()
{

}

void EffectBase::BaseInit(int _maxlife,
	DirectX::SimpleMath::Vector3 _pos_amount, int _pos_changeframe,
	DirectX::SimpleMath::Vector3 _rot_amount, int _rot_changeframe,
	DirectX::SimpleMath::Vector3 _scale_amount, int _scale_changeframe,
	bool _endless)
{
	m_lifeframe = 0;
	m_maxlife = _maxlife;
	m_poschange_frame = _pos_changeframe;
	m_rotchange_frame = _rot_changeframe;
	m_scalechange_frame = _scale_changeframe;
	//座標変化率の設定
	int division_flame = m_maxlife - m_poschange_frame;
	if (division_flame <= 0) division_flame = 1;
	pos_changerate = _pos_amount / (float)(division_flame);
	
	//回転変化率の設定
	division_flame = m_maxlife - m_rotchange_frame;
	if (division_flame <= 0) division_flame = 1;
	rot_changerate = _rot_amount / (float)(division_flame);
	
	//スケール変化率の設定
	division_flame = m_maxlife - m_scalechange_frame;
	if (division_flame <= 0) division_flame = 1;
	scale_changerate = _scale_amount / (float)(division_flame);
	
	endless = _endless;
	m_live = true;
}

void EffectBase::BaseUpdate() {
	//座標を変化量分増加
	if (m_lifeframe >= m_poschange_frame) {
		m_Position += pos_changerate;
	}
	//回転を変化量分増加
	if (m_lifeframe >= m_rotchange_frame) {
		m_Rotation += rot_changerate;
	}
	//スケールを変化量分増加
	if (m_lifeframe >= m_scalechange_frame) {
		m_Scale += scale_changerate;
	}
	//エフェクトの生存フレームを加算
	++m_lifeframe;
	//生存フレーム限界に達したら、存在フラグをfalseにする
	if (m_lifeframe >= m_maxlife) {
		if (endless) {
			m_lifeframe = 0;
			return;
		}
		m_live = false;
	}
}