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

bool Object::UpdateDirectionVectors() 
{
	// 回転が変わった場合のみ再計算
	if (m_Rotation != m_OldRotation)
	{
		Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
		Vector3 initForward = { 0.0f, 0.0f, 1.0f };
		m_ForwardVector = Vector3::Transform(initForward, rotationMatrix);
		m_OldRotation = m_Rotation;
		return true; // 更新あり
	}
	return false;// 更新なし
}
