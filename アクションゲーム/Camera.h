#pragma once

#include	<SimpleMath.h>
#include	"input.h"
#include	"Vibration.h"

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

	DirectX::SimpleMath::Vector2 m_CameraDirection = DirectX::SimpleMath::Vector2(0, 0); //カメラの方向
	const float pi = DirectX::XM_PI;
	Object* m_TargetObject = nullptr; //注視点オブジェクト
	
	bool cameraInputFg = false; //カメラ操作入力有効フラグ

	bool CameraInput(); //カメラ操作入力処理、 trueで操作あり
	
	void DebugCameraStatus();

	Vibration m_vib; //カメラ振動用クラス


public:

	void Init();
	void Update();
	void Draw();
	void Uninit();

	void SetCamera(int mode); // カメラを設定
	void SetDirection(DirectX::SimpleMath::Vector2 dir) { m_CameraDirection = dir; } // カメラの方向を設定
	void SetTarget(Object& ob) { m_TargetObject = &ob; } // 注視点を設定
	void SetInputFg(bool _inputFg) { cameraInputFg = _inputFg; } //カメラ操作入力有効フラグ設定
	void SetPosition(DirectX::SimpleMath::Vector3 _pos) { m_Position = _pos; } // カメラ位置を設定

	void ResetTarget() { m_TargetObject = nullptr; } // 注視点をリセット

	void StartVibration(float amplitude, float frequency, float duration = -1.0f) {
		if (cameraInputFg) m_vib.Start(amplitude, frequency, duration); // 操作有効時のみ
	} // カメラ振動開始

	// View行列を取得する関数
	DirectX::SimpleMath::Matrix GetViewMatrix();

	DirectX::SimpleMath::Vector2 GetCameraDirection() { return m_CameraDirection; }

	DirectX::SimpleMath::Vector3 GetForwardVector(); // カメラの前方向ベクトルを取得
	DirectX::SimpleMath::Vector3 GetRightVector(); // カメラの右方向ベクトルを取得
	DirectX::SimpleMath::Vector3 GetUpVector(); // カメラの上方向ベクトルを取得

	DirectX::SimpleMath::Vector3 GetPosition() { return m_Position; }

};