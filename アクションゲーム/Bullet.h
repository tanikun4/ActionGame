#pragma once
#include "Weapon.h"
class Bullet :
    public Weapon
{
private:
    int m_State = 0; // 0:非表示・1:方向選択・2:パワー選択
    const int Bullettime = 60;//弾が飛んでいる時間
    int id = 0;//弾の種類 0:判定無し,1:プレイヤーの弾,2:敵の弾
	int power = 0;//溜めた量(弾の速度に関係)
	const int maxpower = 180;//パワーの最大値

	Collision::Sphere sphere{ m_Position, m_Scale.x };

	Texture m_texture;
public:
	Bullet(Camera* cam); // コンストラクタ
	Bullet();
	~Bullet(); // デストラクタ
	void Init();
	void Update();
	void Draw();
	void Uninit();
	void PCharge(float rotation_y, float radius, DirectX::SimpleMath::Vector3 position);//プレイヤー弾の溜め状態
	void PShot(float rotation_y, float radius, DirectX::SimpleMath::Vector3 position);//プレイヤー弾の発射
	void EPose(float rotation_y, float radius, DirectX::SimpleMath::Vector3 position);//敵の弾発射前状態
	void EShot(float rotation_y, float radius, DirectX::SimpleMath::Vector3 position);//敵の弾発射

	void LookAtShot(DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 target,bool _pl); // 注視点に向けて発射
	// 状態の設定
	void SetState(int s);
	void SetId(int _id);

	// 矢印のベクトルを取得
	DirectX::SimpleMath::Vector3 GetVector();
	DirectX::SimpleMath::Vector3 GetForwardVector();
	int GetState();
	int GetId();
	int GetAtk();
	Collision::ColliderVariant GetCollision();

	void HitObject(Object* ob) override {
		if (m_State != 2) { return; };
		ob->OnHit(this);
	}
	void OnHit(Object* ob) override {};
	void OnHit(Pole* po) override;
};

