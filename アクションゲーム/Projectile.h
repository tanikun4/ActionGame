#pragma once
#include "TestCube.h"
#include "Weapon.h"

//-----------------------------------------------------------------------------
// 飛び道具クラス
//-----------------------------------------------------------------------------
class Projectile :public Weapon
{
private:

	enum STATE {
		NOT_ACTIVE = 0,
		STANCE,
		CHARGE,
		SHOT,
	};

	//Collision::Sphere sphere { m_Position,m_Scale.x };

	int m_state = 0;
	int power = 0;//溜めた量
	int charge_power = 1;//溜める速度
	int maxpower = 180;//パワーの最大値
	int shottime = 120;//飛んでいる時間
	bool atkFg = false;//攻撃判定があるかどうか

	Collision::OBB obb{ m_Position,m_Rotation,m_Scale };

public:
	Projectile(Camera* cam); // コンストラクタ
	Projectile();
	~Projectile(); // デストラクタ
	void Init();
	void Update();
	void Draw();
	void Uninit();
	void Move();//移動処理

	void ChargeStart(DirectX::SimpleMath::Vector3 _pos,DirectX::SimpleMath::Vector3 _rot, int _power);//溜め開始
	void MaxCharge(DirectX::SimpleMath::Vector3 _pos, DirectX::SimpleMath::Vector3 _rot, int _power);//最大まで溜める
	void Shot(float _speed = 3,int _atk = 2,int _time = 120);//発射
	void Stance(DirectX::SimpleMath::Vector3 _pos,DirectX::SimpleMath::Vector3 _rot, DirectX::SimpleMath::Vector3 _scale);//構え状態
	
	void UpdateShot();//発射中の更新処理
	
	// 状態の設定
	void SetState(int s);

	// 矢印のベクトルを取得
	DirectX::SimpleMath::Vector3 GetVector();
	DirectX::SimpleMath::Vector3 GetForwardVector();
	int GetState();
	bool GetAtkFg() { return atkFg; }
	Collision::ColliderVariant GetCollision();
};

