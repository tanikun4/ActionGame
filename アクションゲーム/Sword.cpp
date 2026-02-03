//#include <memory>
#include "Sword.h"
#include "BallObject.h"
#include "StaticMesh.h"
#include "utility.h"
#include "Game.h"
#include "Ground.h"
#include "Collision.h"
#include "EffectManager.h"
#include "DebugUI.h"
#include "EffectTrail.h"

using namespace std;
using namespace DirectX::SimpleMath;

// コンストラクタ
Sword::Sword(Camera* cam) :Weapon(cam)
{
	m_EffectTrail = Game::GetInstance()->AddObject<EffectTrail>();
}

Sword::Sword() {

}

// デストラクタ
Sword::~Sword()
{

}

//=======================================
// 初期化処理
//=======================================
void Sword::Init()
{
	// メッシュ読み込み
	StaticMesh staticmesh;

	// 3Dモデルデータ
	std::u8string modelFile = u8"assets/model/Weapon/Sword/sword.obj";

	// テクスチャディレクトリ
	std::string texDirectory = "assets/model/Weapon/Sword";

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

	//軌跡エフェクト初期化
	m_EffectTrail->Init();
	m_EffectTrail->SetTexture("assets/texture/white.png");
}


void Sword::Update()
{
	
}

//=======================================
// 更新処理
//=======================================
void Sword::Update(Vector3 position, float radius, Vector3 rotation)//offsetはobbの距離調整用
{

	switch (m_State) {
	case NORMAL: //通常状態
		m_Rotation = { PI * 0.5f + angle_debug.x,  PI * 0.5f + rotation.y + angle_debug.y, PI * 0.5f + angle_debug.z};
		m_baseRotation = m_Rotation;
		break;
	case SWING: //振り攻撃中
		m_baseRotation.y = rotation.y + PI * 0.5f;
		m_baseRotation.z = -rotation.x + PI * 0.5f;
		SwingUpdate();
		break;
	case GUARD: //ガード中
		m_Rotation = { PI * 0.5f, rotation.y ,PI * 0.5f };
		m_baseRotation = m_Rotation;
		m_offset = { offset_debug.x + radius * 2 , offset_debug.y, offset_debug.z + radius * 0.5f};
		break; 
	case STANCE: //構え中
		//構え中の補正処理
		m_Rotation = { PI * 0.5f, rotation.y + PI * 0.5f ,PI * 0.5f };
		m_baseRotation = m_Rotation;
		StanceUpdate();
		break;
	case ATTACK: //攻撃中(回転攻撃など)
		m_Rotation = { PI * 0.5f, rotation.y + PI * 0.5f ,PI * 0.5f };
		if (followFg) m_Rotation.z += rotation.x;
		if (verticalFg) m_Rotation.x = 0;
		NormalizeRad(m_Rotation.z);
		break;
	case THRUST: //突き攻撃中
		m_Rotation = { PI * 0.5f + angle_debug.x, rotation.y + PI * 0.5f + angle_debug.y, PI * 0.5f + angle_debug.z };
		m_baseRotation = m_Rotation;
		ThrustUpdate();
		break;
	}

	//位置計算
	float yaw = rotation.y; // 横回転（Y軸）
	float pitch = rotation.x ; // 縦回転（X軸）

	Vector3 forward;
	forward.x = cosf(pitch) * sinf(yaw);
	forward.y = -1 * sinf(pitch); // +が上向きのため、逆にする
	forward.z = cosf(pitch) * cosf(yaw);

	m_Position.x = position.x + forward.x * radius;
	m_Position.y = position.y + forward.y * radius;
	m_Position.z = position.z + forward.z * radius;

	UpdateOffset(yaw);
	UpdateOBB();

}

void Sword::UpdateOffset(float _yaw) {
	//オフセット部分の更新
	Vector3 rotOffset;

	// Yaw + Pitch 回転
	rotOffset.x = m_offset.x * cosf(_yaw) + m_offset.z * sinf(_yaw);

	rotOffset.y = m_offset.y;

	rotOffset.z = -m_offset.x * sinf(_yaw) + m_offset.z * cosf(_yaw);

	m_Position += rotOffset;

	Vector3 rotOffset_debug;

	rotOffset_debug.x = offset_debug.x * cosf(_yaw) + offset_debug.z * sinf(_yaw);

	rotOffset_debug.y = offset_debug.y;

	rotOffset_debug.z = -offset_debug.x * sinf(_yaw) + offset_debug.z * cosf(_yaw);

	m_Position += rotOffset_debug;
}

