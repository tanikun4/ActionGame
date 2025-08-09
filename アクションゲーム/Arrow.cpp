#include <memory>
#include "Arrow.h"
#include "StaticMesh.h"
#include "utility.h"
#include "Game.h"
#include "GolfBall.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
Arrow::Arrow(Camera* cam) :Weapon(cam)
{

}

Arrow::Arrow() {

}

// デストラクタ
Arrow::~Arrow()
{

}

//=======================================
// 初期化処理
//=======================================
void Arrow::Init()
{
	// メッシュ読み込み
	StaticMesh staticmesh;

	// 3Dモデルデータ
	std::u8string modelFile = u8"assets/model/gorufu/arrow.fbx";
	// テクスチャディレクトリ
	std::string texDirectory = "assets/texture/arrow";

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

	m_State = 1;
	id = 0;
	hitbox.SetPos({ 9999, 9999, 9999});
}

//=======================================
// 更新処理
//=======================================
void Arrow::Update()
{
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
		m_Position += m_ForwardVector * m_Velocity_f;//モデルの向きの関係からマイナスで行っている
		flamecount++;
		if (flamecount > shottime) {
			flamecount = 0;
			m_State = 0;
			power = 0;
		}
		hitbox.SetRotation(m_Rotation);
		hitbox.SetPos({ m_Position.x + sin(m_Rotation.y - PI / 2) * m_Scale.x / 2, m_Position.y, m_Position.z + cos(m_Rotation.y - PI / 2) * m_Scale.z / 2 });
		hitbox.SetScale({ m_Scale.x * 0.1f ,m_Scale.y * 0.15f ,m_Scale.z * 0.1f });
	}
}

//=======================================
// 描画処理
//=======================================
void Arrow::Draw()
{
	if (m_State == 0)return; // 非表示ならreturn

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
void Arrow::Uninit()
{

}


//状態の設定
void Arrow::SetState(int s)
{
	m_State = s;
}

// 矢印のベクトルを取得
Vector3 Arrow::GetVector()
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

void Arrow::PCharge(float rotation_y, float radius, Vector3 position) {
	id = 0;
	m_State = 1;
	m_Rotation.y = rotation_y/* + PI*/;
	m_Position = { position.x/* + sin(rotation_y) * radius * m_Scale.z*/, position.y + radius * 1.5f,  position.z/* + cos(rotation_y) * radius * m_Scale.z */};
	if (power < maxpower) {
		power++;
	}
	else if(power > maxpower){
		power = maxpower;
	}
}

void Arrow::PShot(float rotation_y, float radius, Vector3 position) {
	id = 1;
	m_State = 2;
	m_Position = { position.x + sin(rotation_y) * radius * m_Scale.z, position.y,  position.z + cos(rotation_y) * radius * m_Scale.z  };
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

void Arrow::EPose(float rotation_y, float radius, Vector3 position) {
	id = 0;
	m_State = 1;
	power = maxpower / 2;
	m_Rotation.y = rotation_y + PI;
	m_Position = { position.x + sin(rotation_y) * radius, position.y,  position.z + cos(rotation_y) * radius};
}

void Arrow::EShot(float rotation_y, float radius, Vector3 position) {
	hitbox.SetPos({ 9999, 9999, 9999 });
	id = 2;
	m_State = 2;
	m_Position = { position.x + sin(rotation_y) * radius * m_Scale.z, position.y,  position.z + cos(rotation_y) * radius * m_Scale.z };
	m_Velocity_f = power * 0.01;
	atk = 1;
}

void Arrow::SetId(int _id) {
	id = _id;
}

int Arrow::GetState() {
	return m_State;
}

int Arrow::GetId() {
	return id;
}

int Arrow::GetAtk() {
	return atk;
}

Vector3 Arrow::GetForwardVector() { return m_ForwardVector; }