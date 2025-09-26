#include "Bullet.h"
#include "Game.h"
#include "Pole.h"

using namespace DirectX::SimpleMath;

// コンストラクタ
Bullet::Bullet(Camera* cam) : Weapon(cam)
{

}

Bullet::Bullet() {

}

// デストラクタ
Bullet::~Bullet()
{

}

void Bullet::Init() {
	m_live = false;
	// 頂点データ
	std::vector<VERTEX_3D>	vertices;

	vertices.resize(4);

	vertices[0].position = Vector3(-10, 10, 0);
	vertices[1].position = Vector3(10, 10, 0);
	vertices[2].position = Vector3(-10, -10, 0);
	vertices[3].position = Vector3(10, -10, 0);

	vertices[0].color = Color(1, 1, 1, 1);
	vertices[1].color = Color(1, 1, 1, 1);
	vertices[2].color = Color(1, 1, 1, 1);
	vertices[3].color = Color(1, 1, 1, 1);

	vertices[0].uv = Vector2(0, 0);
	vertices[1].uv = Vector2(1, 0);
	vertices[2].uv = Vector2(0, 1);
	vertices[3].uv = Vector2(1, 1);

	// 頂点バッファ生成
	m_VertexBuffer.Create(vertices);

	// インデックスバッファ生成
	std::vector<unsigned int> indices;
	indices.resize(4);

	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;

	// インデックスバッファ生成
	m_IndexBuffer.Create(indices);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/BillBoardTextureVS.hlsl", "shader/unlitTexturePS.hlsl");

	//m_Textures.emplace_back(std::make_unique<Texture>());
	bool sts = m_texture.Load("assets/texture/shot.png");
	assert(sts);

	//モデルによってスケールを調整
	m_Scale = Vector3::One;
}

void Bullet::Update()
{
	// 未発射状態なら
	if (m_State == 0) {
		id = 0;
		return;
	}
	// 非表示ならreturn
	//if (ballpt.size() > 0)
	//{
	//	// 矢印の位置を更新
	//	m_Position = ballpt[0]->GetPosition();
	//}

	// 表示状態なら
	if (m_State == 1)
	{
		m_live = true;
		m_Scale.x = power * 0.006 + 1;
		m_Scale.y = power * 0.006 + 1;
		m_Scale.z = power * 0.006 + 1; // 長さをため時間に応じたものにする

	}
	// 発射状態なら
	else if (m_State == 2)
	{
		//現在の前向きベクトル

		//まずは回転行列を作成する
		Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);

		// 次に初期前向きベクトルを作る
		Vector3 initForward = { 0.0f,0.0f,1.0f };

		//初期前向きベクトル*回転行列
		m_ForwardVector = Vector3::Transform(initForward, rotationMatrix);

		// 現在の座標を計算
		m_Position += m_ForwardVector * m_Velocity_f;

		flamecount++;
		if (flamecount > Bullettime) {
			flamecount = 0;
			m_State = 0;
			power = 0;
			m_live = false;
		}
		sphere = { m_Position,m_Scale.z * 5 };
	}
}

void Bullet::Draw()
{
	if (m_live == false) return;

	Renderer::SetDepthEnable(false); // 深度書き込みを無効にする

	// カメラの設定を指定
	m_Camera->SetCamera(0);

	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	// ビルボードの処理を追加
	// 常にカメラの方を向くように回転行列を変更する
	Matrix view = m_Camera->GetViewMatrix();
	// カメラから取得したビュー行列の逆行列を作る
	r = view.Invert();
	// ビュー変換行列にはカメラの位置情報が含まれるため
	// それを消す必要がある。
	r._41 = 0.0f;
	r._42 = 0.0f;
	r._43 = 0.0f;

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	m_Shader.SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();
	m_texture.SetGPU();

	devicecontext->DrawIndexed(
		4,							// 描画するインデックス数（四角形なんで４）
		0,							// 最初のインデックスバッファの位置
		0);

	Renderer::SetDepthEnable(true); // 深度書き込みを戻す
}

void Bullet::Uninit()
{
}

//状態の設定
void Bullet::SetState(int s)
{
	m_State = s;
}

// 矢印のベクトルを取得
Vector3 Bullet::GetVector()
{
	//矢印の初期状態の向き
	Vector3 res = { 0, 0, -1 };

	// ベクトルを回転
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	res = Vector3::Transform(res, r);

	//矢印の長さ(パワー)を掛ける
	res *= m_Scale.z;

	return res;
}

void Bullet::PCharge(float rotation_y, float radius, Vector3 position) {
	pl = true;
	m_State = 1;
	m_Rotation.y = rotation_y/* + PI*/;
	m_Position = { position.x/* + sin(rotation_y) * radius * m_Scale.z*/, position.y + radius * 1.5f,  position.z/* + cos(rotation_y) * radius * m_Scale.z */ };
	if (power < maxpower) {
		power++;
	}
	else if (power > maxpower) {
		power = maxpower;
	}
}

void Bullet::PShot(float rotation_y, float radius, Vector3 position) {
	m_State = 2;
	m_Position = { position.x + sin(rotation_y) * radius * m_Scale.z, position.y,  position.z + cos(rotation_y) * radius * m_Scale.z };
	m_Velocity_f = power * 0.03;
	if (power == maxpower) {
		atk = 5;
	}
	else if (power < maxpower / 2) {
		atk = 1;
	}
	else if (power >= maxpower / 2) {
		atk = 2;
	}
}

void Bullet::EPose(float rotation_y, float radius, Vector3 position) {
	pl = false;
	m_State = 1;
	power = maxpower / 2;
	m_Rotation.y = rotation_y + PI;
	m_Position = { position.x + sin(rotation_y) * radius, position.y,  position.z + cos(rotation_y) * radius };
}

void Bullet::EShot(float rotation_y, float radius, Vector3 position) {
	pl = false;
	m_State = 2;
	m_Position = { position.x + sin(rotation_y) * radius * m_Scale.z, position.y,  position.z + cos(rotation_y) * radius * m_Scale.z };
	m_Velocity_f = power * 0.01;
	atk = 1;
}

void Bullet::LookAtShot(Vector3 position, Vector3 target, bool _pl) {
	m_live = true;
	m_Scale = { 1,1,1 };
	pl = _pl;
	m_State = 2;
	m_Position = position;
	// atan2を使用して角度を求める
	m_Rotation.y = atan2f((target.x - m_Position.x), (target.z - m_Position.z));
	m_Velocity_f = maxpower * 0.005;
	atk = 1;
}

void Bullet::OnHit(Pole* po) {
	if (m_State != 2) return;
	if (po->GetPl() && !pl) {
		atk = 5;
		m_Rotation.y = po->GetBaseRotation().y - PI / 2;
		flamecount = 0;
		m_Velocity_f *= 2;
		pl = true;
	}
}

void Bullet::SetId(int _id) {
	id = _id;
}

int Bullet::GetState() {
	return m_State;
}

int Bullet::GetId() {
	return id;
}

int Bullet::GetAtk() {
	return atk;
}

Vector3 Bullet::GetForwardVector() { return m_ForwardVector; }

Collision::ColliderVariant Bullet::GetCollision() {
	return sphere;
}