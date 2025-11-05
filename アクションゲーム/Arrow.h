#pragma once
#include "TestCube.h"
#include "Weapon.h"

//-----------------------------------------------------------------------------
// Arrowクラス
//-----------------------------------------------------------------------------
class Arrow :public Weapon
{
private:

	Collision::Sphere sphere { m_Position,m_Scale.x };

	int m_State = 0; // 0:非表示・1:方向選択・2:パワー選択
	int power = 0;//溜めた量(1フレームで1パワーアップ)
	const int maxpower = 180;//パワーの最大値
	const int shottime = 120;//弾が飛んでいる時間
	int id = 0;//弾の種類 0:判定無し,1:プレイヤーの弾,2:敵の弾

public:
	Arrow(Camera* cam); // コンストラクタ
	Arrow();
	~Arrow(); // デストラクタ
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

