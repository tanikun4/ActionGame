#include "Object.h"

using namespace DirectX::SimpleMath;

// コンストラクタ
Object::Object(Camera* cam) : m_Camera(cam)
{

}

Object::Object() {

}

// デストラクタ
Object::~Object()
{

}

//=======================================
//座標をセット
//=======================================
void Object::SetPosition(const Vector3& pos) {
	m_Position = pos;
	return;
};


//=======================================
//角度をセット
//=======================================
void Object::SetRotation(const Vector3& rot) {
	m_Rotation = rot;
	return;
};

//=======================================
//大きさをセット
//=======================================
void Object::SetScale(const Vector3& sca) {
	m_Scale = sca;
	return;
};

//=======================================
//生存フラグをセット
//=======================================
void Object::SetLive(const bool& live) {
	m_live = live;
	return;
};

//=======================================
//座標をゲット
//=======================================
Vector3 Object::GetPosition() const
{
	return m_Position;
}

//=======================================
//角度をゲット
//=======================================
Vector3 Object::GetRotation() const
{
	return m_Rotation;
}

//=======================================
//大きさをゲット
//=======================================
Vector3 Object::GetScale() const
{
	return m_Scale;
}

//行列更新関数(見た目版)
bool Object::UpdateDirectionVectors() 
{
	// 回転が変わった場合のみ再計算
	if (m_Rotation != m_OldRotation)
	{
		//速度設定の部分はコメントアウト
		//Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
		//Vector3 initForward = { 0.0f, 0.0f, 1.0f };
		//m_ForwardVector = Vector3::Transform(initForward, rotationMatrix);
		m_OldRotation = m_Rotation;
		return true; // 更新あり
	}
	return false;// 更新なし
}

//行列更新関数(進行方向版)
bool Object::UpdateForwardDirectionVectors() {
	// 回転が変わった場合のみ再計算
	if (m_ForwardRotation != m_OldForwardRotation)
	{
		Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(m_ForwardRotation.y, m_ForwardRotation.x, m_ForwardRotation.z);
		Vector3 initForward = { 0.0f, 0.0f, 1.0f };
		m_ForwardVector = Vector3::Transform(initForward, rotationMatrix);
		m_OldForwardRotation = m_ForwardRotation;
		return true; // 更新あり
	}
	return false;// 更新なし
}
