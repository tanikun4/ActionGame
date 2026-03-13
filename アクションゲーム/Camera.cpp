#include "Renderer.h"
#include "Camera.h"
#include "Application.h"
#include "Game.h"
#include "Object.h"
#include "DebugUI.h"
#include "WallManager.h"

using namespace DirectX::SimpleMath;
extern Input input;//externは、元の変数が宣言されているファイルをインクルードしなくても、コンパイル時に自動で読み取ってくれる

using namespace std;

void Camera::DebugCameraStatus() {
	ImGui::Begin("CameraStatus");

	Vector3 forward = GetForwardVector();

	ImGui::SliderFloat2("Direction",&m_CameraDirection.x,-PI,PI);

	ImGui::SliderFloat3("Position", &m_Position.x, 0, 300);

	ImGui::SliderFloat("Distance", &m_Distance, 0, 500);

	ImGui::LabelText("Forward", "(%.2f, %.2f, %.2f)", forward.x, forward.y, forward.z);

	ImGui::End();
}

//=======================================
//初期化処理
//=======================================
void Camera::Init()
{
	//m_Position = Vector3(50.0f, 20.0f, 50.0f);
	m_Position = Vector3(0.0f, 0.0f, 75.0f);
	m_Target = Vector3(0.0f, 0.0f, 0.0f);

	// デバッグ関数の登録
	DebugUI::RedistDebugFunction([this]() {
		DebugCameraStatus();
		});
}


//=======================================
//更新処理
//=======================================
void Camera::Update()
{
	CameraInput();

	//ターゲットの位置を取得し、距離をとる
	if (m_TargetObject) {

		// ターゲットから一定距離後方にカメラを置く
		
		Vector3 offset = GetForwardVector() * m_Distance;

		Vector3 pPos = m_TargetObject->GetPosition();

		m_Position = pPos - offset;

		// カメラをターゲットに向ける
		m_Target = pPos;
	}

	// カメラの振動更新
	if(m_vib.IsActive()) {
		m_Position += m_vib.Update();
	}

	Vector3 start = m_Target;   // プレイヤー
	Vector3 end = m_Position; // カメラ

	Vector3 dir = end - start;
	float length = dir.Length();

	dir.Normalize();

	//カメラの角度制限
	if (m_CameraDirection.x >= pi * 2 || m_CameraDirection.x <= -pi * 2) m_CameraDirection.x = 0.0f;
	if (m_CameraDirection.y >= -pi * 0.55f) m_CameraDirection.y = -pi * 0.55f;
	else if (m_CameraDirection.y <= -pi) m_CameraDirection.y = -pi;

	if (cameraInputFg)
	{
		for (auto& wall : WallManager::GetInstance().GetWalls())
		{
			if (Collision::CheckHitRay(start, dir, length, wall->GetOBB()))
			{
				wall->SetInvisible(true);
			}
			else
			{
				wall->SetInvisible(false);
			}
		}
	}

}

//=======================================
//描画処理
//=======================================
void Camera::Draw()
{
	
}


//=======================================
//終了処理
//=======================================
void Camera::Uninit()
{

}

//=======================================
//プロジェクション行列を指定
//=======================================
void Camera::SetCamera(int mode)
{
	// 3D
	if (mode == 0) {
		// ビュー変換後列作成
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
		m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_Target, up); // 左手系にした
		// DIRECTXTKのメソッドは右手系
		// 右手系にすると３角形頂点が反時計回りになるので描画されなくなるので注意
		// このコードは確認テストのために残す
		// m_ViewMatrix = m_ViewMatrix.CreateLookAt(m_Position, m_Target, up);					

		Renderer::SetViewMatrix(&m_ViewMatrix);

		//プロジェクション行列の生成
		constexpr float fieldOfView = DirectX::XMConvertToRadians(45.0f);    // 視野角

		float aspectRatio = static_cast<float>(Application::GetWidth()) / static_cast<float>(Application::GetHeight());	// アスペクト比	
		float nearPlane = 1.0f;       // ニアクリップ
		float farPlane = 1000.0f;      // ファークリップ

		//プロジェクション行列の生成
		Matrix projectionMatrix;
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane);	// 左手系にした
		// DIRECTXTKのメソッドは右手系
		// 右手系にすると３角形頂点が反時計回りになるので描画されなくなるので注意
		// このコードは確認テストのために残す
		// projectionMatrix = DirectX::SimpleMath::Matrix::CreatePerspectiveFieldOfView(fieldOfView, aspectRatio, nearPlane, farPlane);

		Renderer::SetProjectionMatrix(&projectionMatrix);
	}
	else if (mode == 1) // 2D
	{
		// ビュー変換行列作成
		Vector3 pos = { 0.0f , 0.0f, -10.0f };
		Vector3 tgt = { 0.0f , 0.0f, 1.0f };
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
		m_ViewMatrix = DirectX::XMMatrixLookAtLH(pos, tgt, up);
		Renderer::SetViewMatrix(&m_ViewMatrix);

		// プロジェクション行列の生成
		float nearPlane = 1.0f; // ニアクリップ
		float farPlane = 1000.0f; // ファークリップ
		Matrix projectionMatrix = DirectX::XMMatrixOrthographicLH(
			static_cast<float>(Application::GetWidth()),
			static_cast<float>(Application::GetHeight()),
			nearPlane, farPlane);
		projectionMatrix = DirectX::XMMatrixTranspose(projectionMatrix);
		Renderer::SetProjectionMatrix(&projectionMatrix);
	}
}

Matrix Camera::GetViewMatrix()
{
	return m_ViewMatrix;
}

DirectX::SimpleMath::Vector3 Camera::GetForwardVector() {
	DirectX::SimpleMath::Vector3 forward;
	forward.x = sinf(m_CameraDirection.x) * cosf(m_CameraDirection.y);
	forward.y = sinf(m_CameraDirection.y);
	forward.z = cosf(m_CameraDirection.x) * cosf(m_CameraDirection.y);
	forward.Normalize();
	return forward;
}

Vector3 Camera::GetRightVector() {
	Vector3 forward = GetForwardVector();
	Vector3 worldUp(0, 1, 0);

	Vector3 right = forward.Cross(worldUp);
	right.Normalize();
	return right;
}

Vector3 Camera::GetUpVector() {
	Vector3 forward = GetForwardVector();
	Vector3 right = GetRightVector();

	Vector3 up = right.Cross(forward);
	up.Normalize();
	return up;
}

bool Camera::CameraInput() {

	if (!cameraInputFg) return false;

	//入力フラグ
	bool inputFg = false;

	// 入力取得
	Vector2 inputvector = ActionInput::GetInstance().GetCameraVector();

	const float sensitivity = 0.02f; // 感度

	// 入力がある場合、カメラの方向を更新
	if (inputvector.Length() > 0.01f) {
		m_CameraDirection.x -= inputvector.x * sensitivity;
		m_CameraDirection.y -= inputvector.y * sensitivity;
		inputFg = true;
	}

	return inputFg;
}