#include <memory>
#include "Projectile.h"
#include "StaticMesh.h"
#include "utility.h"
#include "Game.h"
#include "GolfBall.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
Projectile::Projectile(Camera* cam) :Weapon(cam)
{

}

Projectile::Projectile() {

}

// デストラクタ
Projectile::~Projectile()
{

}

//=======================================
// 初期化処理
//=======================================
void Projectile::Init()
{
	// メッシュ読み込み
	StaticMesh staticmesh;

	// 3Dモデルデータ
	std::u8string modelFile = u8"assets/model/Weapon/Bullet/sonic_boom.fbx";
	// テクスチャディレクトリ
	std::string texDirectory = "assets/texture/white.png";

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
	m_Scale.y = 3;
	m_Scale.z = 3;

	m_state = 1;
}

//=======================================
// 更新処理
//=======================================
void Projectile::Update()
{
	switch (m_state)
	{
	case NOT_ACTIVE:
		return;
		break;
	case CHARGE:
		power += charge_power;
		// 最大値チェック
		if(power > max_power) {
			power = max_power;
			m_state = STANCE;
		}
		m_Scale.x = power * 0.01f;
		break;
	case STANCE:
		break;
	case SHOT:
		UpdateShot();
		break;
	}

	if(followFg && m_Owner) {
		// 持ち主に追従
		Vector3 ownerForward = m_Owner->GetForwardRotation();
		m_Position = m_Owner->GetPosition() + ownerForward * followOffset;
	}

	// OBB の更新
	obb = {
		m_Position,
		m_Rotation,
		m_Scale
	};

}

//=======================================
// 描画処理
//=======================================
void Projectile::Draw()
{
	if (m_state == NOT_ACTIVE)return; // 非表示ならreturn

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
void Projectile::Uninit()
{

}


void Projectile::Move()
{
	//まずは回転行列を作成する
	Matrix rotationMatrix = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);

	// 次に初期前向きベクトルを作る
	Vector3 initForward = { 0.0f,0.0f,1.0f };

	//初期前向きベクトル*回転行列
	m_ForwardVector = Vector3::Transform(initForward, rotationMatrix);

	// 現在の座標を計算
	m_Position += m_ForwardVector * m_Velocity_f;//モデルの向きの関係からマイナスで行っている
}

//状態の設定
void Projectile::SetState(int s)
{
	m_state = s;
}

// 矢印のベクトルを取得
Vector3 Projectile::GetVector()
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

// 溜め状態
void Projectile::ChargeStart(Vector3 _pos, Vector3 _rot, float _power,bool _follow) {
	m_Velocity_f = 0;
	atkFg = false;
	m_state = CHARGE;
	m_Rotation = _rot;
	m_Position = _pos;
	charge_power = _power;
	followFg = _follow;
}

void Projectile::MaxCharge(Vector3 _pos, Vector3 _rot, float _maxpower, bool _follow) {
	m_Velocity_f = 0;
	atkFg = false;
	m_state = CHARGE;
	m_Rotation = _rot;
	m_Position = _pos;
	max_power = _maxpower;
	power = max_power;
	followFg = _follow;
}

// 発射
void Projectile::Shot(float _speed,int _atk ,int _time, bool _follow) {
	m_state = SHOT;
	m_Velocity_f = _speed;
	atk = _atk;
	shottime = _time; 
	shottime_max = _time;
	atkFg = true;
	followFg = _follow;
}


// 構え状態
void Projectile::Stance(Vector3 _pos,Vector3 _rot, Vector3 _scale) {
	m_Velocity_f = 0;
	m_state = STANCE;
	m_Rotation = _rot;
	m_Position = _pos;
	m_Scale = _scale;
}

void Projectile::UpdateShot() {
	Move();
	shottime--;
	if (shottime <= 0) {
		m_state = NOT_ACTIVE;
		atkFg = false;
		power = 0;
	}
}

int Projectile::GetState() {
	return m_state;
}

Vector3 Projectile::GetForwardVector() { return m_ForwardVector; }

Collision::ColliderVariant Projectile::GetCollision() {
	return obb;
}

// 角度を反対方向にする
void Projectile::Reflect(bool _pl) {
	m_Rotation *= -1;
	pl = _pl;
	shottime = shottime_max;
	return;
}