// OBBの更新処理
void Sword::UpdateOBB() {
	// 回転行列とワールド行列
	Matrix S = Matrix::CreateScale(m_Scale);
	Matrix R = Matrix::CreateFromYawPitchRoll(
		m_Rotation.y,
		m_Rotation.x,
		m_Rotation.z
	);
	Matrix T = Matrix::CreateTranslation(m_Position);

	Matrix world = S * R * T;

	//OBBの更新
	// OBB の中心位置を「武器の中心」に補正
	// ポールモデルの pivot は "持ち手先端"
	Vector3 obbLocalCenter = { 0.0f, m_Scale.y * 0.8f, 0.0f };

	// ワールド座標へ変換
	Vector3 obbWorldCenter = Vector3::Transform(obbLocalCenter, world);

	// OBBの更新
	obb = {
		obbWorldCenter,
		m_Rotation,
		{ m_Scale.x, m_Scale.y * 2.5f, m_Scale.z }
	};

	m_EffectTrail->Update();


	if (!atkFg) return;

	// OBB更新時に作成した値を基にして、軌跡エフェクト更新
	Vector3 dir = Vector3::TransformNormal(Vector3::UnitY, R);
	float halfLen = m_Scale.y * trailSize;

	Vector3 base = obbWorldCenter - dir * halfLen;
	Vector3 tip = obbWorldCenter + dir * 8.0f;

	m_EffectTrail->AddPoint(base, tip);
}

// 描画処理
void Sword::Draw()
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

// 終了処理
void Sword::Uninit()
{

}

// 振り攻撃開始、プリセット版
void Sword::Swing() 
{
	
	SwingStart({0, -PI * 0.5f, 0}, {0,PI * 0.5f,0}, 18, 0.7f);
}

// 逆振り攻撃開始
void Sword::Swing_Return() 
{

	SwingStart({ 0, PI * 0.5f, 0 }, { 0,-PI * 0.5f,0 }, 18, 0.7f);
}

// 縦振り攻撃開始、デフォルト版
void Sword::Swing_Vertical() 
{
	
	m_Rotation.x = PI;//縦向きにする
	SwingStart({0,0,PI * 0.5f}, {0,0,-PI * 0.2f}, 10, 1.0f);
}

void Sword::Swing_Parry() 
{
	m_EffectTrail->Start();//軌跡エフェクト開始
	m_offset = { 0,0,0 };//振り攻撃中はオフセット無し
	m_AngleAnim.StartAbsolute({ 0, -PI * 0.5f, 0 }, { 0,PI * 0.5f,0 }, 10, 1.0f);
	m_baseRotation = m_Rotation;
	m_stancetime = 0;
	m_attacktime = 0;
	m_State = SWING;
	max_attacktime = 10;
	atkFg = false;
}

// 振り攻撃、時間とモード指定版
void Sword::Swing(int t, SwingMode mode)
{
	m_State = STANCE;
	m_stancetime = 0;
	switch (mode)
	{
	case SwingMode::NORMAL:
		SwingStart({ 0, -PI * 0.5f, 0 }, { 0,PI * 0.5f,0 }, t, 0.7f);
		break;

	case SwingMode::RETURN:
		SwingStart({ 0, PI * 0.5f, 0 }, { 0,-PI * 0.5f,0 }, t, 0.7f);
		break;

	case SwingMode::VERTICAL:
		m_Rotation.x = PI;
		SwingStart({ 0,0,PI * 0.5f }, { 0,0,-PI * 0.2f }, t,1.0f);
		break;
	}
}

