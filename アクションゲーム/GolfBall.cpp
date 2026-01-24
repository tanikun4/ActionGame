#include <memory>
#include "GolfBall.h"
#include "StaticMesh.h"
#include "utility.h"
#include "Game.h"
#include "Ground.h"
#include "Pole.h"
#include "GroundManager.h"

using namespace std;
using namespace DirectX::SimpleMath;
extern Input input;

// コンストラクタ
GolfBall::GolfBall(Camera* cam) : Object(cam)
{

}

GolfBall::GolfBall()
{

}

// デストラクタ
GolfBall::~GolfBall() 
{

}

void GolfBall::Init() {
	GBInit(u8"assets/model/gorufu/GolfBall_v2.fbx");
}

void GolfBall::Update() {
	GBUpdate();
}

void GolfBall::GBInit(std::u8string modelfilename)
{
	// メッシュ読み込み
	StaticMesh staticmesh;

	//3Dモデルデータ
	std::u8string modelFile = modelfilename;

	//テクスチャディレクトリ
	std::string texDirectory = "assets/model/Character";

	//Meshを読み込む
	std::string tmpStr1(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
	staticmesh.Load(tmpStr1, texDirectory);

	m_MeshRenderer.Init(staticmesh);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/litTextureVS_notlight.hlsl", "shader/litTexturePS.hlsl");

	// サブセット情報取得
	m_subsets = staticmesh.GetSubsets();

	// テクスチャ情報取得
	m_Textures = staticmesh.GetTextures();

	// マテリアル情報取得	
	std::vector<MATERIAL> materials = staticmesh.GetMaterials();

	// マテリアル数分ループ
	for (int i = 0; i < materials.size(); i++)
	{
		// マテリアルオブジェクト生成
		std::unique_ptr<Material> m = std::make_unique<Material>();

		materials[i].Diffuse = { 1,1,1,1 };

		// マテリアル情報をセット
		m->Create(materials[i]);

		// マテリアルオブジェクトを配列に追加
		m_Materiales.push_back(std::move(m));
	}
	m_Position = DirectX::SimpleMath::Vector3(0.0f, 50.0f, 0.0f);

	//初期スケールを設定
	m_Scale.x = 1;
	m_Scale.y = 1;
	m_Scale.z = 1;

	//最初に速度を与える
	//m_Velocity.x = 0.50f;

	//丸影を生成
	m_Shadow = Game::GetInstance()->AddObject<ShadowCircle>();
	m_Shadow->Init();
}

void GolfBall::GBUpdate()
{
	//行列の更新
	UpdateForwardDirectionVectors();

	m_oldPos = m_Position; //1フレーム前の位置を記憶しておく

	float keepY = m_Velocity.y;

	//特殊移動中は処理をスキップ
	if(!is_SPECIALMOVE) {
		m_Velocity = m_ForwardVector * m_Velocity_f;
		
		m_Velocity.y = keepY;
	}

	//速度を座標に加算
	m_Position += m_Velocity;

	if (CheckGround())//もし当たっていれば
	{
		is_GROUND = true;

	}
	else {
		//もし当たっていなければ
		is_GROUND = false;
	}

	if (is_GROUND) {
		m_Velocity.y = 0;//Y方向の速度を0にする
	}
	else {
		m_Velocity.y -= gravity;
	}

	//丸影の更新
	m_Shadow->UpdateShadow(m_Position, -0.1f);//地面座標が一旦決め打ち、そのうち地面のシステムから変えたい。

}

void GolfBall::GBDraw()
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

	m_Shadow->Draw();
}

void GolfBall::Draw() {
	GBDraw();
}

void GolfBall::Uninit()
{

}

// 状態の設定・取得
void GolfBall::SetState(int s) { m_State = s; }
int GolfBall::GetState() { return m_State; }

// ショット
void GolfBall::Shot(Vector3 v) { m_Velocity = v;}

float GolfBall::GetRadius() { return radius; }

Vector3 GolfBall::GetForwardVector() { return m_ForwardVector; }

bool GolfBall::CheckGround() {
	//Groundの頂点データを取得

	const auto& ground_polygon = GroundManager::GetInstance().GetGroundPolygons();

	float moveDistance = 9999; //移動距離
	Vector3 contactPoint; //接触点
	Vector3 normal;

	//線分とポリゴンの当たり判定
	for (const auto& poly : ground_polygon)
	{
		Vector3 cp; //接触点
		Collision::Segment collisionSegment = { m_oldPos, m_Position };
		if (Collision::CheckHit(collisionSegment, poly, cp))
		{
			float md = 0;
			Vector3 np = Collision::moveSphere(collisionSegment, radius, poly, cp, md);
			if (moveDistance > md)
			{
				
				moveDistance = md;
				m_Position = np;
				contactPoint = cp;
				normal = Collision::GetNormal(poly);
			}
			return true;
		}
	}

	//球体とポリゴンの当たり判定
	for (const auto& poly : ground_polygon) {
		Vector3 cp; //接触点
		Collision::Sphere collisionSphere = { m_Position, radius };
		if (Collision::CheckHit(collisionSphere, poly, cp)) {
			float md = 0;
			Vector3 np = Collision::moveSphere(collisionSphere, poly, cp);
			md = (np - m_oldPos).Length();
			if (moveDistance > md)
			{
				
				moveDistance = md;
				m_Position = np;
				contactPoint = cp;
				normal = Collision::GetNormal(poly);
			}
			return true;
		}
	}

	return false;
}


Collision::ColliderVariant GolfBall::GetCollision(){
	return Collision::Sphere{ m_Position, radius };
}

void GolfBall::SetColor(const DirectX::SimpleMath::Vector4& color) {//色を変える
	for (auto& m : m_Materiales) {
		m.get()->SetDiffuse(color);
		m.get()->Update();
	}
}

Vector3 GolfBall::ToCameraEffectPos(Vector3 _pos, float _dist) {
	//エフェクト再生の位置調整
	Vector3 forward = m_Camera->GetForwardVector();
	Vector3 toCamera = m_Camera->GetPosition() - _pos;
	toCamera.Normalize();

	// forward を toCamera 方向へ投影し、横ズレ除去
	Vector3 adjustedForward = toCamera * forward.Dot(toCamera);

	// 長さを維持したい場合は再正規化
	adjustedForward.Normalize();

	// 対象座標からdist分前方にずらすした座標を返す
	Vector3 pos = _pos - adjustedForward * _dist;
	return pos;
}
