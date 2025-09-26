#pragma once
#include "TestCube.h"
#include "Weapon.h"


//-----------------------------------------------------------------------------
// Poleクラス
//-----------------------------------------------------------------------------
class Pole :public Weapon
{
private:

	Collision::OBB obb {m_Position,m_Rotation,m_Scale };

	int m_State = 0; // 0:非攻撃・1:攻撃中
	int swing_time = 0;

	DirectX::SimpleMath::Vector3 m_baseRotation { 0,0,0 };//振る前の角度
public:

	Pole(Camera* cam); // コンストラクタ
	Pole(); // コンストラクタ
	~Pole(); // デストラクタ

	TestCube hitbox;
	int atk = 3;//攻撃力
	void Init();
	void Update();
	void Update(DirectX::SimpleMath::Vector3 position, float radius,DirectX::SimpleMath::Vector3 rotation, float offset);
	void Draw();
	void Uninit();
	
	// 位置の設定
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::SimpleMath::Vector3 pos);

	void HitObject(Object* ob) override { //当たり判定を増やす場合、Objectに基底関数を追加すること。
		if (m_State != 1) { return; };
		ob->OnHit(this);
	}
	void OnHit(Object* ob) override {};
	void Swing();
	int GetState();//状態を返す
	Collision::ColliderVariant GetCollision();
	DirectX::SimpleMath::Vector3 GetBaseRotation() { return m_baseRotation; }
	int GetSwingTime() { return swing_time; }
};