// 振り攻撃開始、パラメータ版
void Sword::SwingStart(const Vector3& s, const Vector3& e, int t, float accel)
{
	m_baseRotation = m_Rotation;// 基準角度を保存
	m_EffectTrail->Start();//軌跡エフェクト開始
	m_offset = { 0,0,0 };//振り攻撃中はオフセット無し
	m_AngleAnim.StartAbsolute(s, e, t,accel);
	max_attacktime = t;
	m_stancetime = 0;
	m_attacktime = 0;
	m_State = SWING;
	atkFg = true;
}

// 振り攻撃中の処理
void Sword::SwingUpdate() {
	++m_attacktime;
	m_Rotation =  m_baseRotation + m_AngleAnim.UpdateAbsolute();
}

// 振り攻撃終了
void Sword::SwingEnd() {
	if (m_State != SWING) { return; }
	m_State = NORMAL;
	atkFg = false;
	m_attacktime = 0;
	m_EffectTrail->End();
}

// 突き攻撃開始、デフォルト版
void Sword::Thrust() {
	ThrustStart({ 0, 0, 0 }, { 0, 0, m_Scale.y * 2.5f }, 2, 0.3f);
}

// 突き攻撃開始、デフォルト版
void Sword::Thrust_Right() {
	ThrustStart({ m_Scale.x * 1.5f, 0, 0 }, { m_Scale.x * 1.5f, 0, m_Scale.y * 2.5f }, 2, 0.3f);
}


// 突き攻撃開始、デフォルト版
void Sword::Thrust_Left() {
	ThrustStart({ -m_Scale.x * 1.5f, 0, 0 }, { -m_Scale.x * 1.5f, 0, m_Scale.y * 2.5f }, 2, 0.3f);
}


// 突き攻撃開始、パラメータ版
void Sword::ThrustStart(const Vector3& s, const Vector3& e, int t, float accel)
{
	m_EffectTrail->Start();
	m_PosAnim.StartAbsolute(s, e, t, accel);
	max_attacktime = t;
	m_stancetime = 0;
	m_attacktime = 0;
	m_State = THRUST;
	atkFg = true;
}

// 突き攻撃中の処理
void Sword::ThrustUpdate() {
	++m_attacktime;
	m_offset = m_PosAnim.UpdateAbsolute(); // 位置アニメーション更新
	//　突き出し終わっていたら、引き戻す動きを開始
	if (!m_PosAnim.IsPlaying() && atkFg) {
		atkFg = false;
		m_PosAnim.StartRelative(m_offset, { 0, 0, 0 }, m_attacktime - 1, 0);
		m_attacktime = 0;
	}
}

// 突き攻撃終了
void Sword::ThrustEnd() {
	if (m_State != THRUST) { return; }
	m_State = NORMAL;
	atkFg = false;
	m_attacktime = 0;
	m_offset = { 0,0,0 };
	m_EffectTrail->End();
}


//攻撃状態になるだけの関数、回転切り等で使用
void Sword::AttackStart(bool _follow,bool _vt) {
	m_EffectTrail->Start();
	m_State = ATTACK;
	m_attacktime = 0;
	m_stancetime = 0;
	m_Rotation = m_baseRotation;
	atkFg = true;
	followFg = _follow;
	verticalFg = _vt;
}

//攻撃状態終了
void Sword::AttackEnd() {
	m_EffectTrail->End();
	m_State = NORMAL;
	atkFg = false;
	followFg = false;
	verticalFg = false;
	m_attacktime = 0;
	m_offset = { 0,0,0 };
}

//構え開始、デフォルト版
void Sword::Stance() {
	m_State = STANCE;
	m_stancetime = 0;

	StanceStart({ 0,0,0 }, {0,-PI * 0.5f,0}, 18);
}

void Sword::Stance_Return() {
	m_State = STANCE;
	m_stancetime = 0;

	StanceStart({ 0,0,0 }, { 0,PI * 0.5f,0 }, 18);
}

//構え開始、デフォルト版を時間、構えタイプの指定を可能にしたもの
void Sword::Stance(int t, StanceMode mode) {
	m_State = STANCE;
	m_stancetime = 0;
	switch (mode)
	{
	case StanceMode::NORMAL:
		StanceStart({ 0,0,0 }, { 0,-PI * 0.5f,0 }, t);
		break;

	case StanceMode::RETURN:
		StanceStart({ 0,0,0 }, { 0,PI * 0.5f,0 }, t);
		break;

	case StanceMode::VERTICAL:
		StanceStart({ 0,0,0 }, { PI * 0.5f,0,(PI * 0.5f) + 0.2f }, t);
		break;
	case StanceMode::THRUST:
		Stance_Thrust();
		break;
	}
}

