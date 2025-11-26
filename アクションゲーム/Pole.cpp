//#include <memory>
#include "Pole.h"
#include "GolfBall.h"
#include "StaticMesh.h"
#include "utility.h"
#include "Game.h"
#include "Ground.h"
#include "Collision.h"


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
		m_Rotation = { PI / 2, rotation.y + PI / 2,PI / 2 };
		m_baseRotation = m_Rotation;
		break;
	case SWING: //振り攻撃中
		m_Rotation.y += PI / 20;
		++m_swing_time;
		break;
	case GUARD: //ガード中
		m_Rotation = { PI / 2, rotation.y ,PI / 2 };
		m_baseRotation = m_Rotation;
		m_offset = { cos(m_Rotation.y) * radius * 2 , offset_debug.y, sin(m_Rotation.y) * radius * -2 };
		break;
	case STANCE: //構え中
		StanceUpdate();
		break;
	}

	//DirectX::SimpleMath::Vector3 radian = { rotation.x * (PI / 180) , rotation.y * (PI / 180) , rotation.z * (PI / 180) };//角度をラジアンに変換
	m_Position = { position.x + sin(rotation.y) * radius, position.y,  position.z + cos(rotation.y) * radius };
	m_Position += m_offset;
	obb = { {m_Position.x + sin(m_Rotation.y - PI / 2) * radius * offset, m_Position.y, m_Position.z + cos(m_Rotation.y - PI / 2) * radius * offset},
		m_Rotation,
		{ m_Scale.x,m_Scale.y * 1.5f ,m_Scale.z} };
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
	if (m_State == NORMAL) {
		m_Rotation.y -= PI / 2;
		m_State = SWING;
		m_swing_time = 0;
		atkFg = true;
	}
	else if (m_State == STANCE) {
		m_State = SWING;
		m_swing_time = 0;
		m_stance_time = 0;
		atkFg = true;
	}
}

void Pole::SwingEnd() {
	m_State = NORMAL;
	atkFg = false;
}

void Pole::AttackStart() { //攻撃状態になるだけの関数、回転切り等で使用
	m_State = ATTACK;
	m_swing_time = 0;
	atkFg = true;
}

void Pole::AttackEnd() { //攻撃状態になるだけの関数、回転切り等で使用
	m_State = NORMAL;
	atkFg = false;
}

//構え開始
void Pole::StanceStart() {
	m_State = STANCE;
	m_baseRotation = m_Rotation;
	m_stance_time = 0;
}

//構え中の処理
void Pole::StanceUpdate() {
	if (m_stance_time < 20) {
		m_Rotation.y -= (PI / 2) * 0.05f;
	}
	/*if( m_Rotation.y < m_baseRotation.y - (PI / 2)) {
		m_Rotation.y = m_baseRotation.y - (PI / 2);
	}*/
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
