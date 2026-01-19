#pragma once
#include "TestCube.h"
#include "Weapon.h"
#include "AnimStruct.h"

struct EffectParams;
enum class StanceMode {
	NORMAL = 0,
	RETURN,
	VERTICAL,
	THRUST,

	MODE_MAX
};

enum class SwingMode {
	NORMAL = 0,
	RETURN,
	VERTICAL,

	MODE_MAX
};

class EffectTrail;

//-----------------------------------------------------------------------------
// Poleクラス
//-----------------------------------------------------------------------------
class Pole :public Weapon
{
private:

	Collision::OBB obb {m_Position,m_Rotation,m_Scale };

	int m_State = 0; // 0:通常状態・1:攻撃中 2:ガード中 
	int m_attacktime = 0; //スイング時間カウント
	int m_stancetime = 0;//構え時間カウント
	int m_stance_attackframe = -1;//構えから振りに移行する場合の振り開始フレーム

	static const int Defalt_AttackTime = 18;//振り攻撃の最大フレーム数、デフォルト値
	int max_attacktime = Defalt_AttackTime;//振り攻撃の最大フレーム数
	static const int Defalt_StanceTime = 18;//構えの最大フレーム数、デフォルト値
	int max_stancetime = 60;//構えの最大フレーム数

	float trailSize = 1.0f;//軌跡エフェクトのサイズ

	bool atkFg = false;//攻撃判定があるかどうか
	bool followFg = false;//持ち主と同じ角度になるかどうか
	bool verticalFg = false;//縦かどうか

	DirectX::SimpleMath::Vector3 m_baseRotation { 0,0,0 };//振る前の角度

	AngleAnim m_AngleAnim;//角度アニメーションで使う構造体
	PositionAnim m_PosAnim;// 座標アニメーションで使う構造体
	EffectTrail* m_EffectTrail; // 軌跡のエフェクト
public:

	enum STATE {
		NORMAL = 0,
		SWING,
		GUARD,
		STANCE,
		ATTACK,
		SWING_VERTICAL,
		THRUST,
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

	void UpdateOffset(float _yaw);//オフセット部分のUpdate
	void UpdateOBB();//OBBの更新

	void DebugPoleStatus();//デバッグ用関数(プレイヤー側でのみ呼び出す想定でpublic)

	void HitObject(Object* ob) override { //当たり判定を増やす場合、Objectに基底関数を追加すること。
		if (!atkFg) { return; };//攻撃判定フラグが無ければ判定を行わない
		ob->OnHit(this);
	}
	void OnHit(Object* ob) override {};


	// 振り攻撃形の関数群
	void Swing();//振り攻撃開始、デフォルト
	void Swing_Parry();//振り攻撃開始、デフォルト
	void Swing_Return();//逆からの振り攻撃
	void Swing_Vertical();// 縦振り攻撃開始
	void Swing(int t ,int mode);//振り時間、モード指定版
	void SwingEnd();

	void SwingStart(const DirectX::SimpleMath::Vector3& s, const DirectX::SimpleMath::Vector3& e, int t, float accel = 0);// 振り攻撃開始、パラメータ版
	void SwingUpdate();

	//突き攻撃
	void Thrust();//突き攻撃開始、デフォルト
	void Thrust_Right();//突き攻撃開始右から、デフォルト
	void Thrust_Left();//突き攻撃開始左から、デフォルト
	void ThrustStart(const DirectX::SimpleMath::Vector3& s, const DirectX::SimpleMath::Vector3& e, int t, float accel = 0);//突き攻撃開始、パラメータ版
	void ThrustUpdate();
	void ThrustEnd();

	// 構え関数群
	void Stance();
	void Stance_Return();
	void Stance_Vertical();
	void Stance_Thrust(); // 突き状態の構え
	void Stance(int t,int mode);//時間、構えタイプ指定版
	void StanceUpdate();
	void StanceEnd();

	//構えて開始
	void StanceStart(const DirectX::SimpleMath::Vector3& s, const DirectX::SimpleMath::Vector3& e, int t);
	void StanceToSwing(const DirectX::SimpleMath::Vector3& s_stance, const DirectX::SimpleMath::Vector3& e_stance, int t_stance,
						const DirectX::SimpleMath::Vector3& s_swing, const DirectX::SimpleMath::Vector3& e_swing, int t_swing,int swingframe = -1);

	//現在角度から振る
	void ToSwing(const DirectX::SimpleMath::Vector3& e, int t, float accel = 0);
	//現在の角度から振る、振り終わりの角度はbaseRotationを使用
	void ToSwing();

	void GuardStart();
	void GuardEnd();

	// 攻撃開始・終了
	void AttackStart(bool _follow = false, bool _vt = false);//follow=trueなら持ち主と同じ角度になる vt = trueなら縦になる
	void AttackEnd();

	void ChangeState(int state) {}; // 状態を変更する(現在未使用)

	// 位置の設定
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::SimpleMath::Vector3 pos);

	void SetTrailSize(float _size) { trailSize = _size; };//軌跡エフェクトのサイズ設定
	void SetTrailColor(DirectX::SimpleMath::Vector4 color);//軌跡エフェクトの色設定
	int GetState();//状態を返す
	Collision::ColliderVariant GetCollision();
	DirectX::SimpleMath::Vector3 GetBaseRotation() { return m_baseRotation; }
	int GetAttackTime() { return m_attacktime; }
	int GetStanceTime() { return m_stancetime; }
	bool GetAttack() { return atkFg; }
	bool GetMaxStance();
	bool GetMaxAttack();


	void TipToEffect(int _id, EffectParams _param);//先端からエフェクトを再生する

};