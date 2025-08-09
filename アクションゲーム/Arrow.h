#pragma once
#include "Object.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "Material.h"
#include "TestCube.h"
#include "Weapon.h"

//-----------------------------------------------------------------------------
// Arrowクラス
//-----------------------------------------------------------------------------
class Arrow :public Weapon
{
private:
	//速度
	DirectX::SimpleMath::Vector3 m_Velocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

	float m_Velocity_f = 0.0; // 現在の移動速度

	//前向きベクトル変数
	DirectX::SimpleMath::Vector3 m_ForwardVector = { 0.0f,0.0f,1.0f };//初期ベクトル

	// 描画の為の情報（メッシュに関わる情報）
	MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

	// 描画の為の情報（見た目に関わる部分）
	std::vector<std::unique_ptr<Material>> m_Materiales;
	std::vector<SUBSET> m_subsets;
	std::vector<std::unique_ptr<Texture>> m_Textures; // テクスチャ

	int m_State = 0; // 0:非表示・1:方向選択・2:パワー選択
	int power = 0;//溜めた量(1フレームで1パワーアップ)
	const int maxpower = 180;//パワーの最大値
	int flamecount = 0;//タイマー変数
	const int shottime = 120;//弾が飛んでいる時間
	int id = 0;//弾の種類 0:判定無し,1:プレイヤーの弾,2:敵の弾
	int atk = 1;//攻撃力

public:
	Arrow(Camera* cam); // コンストラクタ
	Arrow();
	~Arrow(); // デストラクタ
	TestCube hitbox;
	void Init();
	void Update();
	void Draw();
	void Uninit();
	void PCharge(float rotation_y, float radius, DirectX::SimpleMath::Vector3 position);//プレイヤー弾の溜め状態
	void PShot(float rotation_y, float radius, DirectX::SimpleMath::Vector3 position);//プレイヤー弾の発射
	void EPose(float rotation_y, float radius, DirectX::SimpleMath::Vector3 position);//敵の弾発射前状態
	void EShot(float rotation_y, float radius, DirectX::SimpleMath::Vector3 position);//敵の弾発射
	// 状態の設定
	void SetState(int s);
	void SetId(int _id);

	// 矢印のベクトルを取得
	DirectX::SimpleMath::Vector3 GetVector();
	DirectX::SimpleMath::Vector3 GetForwardVector();
	int GetState();
	int GetId();
	int GetAtk();
};

