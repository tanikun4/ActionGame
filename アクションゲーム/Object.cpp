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

void Object::SetPosition(const Vector3& pos) 
{
	m_Position.x = pos.x;
	m_Position.y = pos.y;
	m_Position.z = pos.z;
}

Vector3 Object::GetPosition() const
{
	return m_Position;
}

Vector3 Object::GetRotation() const
{
	return m_Rotation;
}

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
