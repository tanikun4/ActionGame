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
	float power = 0;//溜めた量
	float charge_power = 1;//溜める速度
	float max_power = 100;//パワーの最大値
	int shottime = 120;//飛んでいる時間
	int shottime_max = 120;//飛んでいる時間の最大値
	bool atkFg = false;//攻撃判定があるか
	bool followFg = false;//持ち主に追従するか
	Object* m_Owner = nullptr;//持ち主

	Collision::OBB obb{ m_Position,m_Rotation,m_Scale };

	void Move();//移動処理
	void UpdateShot();//発射中の更新処理
public:
	Projectile(Camera* cam); // コンストラクタ
	Projectile();
	~Projectile(); // デストラクタ
	void Init();
	void Update();
	void Draw();
	void Uninit();

	void ChargeStart(DirectX::SimpleMath::Vector3 _pos,DirectX::SimpleMath::Vector3 _rot, float _power,bool _follow = false);//溜め開始
	void MaxCharge(DirectX::SimpleMath::Vector3 _pos, DirectX::SimpleMath::Vector3 _rot, float _maxpower = 100, bool _follow = false);//最大まで溜める
	void Shot(float _speed = 3,int _atk = 2,int _time = 120,bool _follow = false);//発射
	void Stance(DirectX::SimpleMath::Vector3 _pos,DirectX::SimpleMath::Vector3 _rot, DirectX::SimpleMath::Vector3 _scale);//構え状態
	void Reflect(bool _pl,int _atk = 5);//反射関数
	// 状態の設定
	void SetState(int s);
	void SetOwner(Object* owner) { m_Owner = owner; }
	void SetFollowFg(bool f) { followFg = f; }
	void SetOBBScale(DirectX::SimpleMath::Vector3 _scale) { obb = {m_Position,m_Rotation,_scale}; }
	void SetOffset(DirectX::SimpleMath::Vector3 off) { m_offset = off; }

	void Reset();//状態をリセットする

	// 矢印のベクトルを取得
	DirectX::SimpleMath::Vector3 GetVector();
	DirectX::SimpleMath::Vector3 GetForwardVector();
	int GetState();
	bool GetAtkFg() { return atkFg; }
	Collision::ColliderVariant GetCollision();

	void HitObject(Object* ob) override { //当たり判定を増やす場合、Objectに基底関数を追加すること。
		if (!atkFg) { return; };//攻撃判定フラグが無ければ判定を行わない
		ob->OnHit(this);
	}
};

