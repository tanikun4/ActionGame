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

void EffectBase::BaseInit(int _maxlife,Vector3 ta_scale) {
	m_lifeflame = 0;
	m_maxlife = _maxlife;
	if (ta_scale != Vector3{ -1, -1, -1 }) {//スケール変化率の設定
		if (m_maxlife <= 0) m_maxlife = 1;
		scale_changerate = (ta_scale - m_Scale) / m_maxlife;
	}
	m_live = true;
}