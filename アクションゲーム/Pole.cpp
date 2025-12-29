//#include <memory>
#include "Pole.h"
#include "GolfBall.h"
#include "StaticMesh.h"
#include "utility.h"
#include "Game.h"
#include "Ground.h"
#include "Collision.h"
#include "EffectManager.h"
#include "DebugUI.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
Pole::Pole(Camera* cam) :Weapon(cam)
{

}

Pole::Pole() {

}

// デストラクタ
Pole::~Pole()
{

}

//=======================================
// 初期化処理
//=======================================
void Pole::Init()
{
	// メッシュ読み込み
	StaticMesh staticmesh;

	// 3Dモデルデータ
	std::u8string modelFile = u8"assets/model/golf_pole/golf_pole_NO_HOLE.obj";

	// テクスチャディレクトリ
	std::string texDirectory = "assets/model/golf_pole";

	// Meshを読み込む
	std::string tmpStr1(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
	staticmesh.Load(tmpStr1, texDirectory);

	m_MeshRenderer.Init(staticmesh);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	// サブセット情報取得
	m_subsets = staticmesh.GetSubsets();

	// テクスチャ情報取得
	m_Textures = staticmesh.GetTextures();

	// マテリアル情報取得
	vector<MATERIAL> materials = staticmesh.GetMaterials();

	// マテリアル数分ループ
	for (int i = 0; i < materials.size(); i++)
	{
		// マテリアルオブジェクト生成
		std::unique_ptr<Material> m = std::make_unique<Material>();

		// マテリアル情報をセット
		m->Create(materials[i]);

		// マテリアルオブジェクトを配列に追加
		m_Materiales.push_back(std::move(m));
	}

	// モデルによってスケールを調整
	m_Scale.x = 3;
	m_Scale.y = 6;
	m_Scale.z = 3;

}

//=======================================
// 更新処理
//=======================================
void Pole::Update()
{

		//vector<GolfBall*> ball = Game::GetInstance()->GetObjects<GolfBall>();
		//DirectX::SimpleMath::Vector3 position = ball[0]->GetPosition();
		//float radius = ball[0]->GetRadius();
		//DirectX::SimpleMath::Vector3 rotation = ball[0]->GetRotation();
		//if (m_State == 1) {
		//	m_Rotation.y += PI / 20;
		//	swing_time++;
		//	if (swing_time > 18) {
		//		m_State = 0;
		//		swing_time = 0;
		//	}
		//}

		//if(m_State == 0){
		//	m_Rotation = { PI / 2, rotation.y + PI / 2,PI / 2 };
		//}
		////DirectX::SimpleMath::Vector3 radian = { rotation.x * (PI / 180) , rotation.y * (PI / 180) , rotation.z * (PI / 180) };//角度をラジアンに変換
		//m_Position = { position.x + sin(rotation.y) * radius, position.y,  position.z + cos(rotation.y) * radius};
		//hitbox.SetRotation(m_Rotation);
		//hitbox.SetPos({ m_Position.x + sin(m_Rotation.y - PI / 2) * radius * 1.7f, m_Position.y, m_Position.z + cos(m_Rotation.y - PI / 2) * radius * 1.7f });
		//hitbox.SetScale({ m_Scale.x * 0.1f ,m_Scale.y * 0.15f ,m_Scale.z * 0.1f });
}

//=======================================
// 更新処理2
//=======================================
void Pole::Update(Vector3 position, float radius, Vector3 rotation, float offset)//offsetはobbの距離調整用
{

	switch (m_State) {
	case NORMAL: //通常状態
		m_Rotation = { PI / 2 + angle_debug.x, rotation.y + PI / 2 + angle_debug.y, PI / 2 + angle_debug.z};
		m_baseRotation = m_Rotation;
		break;
	case SWING: //振り攻撃中
		/*m_Rotation.y += PI / 20;
		++m_swing_time;*/
		SwingUpdate();
		break;
	case GUARD: //ガード中
		m_Rotation = { PI / 2, rotation.y ,PI / 2 };
		m_baseRotation = m_Rotation;
		m_offset = { cos(m_Rotation.y) * radius * 2 , offset_debug.y, sin(m_Rotation.y) * radius * -2 };
		break;
	case STANCE: //構え中
		StanceUpdate();
		//縦構え中の暫定処理
		if (m_Rotation.x != PI / 2) {
			m_Rotation.y = rotation.y + PI / 2;
		}
		break;
	case ATTACK: //攻撃中(回転攻撃など)
		m_Rotation = { PI / 2, rotation.y + PI / 2,PI / 2 };
		break;
	case SWING_VERTICAL: //縦振り攻撃中
		m_Rotation.z -= PI / 20;
		++m_swing_time;
		break;
	}

	//DirectX::SimpleMath::Vector3 radian = { rotation.x * (PI / 180) , rotation.y * (PI / 180) , rotation.z * (PI / 180) };//角度をラジアンに変換

	m_Position = { 
		position.x + sin(rotation.y + angle_debug.y) * radius, 
		position.y ,  
		position.z + cos(rotation.y + angle_debug.y) * radius };
	m_Position += m_offset;
	//obb = { 
	//    {
	//	m_Position.x + sin(m_Rotation.y - PI / 2) * radius * offset, 
	//	m_Position.y,
	//	m_Position.z + cos(m_Rotation.y - PI / 2) * radius * offset
	//	},
	//	m_Rotation,
	//	{ m_Scale.x,m_Scale.y * 1.5f ,m_Scale.z} };

	// 回転行列とワールド行列
	Matrix S = Matrix::CreateScale(m_Scale);
	Matrix R = Matrix::CreateFromYawPitchRoll(
		m_Rotation.y,
		m_Rotation.x,
		m_Rotation.z
	);
	Matrix T = Matrix::CreateTranslation(m_Position);

	Matrix world = S * R * T;

	// OBB の中心位置を「武器の中心」に補正
	// ポールモデルの pivot は "持ち手先端"
	Vector3 obbLocalCenter = { 0.0f, m_Scale.y * 0.2f, 0.0f };

	// ワールド座標へ変換
	Vector3 obbWorldCenter = Vector3::Transform(obbLocalCenter, world);

	// OBBの更新
	obb = {
		obbWorldCenter,           
		m_Rotation,
		{ m_Scale.x, m_Scale.y * 1.5f, m_Scale.z }
	};
}

//=======================================
// 描画処理
//=======================================
void Pole::Draw()
{
	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	m_Shader.SetGPU();

	// インデックスバッファ・頂点バッファをセット
	m_MeshRenderer.BeforeDraw();

	// カメラの設定を指定
	m_Camera->SetCamera(0);

	//マテリアル数分ループ 
	for (int i = 0; i < m_subsets.size(); i++)
	{
		// マテリアルをセット(サブセット情報の中にあるマテリアルインデックスを使用)
		m_Materiales[m_subsets[i].MaterialIdx]->SetGPU();

		if (m_Materiales[m_subsets[i].MaterialIdx]->isTextureEnable())
		{
			m_Textures[m_subsets[i].MaterialIdx]->SetGPU();
		}

		m_MeshRenderer.DrawSubset(
			m_subsets[i].IndexNum, // 描画するインデックス数
			m_subsets[i].IndexBase, // 最初のインデックスバッファの位置	
			m_subsets[i].VertexBase); // 頂点バッファの最初から使用
	}
}

//=======================================
// 終了処理
//=======================================
void Pole::Uninit()
{

}

void Pole::Swing() {
	//なんかしたときに視野角
	//攻撃に当たったらカメラ揺らす
	if (m_State == NORMAL) {
		m_Rotation.y -= PI / 2;
	}
	Vector3 endrot = m_Rotation;
	endrot.y += PI;
	SwingStart(m_Rotation, endrot, 18,0.3f);
}

void Pole::Swing_Vertical() {
	//なんかしたときに視野角
	//攻撃に当たったらカメラ揺らす

	// xをPI / 2足してから、zを変化させると縦振りが可能
	/*if (m_State == NORMAL) {
		m_Rotation.x += PI / 2;
		m_Rotation.z += 1.8f;
		m_State = SWING_VERTICAL;
		m_swing_time = 0;
		atkFg = true;
	}
	else if (m_State == STANCE) {
		m_State = SWING_VERTICAL;
		m_swing_time = 0;
		m_stance_time = 0;
		atkFg = true;
	}*/

	//m_Rotation.x += PI / 2;
	//m_Rotation.z += 1.8f;

	m_Rotation.x = PI;
	m_Rotation.z = PI + 0.2f;
	Vector3 endrot = m_Rotation;
	endrot.z = PI * 0.5f;
	SwingStart(m_Rotation, endrot,10,1.0f);
}

void Pole::SwingStart(const Vector3& s, const Vector3& e, int t, float accel)
{
	m_SwingAnim.Start(s, e, t,accel);
	m_stance_time = 0;
	m_swing_time = 0;
	m_State = SWING;
	atkFg = true;
}

void Pole::SwingUpdate() {
	m_Rotation = m_SwingAnim.Update();
	++m_swing_time;
}

void Pole::SwingEnd() {
	m_State = NORMAL;
	atkFg = false;
	m_swing_time = 0;
}

void Pole::AttackStart() { //攻撃状態になるだけの関数、回転切り等で使用
	m_State = ATTACK;
	m_swing_time = 0;
	m_Rotation = m_baseRotation;
	atkFg = true;
}

void Pole::AttackEnd() { //攻撃状態終了
	m_State = NORMAL;
	atkFg = false;
}

void Pole::Stance_Vertical() {
	Vector3 endrot = m_Rotation;
	endrot.x = PI;
	endrot.z = PI + 0.2f;
	StanceStart(m_Rotation, endrot, 60);
}

void Pole::StanceStart(const Vector3& s, const Vector3& e, int t) {
	m_SwingAnim.Start(s, e, t);
	m_stance_time = 0;
	m_State = STANCE;
}

//構え開始
void Pole::StanceStart() {
	m_State = STANCE;
	m_baseRotation = m_Rotation;
	m_stance_time = 0;

	Vector3 endrot = m_Rotation;
	endrot.y -= PI / 2;
	StanceStart(m_Rotation, endrot, 18);
}

//構えてから振る
void Pole::StanceToSwing(const Vector3& s_stance, const Vector3& e_stance, int t_stance,
	const Vector3& s_swing, const Vector3& e_swing, int t_swing, int swingframe) 
{
	m_SwingAnim.Start(s_stance, e_stance, t_stance);
	m_stance_time = 0;
	m_stance_swingframe = swingframe;
	m_State = STANCE;
	m_baseRotation = m_Rotation;
}

//現在角度から振る
void Pole::ToSwing(const DirectX::SimpleMath::Vector3& e, int t, float accel)
{
	m_SwingAnim.Start(m_Rotation, e, t, accel);
	m_stance_time = 0;
	m_swing_time = 0;
	m_State = SWING;
	atkFg = true;
}

//構え中の処理
void Pole::StanceUpdate() {
	/*if (m_stance_time < 20) {
		m_Rotation.y -= (PI / 2) * 0.05f;
	}*/
	/*if( m_Rotation.y < m_baseRotation.y - (PI / 2)) {
		m_Rotation.y = m_baseRotation.y - (PI / 2);
	}*/

	m_Rotation = m_SwingAnim.Update();
	++m_stance_time;
}

void Pole::StanceEnd() {
	m_State = NORMAL;
	m_offset = { 0,0,0 };
}

int Pole::GetState() { return m_State; }

//位置の設定
void Pole::SetPosition(float x, float y, float z)
{
	Vector3 p = { x, y, z };
	SetPosition(p);
}
void Pole::SetPosition(Vector3 pos)
{
	m_Position = pos;

	// Y座標を地形に合わせて変更

	// Groundの頂点データを取得
	vector<Ground*> grounds = Game::GetInstance()->GetObjects<Ground>();
	vector<VERTEX_3D> vertices;
	for (auto& g : grounds) // Groundオブジェクトの数ループ
	{
		vector<VERTEX_3D> vecs = g->GetVertices();
		for (auto& v : vecs) // 頂点の数ループ
		{
			vertices.emplace_back(v);
		}
	}

	// 線分とポリゴンの当たり判定
	for (int i = 0; i < vertices.size(); i += 3)
	{
		// 三角形ポリゴン
		Collision::Polygon collisionPolygon = {
			vertices[i + 0].position,
			vertices[i + 1].position,
			vertices[i + 2].position };

		Vector3 cp; // 接触点

//		Vector3 start = m_Position;
//		Vector3 end = m_Position;
//		start.y -= 100;;
//		end.y += 100;
//		Collision::Segment collisionSegment = { start, end };
//		if (Collision::CheckHit(collisionSegment, collisionPolygon, cp))
//		{
//
////			m_Position.y = cp.y; // 接触点のちょっとだけ上をPoleのY座標とする
//			int a = 4;
//		}

		Collision::Line line = { m_Position , Vector3(0,1,0) };
		if (Collision::CheckHit(line, collisionPolygon, cp))
		{
			m_Position.y = cp.y; // 接触点のちょっとだけ上をPoleのY座標とする
		}


	}
}

Collision::ColliderVariant Pole::GetCollision() {
	return obb;
}

void Pole::GuardStart() {
	m_State = 2;
	m_baseRotation = m_Rotation;
}

void Pole::GuardEnd() {
	m_State = 0;
	m_offset = { 0,0,0 };
}

void Pole::TipToEffect(int _id, EffectParams _param)
{
	Matrix S = Matrix::CreateScale(m_Scale);
	Matrix R = Matrix::CreateFromYawPitchRoll(
		m_Rotation.y,
		m_Rotation.x,
		m_Rotation.z
	);
	Matrix T = Matrix::CreateTranslation(m_Position);

	Matrix world = S * R * T;

	// 武器の先端は +Y
	Vector3 tipLocal = { 0.0f, m_Scale.y * 0.5f, 0.0f };
	Vector3 tipWorld = Vector3::Transform(tipLocal, world);

	// 土煙エフェクトは中心を基準に表示されるので補正する
	Vector3 downOffset = _param.scale * 0.5f * m_Camera->GetForwardVector();
	_param.pos = tipWorld + Vector3(downOffset.x, 0, downOffset.z);

	EffectManager::GetInstance()->Play(_id, _param);
}



void Pole::DebugPoleStatus() {
	Matrix S = Matrix::CreateScale(m_Scale);
	Matrix R = Matrix::CreateFromYawPitchRoll(
		m_Rotation.y,
		m_Rotation.x,
		m_Rotation.z
	);
	Matrix T = Matrix::CreateTranslation(m_Position);

	Matrix world = S * R * T;

	// world = S * R * T; がある前提

	Vector3 xAxis = Vector3::TransformNormal(Vector3(1, 0, 0), world);
	Vector3 yAxis = Vector3::TransformNormal(Vector3(0, 1, 0), world);
	Vector3 zAxis = Vector3::TransformNormal(Vector3(0, 0, 1), world);

	ImGui::Begin("Pole Axis Debug");

	ImGui::Text("X Axis (Right):   %.2f %.2f %.2f", xAxis.x, xAxis.y, xAxis.z);
	ImGui::Text("Y Axis (Up/Tip?): %.2f %.2f %.2f", yAxis.x, yAxis.y, yAxis.z);
	ImGui::Text("Z Axis (Forward): %.2f %.2f %.2f", zAxis.x, zAxis.y, zAxis.z);

	ImGui::End();

}
