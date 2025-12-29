#pragma once
#include "TestCube.h"
#include "Weapon.h"
#include "AngleAnim.h"

struct EffectParams;

//-----------------------------------------------------------------------------
// Poleクラス
//-----------------------------------------------------------------------------
class Pole :public Weapon
{
private:

	Collision::OBB obb {m_Position,m_Rotation,m_Scale };

	int m_State = 0; // 0:通常状態・1:攻撃中 2:ガード中 
	int m_swing_time = 0; //スイング時間カウント
	int m_stance_time = 0;//構え時間カウント
	int m_stance_swingframe = -1;//構えから振りに移行する場合の振り開始フレーム
	bool atkFg = false;//攻撃判定があるかどうか

	DirectX::SimpleMath::Vector3 m_baseRotation { 0,0,0 };//振る前の角度

	AngleAnim m_SwingAnim;
public:

	enum STATE {
		NORMAL = 0,
		SWING,
		GUARD,
		STANCE,
		ATTACK,
		SWING_VERTICAL
	};

	Pole(Camera* cam); // コンストラクタ
	Pole(); // コンストラクタ
	~Pole(); // デストラクタ

	//TestCube hitbox;
	//int atk = 3;//攻撃力
	void Init();
	void Update();
	void Update(DirectX::SimpleMath::Vector3 position, float radius,DirectX::SimpleMath::Vector3 rotation, float offset);
	void Draw();
	void Uninit();

	void DebugPoleStatus();//デバッグ用関数(プレイヤー側でのみ呼び出す想定でpublic)
	
	// 位置の設定
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::SimpleMath::Vector3 pos);

	void HitObject(Object* ob) override { //当たり判定を増やす場合、Objectに基底関数を追加すること。
		if (!atkFg) { return; };//攻撃判定フラグが無ければ判定を行わない
		ob->OnHit(this);
	}
	void OnHit(Object* ob) override {};


	void Swing();
	void Swing_Vertical();
	void SwingEnd();

	void SwingStart(const DirectX::SimpleMath::Vector3& s, const DirectX::SimpleMath::Vector3& e, int t,float accel = 0);
	void SwingUpdate();

	void Stance_Vertical();

	void StanceStart(const DirectX::SimpleMath::Vector3& s, const DirectX::SimpleMath::Vector3& e, int t);
	void StanceToSwing(const DirectX::SimpleMath::Vector3& s_stance, const DirectX::SimpleMath::Vector3& e_stance, int t_stance,
						const DirectX::SimpleMath::Vector3& s_swing, const DirectX::SimpleMath::Vector3& e_swing, int t_swing,int swingframe = -1);

	//現在角度から振る
	void ToSwing(const DirectX::SimpleMath::Vector3& e, int t, float accel = 0);


	void GuardStart();
	void GuardEnd();

	void AttackStart();
	void AttackEnd();

	void StanceStart();
	void StanceUpdate();
	void StanceEnd();
	void ChangeState(int state) {}; // 状態を変更する(現在未使用)

	int GetState();//状態を返す
	Collision::ColliderVariant GetCollision();
	DirectX::SimpleMath::Vector3 GetBaseRotation() { return m_baseRotation; }
	int GetSwingTime() { return m_swing_time; }
	int GetStanceTime() { return m_stance_time; }
	bool GetAttack() { return atkFg; }

	void TipToEffect(int _id, EffectParams _param);//先端からエフェクトを再生する

};