void Sword::Stance_Thrust() {
	m_PosAnim.StartRelative({ 0,0,0 }, { m_Scale.x * 2, 0, -m_Scale.z * 2 }, 20);
	m_stancetime = 0;
	m_State = STANCE;
	max_stancetime = 30;
	m_AngleAnim.Reset();
}

// 縦構え開始、デフォルト版
void Sword::Stance_Vertical() {
	StanceStart({0,0,0}, { PI * 0.5,0,(PI * 0.5) + 0.2f}, 60);
}


//構え開始、パラメータ版
void Sword::StanceStart(const Vector3& s, const Vector3& e, int t) {
	m_AngleAnim.StartRelative(s, e, t);
	m_PosAnim.Reset();
	m_baseRotation = m_Rotation;
	m_stancetime = 0;
	m_State = STANCE;
	max_stancetime = t;
}


//構えてから振る(没関数)
void Sword::StanceToSwing(const Vector3& s_stance, const Vector3& e_stance, int t_stance,
	const Vector3& s_swing, const Vector3& e_swing, int t_swing, int swingframe) 
{
	m_AngleAnim.StartRelative(s_stance, e_stance, t_stance);
	m_stancetime = 0;
	m_stance_attackframe = swingframe;
	m_State = STANCE;
	m_baseRotation = m_Rotation;
}

//現在角度から振る
void Sword::ToSwing(const DirectX::SimpleMath::Vector3& e, int t, float accel)
{
	m_AngleAnim.StartRelative(e, t, accel);
	max_attacktime = t;
	m_stancetime = 0;
	m_attacktime = 0;
	m_State = SWING;
	atkFg = true;
}

void Sword::ToSwing()
{
	Vector3 endrot = m_baseRotation;
	endrot.y += PI;
	ToSwing(endrot, max_attacktime,0.3f);
}

//構え中の処理
void Sword::StanceUpdate() 
{
	m_Rotation += m_AngleAnim.UpdateAbsolute();
	
	m_offset = m_PosAnim.UpdateAbsolute();
	
	++m_stancetime;
}

void Sword::StanceEnd() 
{
	m_State = NORMAL;
	m_Rotation = m_baseRotation;
	m_offset = { 0,0,0 };
}

int Sword::GetState() { return m_State; }

//位置の設定
void Sword::SetPosition(float x, float y, float z)
{
	Vector3 p = { x, y, z };
	SetPosition(p);
}
void Sword::SetPosition(Vector3 pos)
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

void Sword::SetTrailColor(Vector4 color) {
	m_EffectTrail->SetColor(color);
}

Collision::ColliderVariant Sword::GetCollision() {
	return obb;
}

void Sword::GuardStart() {
	m_State = 2;
	m_baseRotation = m_Rotation;
}

void Sword::GuardEnd() {
	m_State = 0;
	m_offset = { 0,0,0 };
}

// 武器の先端位置にエフェクトを表示する
void Sword::TipToEffect(int _id, EffectParams _param)
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



void Sword::DebugPoleStatus() {
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

	ImGui::Begin("Sword Axis Debug");

	ImGui::Text("X Axis (Right):   %.2f %.2f %.2f", xAxis.x, xAxis.y, xAxis.z);
	ImGui::Text("Y Axis (Up/Tip?): %.2f %.2f %.2f", yAxis.x, yAxis.y, yAxis.z);
	ImGui::Text("Z Axis (Forward): %.2f %.2f %.2f", zAxis.x, zAxis.y, zAxis.z);

	ImGui::End();

}

bool Sword::GetMaxAttack() {
	if (m_attacktime >= max_attacktime) {
		return true;
	}
	return false;
}

bool Sword::GetMaxStance() {
	if (m_stancetime >= max_stancetime) {
		return true;
	}
	return false;
}
