#include "EffectBase.h"

using namespace std;
using namespace DirectX::SimpleMath;

//エフェクト再生関数、引数で位置、回転、スケール、最大生存フレーム、最終スケールを指定
void EffectBase::PlayEffect(int _id,Vector3 _pos, Vector3 _rot, Vector3 _scale, int _maxlife, Vector3 _ta_scale) {
	m_Position = _pos;
	m_Rotation = _rot;
	m_Scale = _scale;
	m_maxlife = _maxlife;
	m_lifeflame = 0;
	m_live = true;
	if (_ta_scale == Vector3{ -1, -1, -1 }) {
		m_scalechange = false;
	}
	else {
		m_scalechange = true;
		if (_maxlife <= 0) _maxlife = 1;//ゼロ除算防止
		scale_changerate = _ta_scale - _scale / _maxlife;//1フレームあたりのスケール変化率を計算
	}
}