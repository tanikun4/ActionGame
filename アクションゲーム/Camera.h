#pragma once

#include	<SimpleMath.h>
#include	"input.h"

class Object;//前方宣言
//-----------------------------------------------------------------------------
//Cameraクラス
//-----------------------------------------------------------------------------
class Camera {
private:
	DirectX::SimpleMath::Vector3	m_Position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3	m_Rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
	DirectX::SimpleMath::Vector3	m_Scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);

	DirectX::SimpleMath::Vector3	m_Target{};
	DirectX::SimpleMath::Matrix		m_ViewMatrix{};

	DirectX::SimpleMath::Vector2 m_CameraDirection = DirectX::SimpleMath::Vector2(0,0); //カメラの方向
	const float pi = DirectX::XM_PI;
	Object* m_TargetObject; //注視点オブジェクト
public:

	void Init();
	void Update();
	void Draw();
	void Uninit();

	void SetCamera(int mode); // カメラを設定
	void SetTarget(Object& ob) { m_TargetObject = &ob; } // 注視点を設定
	// View行列を取得する関数
	DirectX::SimpleMath::Matrix GetViewMatrix();
	DirectX::SimpleMath::Vector2 GetCameraDirection() { return m_CameraDirection; }
};