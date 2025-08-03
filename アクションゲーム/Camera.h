#pragma once

#include	<SimpleMath.h>
#include	"input.h"

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

	float m_CameraDirection = 0; //カメラの方向
	float pi = DirectX::XM_PI;
public:

	void Init();
	void Update();
	void Draw();
	void Uninit();

	void SetCamera(int mode); // カメラを設定
};