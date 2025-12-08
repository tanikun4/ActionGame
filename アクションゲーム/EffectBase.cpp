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

void EffectBase::BaseInit(int _maxlife,Vector3 ta_scale, int change_flame) {
	m_lifeflame = 0;
	m_maxlife = _maxlife;
	m_change_flame = change_flame;
	if (ta_scale != Vector3{ -1, -1, -1 }) {//スケール変化率の設定
		int division_flame = m_maxlife - m_change_flame;
		if (division_flame <= 0) division_flame = 1;
		scale_changerate = (ta_scale - m_Scale) / (division_flame);
	}
	m_live = true;
}

void EffectBase::BaseUpdate() {
	//スケールを変化量分増加
	if (m_lifeflame >= m_change_flame) {
		m_Scale += scale_changerate;
	}
	//エフェクトの生存フレームを加算
	++m_lifeflame;
	//生存フレーム限界に達したら、存在フラグをfalseにする
	if (m_lifeflame >= m_maxlife) {
		m_live = false;
	}
}