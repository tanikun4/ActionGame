#include "Renderer.h"
#include "Camera.h"
#include "Application.h"
#include "Game.h"
#include "Object.h"

using namespace DirectX::SimpleMath;
extern Input input;//externは、元の変数が宣言されているファイルをインクルードしなくても、コンパイル時に自動で読み取ってくれる

using namespace std;

//=======================================
//初期化処理
//=======================================
void Camera::Init()
{
	m_Position = Vector3(50.0f, 20.0f, 50.0f);
	m_Target = Vector3(0.0f, 0.0f, 0.0f);
}


//=======================================
//更新処理
//=======================================
void Camera::Update()
{
	//上下左右キーでカメラ回転
	if (Input::GetKeyPress(VK_LEFT)) {
		m_CameraDirection.x += 0.02;
	}
	if (Input::GetKeyPress(VK_RIGHT)) {
		m_CameraDirection.x -= 0.02;
	}
	if (Input::GetKeyPress(VK_UP)) {
		m_CameraDirection.y -= 0.02;
	}
	if (Input::GetKeyPress(VK_DOWN)) {
		m_CameraDirection.y += 0.02;
	}

	//ターゲットの位置を取得
	if (m_TargetObject) {
		Vector3 pPos = m_TargetObject->GetPosition();

		//カメラの位置を更新
		m_Position.x = pPos.x + sin(m_CameraDirection.x) * 50;
		m_Position.y = pPos.y + 40 + sin(m_CameraDirection.y) * 40;
		m_Position.z = pPos.z + cos(m_CameraDirection.x) * 50;

		//カメラの注視点を更新
		m_Target = pPos;
	}

	//カメラの角度制限
	if (m_CameraDirection.x >= pi * 2 || m_CameraDirection.x <= -pi * 2) m_CameraDirection.x = 0.0f;
	if (m_CameraDirection.y >= pi * 0.5f) m_CameraDirection.y = pi * 0.5f;
	else if (m_CameraDirection.y <= -pi * 0.5f) m_CameraDirection.y = -pi * 0.5f;

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
		m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_Target, up); // 左手系にした　20230511 by suzuki.tomoki
		// DIRECTXTKのメソッドは右手系　20230511 by suzuki.tomoki
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
		projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, aspectRatio, nearPlane, farPlane);	// 左手系にした　20230511 by suzuki.tomoki
		// DIRECTXTKのメソッドは右手系　20230511 by suzuki.tomoki
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
