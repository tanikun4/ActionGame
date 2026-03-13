#include "EnemyImpl.h"
#include "Player.h"
#include "Ground.h"
#include "Game.h"
#include "Sword.h"
#include "Collision.h"
#include "Projectile.h"
#include "Bullet.h"
#include "DebugUI.h"
#include "EffectManager.h"
#include "EnemyManager.h"

using namespace std;
using namespace DirectX::SimpleMath;


Enemy::Impl::Impl(Camera* cam, Enemy* owner)
	: m_Owner(owner)
	, m_Camera(cam)
{
	m_weapon = Game::GetInstance()->AddObject<Sword>();
	m_weapon->SetOwner(m_Owner);
}

// デストラクタ
Enemy::Impl::~Impl()
{
	if (m_weapon)
	{
		m_weapon->SetLive(false);
		m_weapon->SetOwner(nullptr);
		m_weapon = nullptr;
	}

	for (auto& p : m_projectile)
	{
		p->SetLive(false);
		p->SetOwner(nullptr);
		p->Uninit();
		p = nullptr;
	}
}

void Enemy::Impl::DebugEnemyStatus() {//ボスの状態を操作する
	ImGui::Begin("EnemyStatus");

	ImGui::Checkbox("NotUpdate", &notUpdate);
	ImGui::Checkbox("Slow", &m_slowFg);
	ImGui::SliderInt("Slowrate", &slow_rate, 1, 59);

	ImGui::SliderFloat("JumpPower", &jumppower, 0, 10);

	if (ImGui::Button("STUN"))
		Stun();

	if (ImGui::Button("DEATH")) {
		hp = 0;
	}

	if (ImGui::Button("HP MAX")) {
		hp = maxhp;
	}

	static int debug_attack_kind = -1;
	ImGui::SliderInt("AttackKind", &debug_attack_kind, -1, KIND_MAX - 1);
	if (ImGui::Button("ENEMYATTACK")) {
		m_Owner->m_State = ATTACK;
		m_stateframe = 0;
		m_Owner->m_Velocity_f = 0;
		attack_kind = debug_attack_kind;
	}

	ImGui::SliderFloat3("Rotation", &m_Owner->m_Rotation.x, -2 * PI, 2 * PI);
	if (ImGui::Button("Reset Rotation"))
	{
		m_Owner->m_Rotation.x = 0;
		m_Owner->m_Rotation.z = 0;
	}

	//static Vector3 gauge_pos = { 500,300,0 };
	//ImGui::SliderFloat3("GaugePos", &gauge_pos.x, 0, 700);

	//static Vector2 gauge_scale = { 800,50 };
	//ImGui::SliderFloat2("GaugeScale", &gauge_scale.x, 0, 1000);

	//m_gauge.SetPosScale(gauge_pos, { gauge_scale.x, gauge_scale.y, 0});

	/*static Vector3 projectile_offset = {0,-4,16};
	ImGui::SliderFloat3("Projectile Offset", &projectile_offset.x,-30,30);

	static Vector3 projectile_OBB_scale = { 10.0f,1.0f,1.0f };
	ImGui::SliderFloat3("Projectile OBBScale", &projectile_OBB_scale.x, 0, 20);

	for (auto& p : m_projectile) {
		p->SetOffset(projectile_offset);
		p->SetOBBScale(projectile_OBB_scale);
	}*/

	ImGui::End();
}

void Enemy::Impl::Init() {
	m_Owner->BallInit(u8"assets/model/Character/boss.fbx");
	//m_Owner->m_Position = Vector3(0.0f, 50.0f, -50.0f);
	m_Owner->m_Velocity_f = 0.0f;//はじめに移動速度を0にする
	hp = maxhp;
	def = 0;
	m_Owner->m_Scale.x = 1;
	m_Owner->m_Scale.y = 1;
	m_Owner->m_Scale.z = 1;
	m_Owner->radius *= m_Owner->m_Scale.x;

	//丸影の大きさをセット
	m_Owner->m_Shadow->SetBaseScale(15 * m_Owner->m_Scale.x);

	m_weapon->SetScale({2,2,2});
	// 武器の軌跡色をセット
	m_weapon->SetTrailColor({ 1,0,1,1 });

	attack_kind = (rand() % (KIND_MAX - 1)) + 1; // 攻撃をランダムに設定、以降固定される

	SetProjectile();
	// 武器、影を非表示
	m_weapon->SetLive(false);
	m_Owner->m_Shadow->SetLive(false);

	//DebugUI::RedistDebugFunction([this]() { DebugBossStatus(); });
}

void Enemy::Impl::Update() {
	if (hp <= 0 || notUpdate) { return; };
	//　簡易的なスローモーション処理
	if (m_slowFg) {
		++slow_frame;
		if (slow_frame < slow_rate) {
			return;
		}
		else {
			slow_frame = 0;
		}
	}

	// 目標に向かう
	if (m_lookatFg)
		LookAt(m_target->GetPosition());

	switch (m_Owner->m_State) {
	case NORMAL:
		DistanceMove();
		break;
	case ATTACK:
		AttackUpdate();
		break;
	case STUN:
		StunUpdate();
		break;
	}

	if (inviFg) {
		++invicount;
	}

	if (invicount > 20) {
		inviFg = false;
		invicount = 0;
		m_Owner->SetColor(Vector4(1, 0, 0, 1));
	}

	// 回転していない場合は前方回転を適用
	if (!m_spinFg)
		m_Owner->m_Rotation.y = m_Owner->m_ForwardRotation.y;

	m_Owner->BallUpdate();
	hitwall = false;

	if (m_weapon)
		m_weapon->Update(m_Owner->m_Position, m_Owner->radius, m_Owner->m_Rotation);
}

void Enemy::Impl::Draw()
{
	m_Owner->BallDraw();
}

void Enemy::Impl::Uninit()
{
	m_weapon->AttackEnd();
	m_weapon->StanceEnd();
	m_AngleAnim.Reset();
	m_ArcAnim.Reset();

}

void Enemy::Impl::ReInit() {
	m_Owner->m_live = true;
	m_Owner->m_Velocity_f = 0.0f;//はじめに移動速度を0にする
	hp = maxhp;
	m_Owner->m_Scale.x = 1;
	m_Owner->m_Scale.y = 1;
	m_Owner->m_Scale.z = 1;
	m_Owner->radius *= m_Owner->m_Scale.x;

	m_weapon->SetLive(true);
	m_Owner->m_Shadow->SetLive(true);

	m_Owner->m_State = NORMAL;
	m_weapon->AttackEnd();
	StateReset();
	attack_kind = (rand() % (KIND_MAX - 1)) + 1; // 攻撃をランダムに設定、以降固定される
}

void Enemy::Impl::Attack()
{
	m_Owner->m_State = ATTACK;
	m_Owner->m_Velocity_f = 0;
}

void Enemy::Impl::Damage(int _atk) {
	if (inviFg)  return;
	//防御力分ダメージ軽減
	_atk -= def;
	//ダメージがマイナスにならないように補正
	if (_atk < 0) _atk = 0;

	hp -= _atk;
	invicount = 0;
	inviFg = true;
	//m_Owner->m_Velocity_f = 0.0f;//移動速度を0にする
	m_Owner->SetColor(Vector4(1, 1, 1, 0.5));

	//左から右へ移動するエフェクト再生
	Vector3 pos = m_Owner->m_Position;
	pos = EffectManager::ToCameraEffectPos(pos, m_Owner->radius * m_Owner->m_Scale.x * 1.5f);
	pos -= m_Camera->GetRightVector() * m_Owner->radius;

	//エフェクトパラメーター構造体作成
	EffectParams param;
	param.pos = pos;
	param.pos_amount = { m_Owner->radius * 2, 0 ,0 };
	param.scale = m_Owner->m_Scale * 10;
	param.maxLife = 12;

	// エフェクト再生
	EffectManager::GetInstance()->Play(EFFECT_SLASH, param);

	// SE再生
	Sound::GetInstance()->Play(SOUND_SE_SWORDHIT);
	// ヒットストップ処理
	Game::GetInstance()->HitStop();

	if (hp <= 0)
	  Death();
	
}


void Enemy::Impl::Death()
{
	// 武器、影を非表示
	m_Owner->m_live = false;
	m_weapon->SetLive(false);
	m_Owner->m_Shadow->SetLive(false);
	hp = 0;
	EnemyManager::GetInstance().EnemyDeath();
	for(auto& p : m_projectile)
	{
		p->Reset();
	}
}

void Enemy::Impl::LookAt(Vector3 ta_pos) {

	// atan2を使用して角度を求める
	m_destrot.y = atan2f((ta_pos.x - m_Owner->m_Position.x), (ta_pos.z - m_Owner->m_Position.z));

	// 現在の向きとの差分を計算する
	float fDiffRotY = m_destrot.y - m_Owner->m_ForwardRotation.y;

	// 補正（－１８０～１８０の範囲）
	if (fDiffRotY > PI)
	{
		fDiffRotY -= PI * 2.0f;
	}
	if (fDiffRotY < -PI)
	{
		fDiffRotY += PI * 2.0f;
	}

	// 回転角度計算
	m_Owner->m_ForwardRotation.y += fDiffRotY * m_rotatespeed * 2;
	if (m_Owner->m_ForwardRotation.y > PI)
	{
		m_Owner->m_ForwardRotation.y -= PI * 2.0f;
	}
	if (m_Owner->m_ForwardRotation.y < -PI)
	{
		m_Owner->m_ForwardRotation.y += PI * 2.0f;
	}
}

void Enemy::Impl::AttackUpdate() {
	int weapon_state = m_weapon->GetState();
	switch (attack_kind) {
	case NONE: // 攻撃無し(未指定状態)
		m_attackPhase = AttackPhase::ENTER;
		m_Owner->m_State = NORMAL;
		break;
	case SWING:// 横振り
		// 開始フェーズ
		if (m_attackPhase == AttackPhase::ENTER) {
			m_weapon->Stance();
			m_attackPhase = AttackPhase::PREPARE;
			m_attackframe = 0;
		}

		// 構えフェーズ、一定フレーム経過後、攻撃開始
		if (m_attackPhase == AttackPhase::PREPARE) {
			if (m_weapon->GetStanceTime() > 60) {
				m_weapon->Swing();
				m_attackPhase = AttackPhase::ATTACK;
			}
		}
		// 攻撃フェーズ、攻撃終了後、硬直へ
		if (m_attackPhase == AttackPhase::ATTACK) {
			if (m_weapon->GetMaxAttack()) {
				m_attackPhase = AttackPhase::RECOVER;
			}
		}

		// 硬直フェーズ、攻撃終了後しばらく硬直
		if (m_attackPhase == AttackPhase::RECOVER) {
			if (m_attackframe > 15) {
				m_attackPhase = AttackPhase::END;
			}
			++m_attackframe;
		}

		// 終了フェーズ、終了処理を行う
		if (m_attackPhase == AttackPhase::END) {
			m_weapon->SwingEnd();
			m_Owner->m_State = NORMAL;
			StateReset();
		}
		break;

	case ROTATESWING://回転切り
		RotateSwing();
		break;
	case SWING_VERTICAL_RUSH://縦振り
		SwingVerticalRush();
		break;
	case MANY_THRUST_LOOKAT://連続突き
		ManyThrustLookAt();
		break;
	case THREE_SWING://三連斬り
		ThreeSwing();
		break;
	case JUMP_SPINSLASH://ジャンプ回転切り
		JumpSpinSlash();
		break;
	case JUMP_SPINSLASH_RUSH://ジャンプ回転切り突進
		JumpSpinSlashRush();
		break;
	case SONICBOOM_SHOT: //衝撃波発射
		SonicBoomShot();
		break;
	case WRAPAROUND_THRUST:
		WrapAroundThrust();
		break;
	case JUMP_SPINSLASH_SHOT:
		JumpSpinSlashShot();
		break;
	case CROSS_SHOT:
		CrossShot();
		break;
	case ROTATESWING_FIBONACCI:
		RotateSwingFibonacci();
		break;

	// 分身攻撃は削除
	/*case ALTEREGO_SHOT:
		AlterEgoShot();
		break;
	case ALTEREGO_SPINSLASH:
		AlterEgoSpinSlash();
		break;*/
	case KIND_MAX:
		m_attackPhase = AttackPhase::ENTER;
		m_Owner->m_State = NORMAL;
		break;
	}
}

// 回転切り
void Enemy::Impl::RotateSwing() {
	// 開始フェーズ
	if (m_attackPhase == AttackPhase::ENTER) {
		m_weapon->Stance();
		m_attackPhase = AttackPhase::PREPARE;
	}
	// 構えフェーズ、一定フレーム経過後、攻撃開始
	if (m_attackPhase == AttackPhase::PREPARE) {
		if (m_weapon->GetStanceTime() > 90) {
			m_weapon->AttackStart();
			m_spinFg = true;
			m_attackframe = 0;
			m_attackPhase = AttackPhase::ATTACK;
		}
	}
	// 攻撃フェーズ、回転しながら移動
	if (m_attackPhase == AttackPhase::ATTACK) {
		m_Owner->m_Rotation.y += PI * 0.1f;
		Move();
		++m_attackframe;

		if (m_attackframe % 30 == 0) {
			Sound::GetInstance()->Play(SOUND_SE_ROTATEATTACK);
		}

		if (m_attackframe > 300) {
			m_weapon->AttackEnd();
			m_attackframe = 0;
			m_attackPhase = AttackPhase::RECOVER;
			m_Owner->m_Velocity_f = 0.0f;//移動を停止する
		}
	}

	// 硬直フェーズ、攻撃終了後しばらく硬直
	if (m_attackPhase == AttackPhase::RECOVER) {
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::END;
		}
		++m_attackframe;
	}

	// 終了フェーズ、終了処理を行う
	if (m_attackPhase == AttackPhase::END) {

		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// 突進縦振り
void Enemy::Impl::SwingVerticalRush() {
	// 開始フェーズ
	if (m_attackPhase == AttackPhase::ENTER) {
		m_weapon->Stance_Vertical();
		m_rotatespeed = 0.1f;
		m_attackPhase = AttackPhase::PREPARE;
	}

	// 準備フェーズ、一定フレーム経過後、突進開始
	if (m_attackPhase == AttackPhase::PREPARE) {
		if (m_weapon->GetStanceTime() > 120) {
			m_lookatFg = false;
			m_ta_pos = Game::GetInstance()->GetObjects<Player>()[0]->GetPosition();
			m_ta_pos.y = m_Owner->m_Position.y;//高さはそのまま
			m_rushFg = true;
			m_attackPhase = AttackPhase::ATTACK;
		}

	}

	// 攻撃フェーズ、ターゲットに向かって突進
	if (m_attackPhase == AttackPhase::ATTACK) {
		Move();
		if (m_stateframe > 3) {
			// 土煙エフェクト再生
			EffectParams   param;
			param.pos = m_Owner->m_Position;
			param.scale = m_Owner->m_Scale * 20;
			param.maxLife = 30;
			EffectManager::GetInstance()->Play(EFFECT_TUTIKEMURI, param);
			m_stateframe = 0;
		}
		++m_attackframe;

		//近づくか、壁に当たったら振る
		if (fabs(m_Owner->m_Position.x - m_ta_pos.x) < m_Owner->radius * 2 &&
			fabs(m_Owner->m_Position.z - m_ta_pos.z) < m_Owner->radius * 2 || hitwall) {
			m_weapon->Swing_Vertical();
			m_rushFg = false;
			m_Owner->m_Velocity_f = 0.0f;//移動速度を0にする
			m_attackframe = 0;
			m_attackPhase = AttackPhase::FOLLOW;
			m_AngleAnim.StartAbsolute({ -PI * 0.3f,0,0 }, { PI * 0.1f,0,0 }, 18, 1.0f);
		}

	}

	if (m_attackPhase == AttackPhase::FOLLOW) {
		m_Owner->m_Rotation = m_Owner->m_ForwardRotation + m_AngleAnim.UpdateAbsolute();
		// 大きな土煙エフェクト再生
		if (m_weapon->GetMaxAttack()) {
			//エフェクトパラメーター構造体作成
			EffectParams param;
			param.scale = m_Owner->m_Scale * 15;
			param.maxLife = 60;
			// エフェクト再生
			m_weapon->TipToEffect(EFFECT_TUTIKEMURI_BIG, param);
			Sound::GetInstance()->Play(SOUND_SE_SWINGVERTICAL);
			m_attackPhase = AttackPhase::RECOVER;
			m_attackcount = 0;
			m_Owner->m_Rotation.x = 0.0f;//回転リセット
		}
	}

	if (m_attackPhase == AttackPhase::RECOVER) {
		if (m_attackframe > 30) {
			m_attackPhase = AttackPhase::END;
		}
		++m_attackframe;
	}

	if (m_attackPhase == AttackPhase::END) {

		m_weapon->SwingEnd();
		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// 連続突き移動
void Enemy::Impl::ManyThrustLookAt() {
	// 開始フェーズ
	if (m_attackPhase == AttackPhase::ENTER) {
		m_weapon->Stance_Thrust();
		m_attackPhase = AttackPhase::PREPARE;
		m_attackcount = 0;
	}

	// 準備フェーズ
	if (m_attackPhase == AttackPhase::PREPARE) {
		// 一定フレーム構えた後、攻撃開始
		if (m_weapon->GetStanceTime() > 90) {
			m_attackPhase = AttackPhase::ATTACK;
			m_weapon->StanceEnd();
			m_weapon->Thrust();
			m_attackcount++;
			Sound::GetInstance()->Play(SOUND_SE_SWING);
		}
	}


	if (m_attackPhase == AttackPhase::ATTACK) {
		Move();
		// 攻撃終了するか
		if (ManyThrust(40)) {//連続突きを行い、40回以上行っていたらtrueを返す
			m_attackPhase = AttackPhase::RECOVER;
			m_weapon->ThrustEnd();
			m_Owner->m_Velocity_f = 0.0f;//移動停止
			m_attackframe = 0;
		}
	}

	if (m_attackPhase == AttackPhase::RECOVER) {
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::END;
		}
		++m_attackframe;
	}

	if (m_attackPhase == AttackPhase::END) {

		m_Owner->m_State = NORMAL;
		StateReset();
	}

}

// 三連斬り
void Enemy::Impl::ThreeSwing() {
	// 攻撃開始処理、構えに入る
	if (m_attackPhase == AttackPhase::ENTER) {
		m_weapon->Stance();
		m_rotatespeed = 0.1f;
		m_lookatFg = true;
		m_rushFg = true;
		m_attackPhase = AttackPhase::PREPARE;
	}

	// 準備フェーズ、構えが完了したら攻撃開始、三回攻撃を繰り返す
	if (m_attackPhase == AttackPhase::PREPARE) {
		// しばらく構えた後、攻撃に以降
		if (m_weapon->GetStanceTime() > 60) {
			if (m_attackcount == 0) {
				m_weapon->Swing();
				m_AngleAnim.StartAbsolute({ 0, -PI * 0.3f,0 }, { 0, PI * 0.3f,0 }, 18, 0.7f);
				Sound::GetInstance()->Play(SOUND_SE_SWING);
			}
			else if (m_attackcount == 1) {
				m_weapon->Swing_Return();
				m_AngleAnim.StartAbsolute({ 0, PI * 0.3f,0 }, { 0, -PI * 0.3f,0 }, 18, 0.7f);
				Sound::GetInstance()->Play(SOUND_SE_SWING);
			}
			else if (m_attackcount == 2) {
				m_weapon->Swing(18, SwingMode::VERTICAL);
				m_AngleAnim.StartAbsolute({ -PI * 0.3f,0,0 }, { PI * 0.1f,0,0 }, 18, 1.0f);
				Sound::GetInstance()->Play(SOUND_SE_SWING);
			}
			m_attackPhase = AttackPhase::ATTACK;
			m_spinFg = true;
			m_lookatFg = false;
			++m_attackcount;
		}
	}

	// 攻撃フェーズ、攻撃後準備フェーズに戻る、三回攻撃を繰り返す
	if (m_attackPhase == AttackPhase::ATTACK) {
		if (m_weapon->GetMaxAttack()) {
			m_weapon->SwingEnd();

			if (m_attackcount == 1) {
				m_weapon->Stance_Return();
			}
			else if (m_attackcount == 2) {
				m_weapon->Stance(10, StanceMode::VERTICAL);
			}
			m_lookatFg = true;
			m_Owner->m_Velocity_f = 0.0f;//移動速度を0にする
			m_attackPhase = AttackPhase::FOLLOW;
		}
		else { // 攻撃していない間は移動する
			Move();
			m_Owner->m_Rotation = m_Owner->m_ForwardRotation + m_AngleAnim.UpdateAbsolute();// アニメーション更新、絶対値と前進方向を合わせて算出する
		}

	}

	// 攻撃後処理、三回攻撃したら終了、そうでなければ準備フェーズに戻る
	if (m_attackPhase == AttackPhase::FOLLOW) {
		// 三回攻撃したら終了
		if (m_attackcount >= 3) {
			m_lookatFg = false;
			m_attackPhase = AttackPhase::RECOVER;
		}
		else {
			m_attackPhase = AttackPhase::PREPARE;
		}
		m_spinFg = false;
		m_Owner->m_Rotation.x = 0.0f;//回転リセット
	}

	// 硬直フェーズ、終了フェーズに以降するまで硬直する
	if (m_attackPhase == AttackPhase::RECOVER) {
		// 3回目の攻撃後、しばらくそのままで待機してから終了
		if (m_attackframe > 90) {
			m_attackPhase = AttackPhase::END;
		}
		++m_attackframe;
	}

	// 終了フェーズ、終了処理を行う
	if (m_attackPhase == AttackPhase::END) {

		m_weapon->StanceEnd();
		m_Owner->m_State = NORMAL;
		StateReset();
	}

}

// ジャンプ回転切り
void Enemy::Impl::JumpSpinSlash() {
	// ジャンプしていなければジャンプする
	if (m_attackPhase == AttackPhase::ENTER) {
		Jump();
		m_rushFg = true;
		move_speed = 0.1f;// 移動速度を下げる
		m_attackPhase = AttackPhase::PREPARE;
	}

	// 準備フェーズ
	if (m_attackPhase == AttackPhase::PREPARE) {
		if (m_attackframe > 30) {
			m_attackPhase = AttackPhase::ATTACK;
			m_attackframe = 0;
			m_Owner->m_Velocity_f = 0;//移動速度を0にする
			m_rushFg = false;
		}
		++m_attackframe;

	}

	if (m_attackPhase == AttackPhase::ATTACK) {
		m_Owner->m_Velocity.y = 0;//空中で停止する
		// 攻撃開始
		if (m_weapon->GetState() == Sword::STATE::NORMAL) {
			m_weapon->AttackStart(true, true);
			m_AngleAnim.StartRelative({ PI * 4,0,0 }, 30, 0.0f);// 縦回転切り
			Sound::GetInstance()->Play(SOUND_SE_SWING);
			++m_attackcount;
		}

		m_Owner->m_Rotation += m_AngleAnim.UpdateRelative();// 回転切りアニメーション更新

		if (!m_AngleAnim.IsPlaying()) {
			m_Owner->m_Rotation.x = 0;
			m_weapon->AttackEnd();
			m_weapon->Stance(15, StanceMode::VERTICAL);
			m_attackPhase = AttackPhase::FOLLOW;
		}

	}

	// 追撃フェーズ
	if (m_attackPhase == AttackPhase::FOLLOW) {
		if (m_weapon->GetStanceTime() > 90) {
			m_weapon->StanceEnd();
			// 攻撃開始
			m_weapon->AttackStart(true, true);
			Vector3 endrot = m_Owner->m_Rotation;
			endrot.x += PI * 2;
			m_AngleAnim.StartAbsolute(m_Owner->m_Rotation, endrot, 10, 0.0f);// 縦回転切り、絶対値参照で行う
			Sound::GetInstance()->Play(SOUND_SE_SWING);
			m_Owner->m_Velocity.y = -0.5f;//落下開始
			move_speed = 0.25f;// 移動速度を元に戻す
			m_rushFg = true;
			++m_attackcount;
		}

		if (m_attackcount > 1) {
			m_Owner->m_Rotation.x = m_AngleAnim.UpdateAbsolute().x;// 回転切りアニメーション更新、絶対値参照
		}
		else {
			m_Owner->m_Velocity.y = 0;//空中で停止する
		}

	}

	// 攻撃していないときのみ移動する
	if (m_attackPhase == AttackPhase::PREPARE || m_rushFg) {
		Move();
	}

	// 攻撃終了
	if (m_Owner->is_GROUND && m_rushFg) {
		m_attackPhase = AttackPhase::END;
	}

	// 攻撃終了処理
	if (m_attackPhase == AttackPhase::END) {
		
		m_weapon->AttackEnd();
		m_Owner->m_State = NORMAL;
		StateReset();
	}

}

// ジャンプ回転切り突進
void Enemy::Impl::JumpSpinSlashRush() {
	// ジャンプしていなければジャンプする
	if (m_attackPhase == AttackPhase::ENTER) {
		Jump();
		m_attackPhase = AttackPhase::PREPARE;
	}

	// 準備フェーズ
	if (m_attackPhase == AttackPhase::PREPARE) {
		if (m_attackframe > 30) {
			m_attackPhase = AttackPhase::ATTACK;
			m_attackframe = 0;
		}
		++m_attackframe;

	}

	if (m_attackPhase == AttackPhase::ATTACK) {
		m_Owner->m_Velocity.y = 0;//空中で停止する
		// 攻撃開始
		if (m_weapon->GetState() == Sword::STATE::NORMAL) {
			m_weapon->AttackStart(true, true);
	/*		Vector3 endrot = m_Owner->m_Rotation;
			endrot.x += PI * 8;*/
			m_AngleAnim.StartAbsolute({ 0,0,0 }, { PI * 8 ,0,0}, 30, 0.0f);// 縦回転切り、絶対値参照で行う
			m_rushFg = true;
			Sound::GetInstance()->Play(SOUND_SE_SWING);
			m_lookatFg = false;
		}

		m_Owner->m_Rotation.x = m_AngleAnim.UpdateAbsolute().x;// 回転切りアニメーション更新、絶対値参照

		//回転終了後、硬直フェーズへ。ここから着地まで何もしない
		if (!m_AngleAnim.IsPlaying() || hitwall) {
			m_Owner->m_Rotation.x = 0;
			m_weapon->AttackEnd();
			m_attackPhase = AttackPhase::RECOVER;
			m_rushFg = false;
		}

	}

	// 突進フラグが有効なら移動する
	if (m_rushFg) {
		Move();
	}

	// 攻撃終了
	if (m_Owner->is_GROUND) {
		m_attackPhase = AttackPhase::END;
	}

	// 攻撃終了処理
	if (m_attackPhase == AttackPhase::END) {
		m_weapon->AttackEnd();

		m_weapon->AttackEnd();
		m_Owner->m_State = NORMAL;
		StateReset();
	}

}

// 衝撃波発射
void  Enemy::Impl::SonicBoomShot() {
	if (m_attackPhase == AttackPhase::ENTER) {
		m_rand = rand() % 2;
		// 衝撃波の構え、ランダムでモーション分岐
		if (m_rand == 1) {
			ProjectileCharge();
			m_weapon->Stance();
		}
		else {
			ProjectileCharge_VT();
			m_weapon->Stance(18, StanceMode::VERTICAL);
		}
		m_attackcount = 0;
		m_attackframe = 0;
		m_rotatespeed = 0.3f;
		m_attackPhase = AttackPhase::PREPARE;
	}

	if (m_attackPhase == AttackPhase::PREPARE) {
		++m_attackframe;
		if (m_attackframe > 150) {
			m_attackPhase = AttackPhase::ATTACK;
		}
	}

	if (m_attackPhase == AttackPhase::ATTACK) {
		// 衝撃波発射
		m_lookatFg = false;
		ProjectileShot();
		// モーション分岐、構え時から乱数を変えていないので構えに応じた振り方になる
		if (m_rand == 1) {
			m_weapon->Swing();
			//m_AngleAnim.StartAbsolute({ 0, -PI * 0.3f,0 }, { 0, PI * 0.3f,0 }, 18, 0.7f);
		}
		else {
			m_weapon->Swing_Vertical();
			//m_AngleAnim.StartAbsolute({ -PI * 0.3f,0,0 }, { PI * 0.1f,0,0 }, 18, 1.0f);
		}
		m_attackcount++;
		m_attackframe = 0;
		// 3回発射したら硬直へ、そうでなければ次の攻撃の構えに移る
		if (m_attackcount >= 3) {
			m_attackPhase = AttackPhase::RECOVER;
		}
		else {
			m_attackPhase = AttackPhase::FOLLOW;
		}
		Sound::GetInstance()->Play(SOUND_SE_SWING);
	}

	if (m_attackPhase == AttackPhase::FOLLOW)
	{
		++m_attackframe;
		//if (m_AngleAnim.IsPlaying()) {
		//	m_Owner->m_Rotation = m_Owner->m_ForwardRotation + m_AngleAnim.UpdateAbsolute();// アニメーション更新、絶対値と前進方向を合わせて算出する
		//}
		if (m_weapon->GetMaxAttack()) {
			m_lookatFg = true;
			m_weapon->SwingEnd();
			m_rand = rand() % 2;
			// 次の衝撃波の構え、ランダムでモーション分岐
			if (m_rand == 1) {
				m_weapon->Stance();
				ProjectileChargeMax();
			}
			else {
				m_weapon->Stance(18, StanceMode::VERTICAL);
				ProjectileChargeMax_VT();
			}
		}
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::ATTACK;
		}
	}

	if (m_attackPhase == AttackPhase::RECOVER)
	{
		++m_attackframe;
		if (m_attackframe > 90) {
			m_attackPhase = AttackPhase::END;
		}
	}

	if (m_attackPhase == AttackPhase::END)
	{
		m_weapon->SwingEnd();
		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// 回り込み→連続突き
void Enemy::Impl::WrapAroundThrust() {
	// 開始フェーズ
	if (m_attackPhase == AttackPhase::ENTER) {
		m_weapon->Stance_Vertical();
		m_rotatespeed = 0.1f;
		m_Owner->m_Velocity_f = 0.0f;//速度を0にする
		m_attackPhase = AttackPhase::PREPARE;
		m_lookatFg = true;
	}

	// 準備フェーズ、一定フレーム経過後、突進開始
	if (m_attackPhase == AttackPhase::PREPARE) {
		if (m_weapon->GetStanceTime() > 120) {
			m_rushFg = true;
			m_attackPhase = AttackPhase::ATTACK;
		}

	}
	// 攻撃フェーズ、ターゲットに向かって突進し、近づいたら回り込む
	if (m_attackPhase == AttackPhase::ATTACK) {
		Move();
		m_ta_pos = Game::GetInstance()->GetObjects<Player>()[0]->GetPosition();
		m_ta_pos.y = m_Owner->m_Position.y;//高さはそのまま
		if (m_attackframe % 3) {
			// 土煙エフェクト再生
			EffectParams   param;
			param.pos = m_Owner->m_Position;
			param.scale = m_Owner->m_Scale * 20;
			param.maxLife = 30;
			EffectManager::GetInstance()->Play(EFFECT_TUTIKEMURI, param);
			m_attackframe = 0;
		}
		++m_attackframe;

		//近づいたら回り込む
		if (fabs(m_Owner->m_Position.x - m_ta_pos.x) < m_Owner->radius * 3 &&
			fabs(m_Owner->m_Position.z - m_ta_pos.z) < m_Owner->radius * 3) {
			float rot_y = m_Owner->m_Rotation.y;
			m_Owner->is_SPECIALMOVE = true;// 特殊移動ON
			rot_y -= PI;
			m_rotatespeed = 0.3f;
			m_ArcAnim.Start(m_Owner->m_Rotation.y, rot_y, m_Owner->radius * 3, 60, 0.7f);
			m_attackPhase = AttackPhase::FOLLOW;
			m_attackframe = 0;
			m_Owner->m_Velocity_f = 0.0f;//移動停止
		}
	}

	// 追撃フェーズ、回り込んでから連続突きを行う
	if (m_attackPhase == AttackPhase::FOLLOW)
	{
		if (!m_ArcAnim.IsPlaying()) {
			m_Owner->is_SPECIALMOVE = false;// 特殊移動OFF
			m_lookatFg = false;
			Move();

			// 攻撃終了するか
			if (ManyThrust(6)) {//連続突きを行い、6回以上行っていたらtrueを返す
				m_attackPhase = AttackPhase::RECOVER;
				m_weapon->ThrustEnd();
				m_Owner->m_Velocity_f = 0.0f;//移動停止
				m_attackframe = 0;
			}
		}
		else {
			m_Owner->m_Velocity = m_ArcAnim.Update();
			++m_attackframe;
			if (m_attackframe % 3 == 0) {
				// 残像エフェクト再生
				EffectParams  param;
				param.pos = m_Owner->m_Position;
				param.rot = m_Owner->m_Rotation;
				param.scale = m_Owner->m_Scale;
				param.maxLife = 10;
				param.color = { 1,0.2f,0,0.3f };
				EffectManager::GetInstance()->Play(EFFECT_PLAYER, param);
			}

		}
	}
	// 硬直フェーズ、終了フェーズに以降するまで硬直する
	if (m_attackPhase == AttackPhase::RECOVER) {

		++m_attackframe;
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::END;
		}

	}
	// 終了フェーズ、終了処理を行う
	if (m_attackPhase == AttackPhase::END) {

		m_weapon->SwingEnd();
		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// ジャンプ回転切り→衝撃波飛ばし
void Enemy::Impl::JumpSpinSlashShot()
{
	// ジャンプしていなければジャンプする
	if (m_attackPhase == AttackPhase::ENTER) {
		Jump(2.0f);
		m_Owner->m_Velocity_f = 0;//移動速度を0にする
		m_attackPhase = AttackPhase::PREPARE;
	}

	// 準備フェーズ
	if (m_attackPhase == AttackPhase::PREPARE) {
		if (m_attackframe > 30) {
			m_attackPhase = AttackPhase::ATTACK;
			m_attackframe = 0;
			m_Owner->m_Velocity_f = 0;//移動速度を0にする
		}
		++m_attackframe;

	}

	if (m_attackPhase == AttackPhase::ATTACK) {
		// 攻撃開始
		if (m_weapon->GetState() == Sword::STATE::NORMAL) {
			m_weapon->AttackStart(true, true);
			Vector3 endrot = m_Owner->m_Rotation;
			endrot.x += PI * 36;
			m_AngleAnim.StartAbsolute(m_Owner->m_Rotation, endrot, 120, 0.0f);// 縦回転切り、絶対値参照で行う
			Sound::GetInstance()->Play(SOUND_SE_SWING);
		}

		m_Owner->m_Rotation.x = m_AngleAnim.UpdateAbsolute().x;// 回転切りアニメーション更新、絶対値参照

		// 着地したら回転終了して衝撃波発射、硬直へ
		if (m_Owner->is_GROUND) {
			m_Owner->m_Rotation.x = 0;
			m_weapon->AttackEnd();
			ProjectileChargeMax_VT();
			ProjectileShot();
			m_attackPhase = AttackPhase::RECOVER;
		}

	}

	// 硬直フェーズ、一定フレーム経過後終了へ
	if (m_attackPhase == AttackPhase::RECOVER)
	{
		++m_attackframe;
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::END;
		}
	}

	// 終了フェーズ、攻撃終了処理を行う
	if (m_attackPhase == AttackPhase::END)
	{

		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// 十字衝撃波発射
void Enemy::Impl::CrossShot() {
	// 開始フェーズ
	if (m_attackPhase == AttackPhase::ENTER) {
		// 十字衝撃波の構え、縦と横両方の衝撃波をチャージする
		m_rotatespeed = 0.3f;
		ProjectileCharge();
		ProjectileCharge_VT();
		m_weapon->Stance(18, StanceMode::VERTICAL);
		m_attackPhase = AttackPhase::PREPARE;
	}
	// 準備フェーズ、一定フレーム経過後、攻撃フェーズに
	if (m_attackPhase == AttackPhase::PREPARE) {
		++m_attackframe;
		if (m_attackframe > 120) {
			m_attackPhase = AttackPhase::ATTACK;
			m_attackframe = 0;
		}
	}
	// 攻撃フェーズ、十字衝撃波発射
	if (m_attackPhase == AttackPhase::ATTACK) {
		m_lookatFg = false;
		// 十字衝撃波発射
		ProjectileShot();
		ProjectileShot();
		m_weapon->Swing_Vertical();
		Sound::GetInstance()->Play(SOUND_SE_SWING);
		m_attackPhase = AttackPhase::FOLLOW;
		m_attackframe = 0;
		++m_attackcount;
		// 2回発射したら硬直へ
		if (m_attackcount >= 2) {
			m_attackPhase = AttackPhase::RECOVER;
		}
	}

	if (m_attackPhase == AttackPhase::FOLLOW)
	{
		++m_attackframe;
		if (m_weapon->GetMaxAttack()) {
			m_lookatFg = true;
			m_weapon->SwingEnd();
			ProjectileChargeMax();
			ProjectileChargeMax_VT();
			m_weapon->Stance(18, StanceMode::VERTICAL);
		}
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::ATTACK;
		}
	}

	// 硬直フェーズ、一定フレーム経過後終了へ
	if (m_attackPhase == AttackPhase::RECOVER) {
		++m_attackframe;
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::END;
		}
	}

	// 終了フェーズ、攻撃終了処理を行う
	if (m_attackPhase == AttackPhase::END) {

		m_weapon->SwingEnd();
		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// 回転斬りフィボナッチ数列軌道
void Enemy::Impl::RotateSwingFibonacci()
{
	// 開始フェーズ
	if (m_attackPhase == AttackPhase::ENTER) {
		m_weapon->Stance();
		m_attackPhase = AttackPhase::PREPARE;
	}
	// 構えフェーズ、一定フレーム経過後、攻撃開始
	if (m_attackPhase == AttackPhase::PREPARE) {
		if (m_weapon->GetStanceTime() > 90) {
			m_lookatFg = false;
			m_spinFg = true;
			m_Owner->is_SPECIALMOVE = true;// 特殊移動モードにする
			m_weapon->AttackStart();
			m_FiboAnim.Start(0, PI * 2, m_Owner->radius * 0.75f, 120, 1.0f);
			m_attackframe = 0;
			m_attackPhase = AttackPhase::ATTACK;
		}
	}

	// 攻撃フェーズ、回転しながら移動
	if (m_attackPhase == AttackPhase::ATTACK) {
		m_Owner->m_Rotation.y += PI * 0.2f;

		Matrix rot = Matrix::CreateRotationY(m_Owner->m_ForwardRotation.y);

		// 平行移動を含まない回転なので TransformNormal
		Vector3 worldVelocity = Vector3::TransformNormal(m_FiboAnim.Update(), rot);// フィボナッチ数列軌道に沿って移動、前進方向に合わせて速度を回転させる

		// フィボナッチ数列軌道に沿って移動、前進方向に合わせて速度を回転させる
		m_Owner->m_Velocity = worldVelocity;

		++m_attackframe;

		if (m_attackframe % 30 == 0) {
			Sound::GetInstance()->Play(SOUND_SE_ROTATEATTACK);
		}

		if (!m_FiboAnim.IsPlaying()) {
			m_weapon->AttackEnd();
			m_attackframe = 0;
			m_attackPhase = AttackPhase::RECOVER;
			m_Owner->m_Velocity_f = 0.0f;//移動を停止する
			m_Owner->is_SPECIALMOVE = false; // 特殊移動モード解除
		}
	}

	// 硬直フェーズ、攻撃終了後しばらく硬直
	if (m_attackPhase == AttackPhase::RECOVER) {
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::END;
		}
		++m_attackframe;
	}

	// 終了フェーズ、終了処理を行う
	if (m_attackPhase == AttackPhase::END) {
		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// 分身して衝撃波発射
void Enemy::Impl::AlterEgoShot()
{
	// 開始フェーズ
	if (m_attackPhase == AttackPhase::ENTER) {
		// 分身衝撃波の構え、縦の衝撃波をチャージする
		m_rotatespeed = 0.3f;
		ProjectileCharge_VT();
		m_weapon->Stance(18, StanceMode::VERTICAL);
		m_startpos = m_Owner->m_Position;
		m_vib.Start(20, PI * 0.5f);//振動開始、振れ幅を大きくして分身っぽく見せる
		m_Owner->is_SPECIALMOVE = true;
		m_Owner->m_Shadow->SetLive(false);// 分身中は影を消す
		m_attackPhase = AttackPhase::PREPARE;
	}
	// 準備フェーズ、一定フレーム経過後、攻撃フェーズに
	if (m_attackPhase == AttackPhase::PREPARE) {
		m_Owner->m_Position = m_startpos;// 元の位置に戻す
		Vector3 forward;
		forward.x = sinf(m_Owner->m_ForwardRotation.y);
		forward.y = 0.0f;
		forward.z = cosf(m_Owner->m_ForwardRotation.y);

		m_Owner->m_Velocity = m_vib.UpdateMoveDir(forward);

		++m_attackframe;

		if (m_attackframe > 120) {
			m_Owner->m_Position = m_startpos;// 元の位置に戻す
			// 左右に弾を発射する準備
			ProjectileChargeMax_VT(20.0f, PI);
			ProjectileChargeMax_VT(-20.0f, PI);
			m_attackPhase = AttackPhase::ATTACK;
			m_attackframe = 0;
			m_Owner->is_SPECIALMOVE = false;
			m_Owner->m_Velocity_f = 0;
		}
	}

	// 攻撃フェーズ、三発同時に縦衝撃波発射
	if (m_attackPhase == AttackPhase::ATTACK) {
		m_lookatFg = false;
		// 三方向衝撃波発射、位置を正すために更新も行う
		for (auto& p : m_projectile) {
			p->Update();
		}
		// 衝撃波発射
		ProjectileShot_All();
		m_weapon->Swing_Vertical();
		//m_AngleAnim.StartAbsolute({ -PI * 0.3f,0,0 }, { PI * 0.1f,0,0 }, 18, 1.0f);
		Sound::GetInstance()->Play(SOUND_SE_SWING);
		m_attackPhase = AttackPhase::FOLLOW;
		m_attackframe = 0;
		++m_attackcount;
		m_attackPhase = AttackPhase::RECOVER;
		m_Owner->m_Shadow->SetLive(true); // 分身終了、影を戻す
	}

	// 硬直フェーズ、一定フレーム経過後終了へ
	if (m_attackPhase == AttackPhase::RECOVER) {
		//if (m_AngleAnim.IsPlaying()) {
		//	m_Owner->m_Rotation = m_Owner->m_ForwardRotation + m_AngleAnim.UpdateAbsolute();// アニメーション更新、絶対値と前進方向を合わせて算出する
		//}
		++m_attackframe;
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::END;
		}
	}

	// 終了フェーズ、攻撃終了処理を行う
	if (m_attackPhase == AttackPhase::END) {
		//m_attackframe = 0;
		//m_attackcount = 0;
		//m_rotatespeed = 0.01f;
		//attack_kind = NONE;//攻撃終了
		//m_lookatFg = true;
		//m_attackPhase = AttackPhase::ENTER;

		m_weapon->SwingEnd();
		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// 分身の後、回転斬り突進
void Enemy::Impl::AlterEgoSpinSlash()
{
	// 開始フェーズ
	if (m_attackPhase == AttackPhase::ENTER) {
		m_vib.Start(20, PI * 0.5f);//振動開始、振れ幅を大きくして分身っぽく見せる
		m_Owner->is_SPECIALMOVE = true;
		m_attackPhase = AttackPhase::PREPARE;
		m_Owner->m_Velocity_f = 0.0f;
		m_attackframe = 0;
		m_weapon->Stance();
		m_rand = rand() % 4;
		m_Owner->m_Shadow->SetLive(false);// 分身中は影を消す
	}
	// 準備フェーズ、一定フレーム経過後、攻撃フェーズに
	if (m_attackPhase == AttackPhase::PREPARE) {
		m_Owner->m_Velocity = m_vib.Update();
		m_Owner->m_Velocity.y = 0.0f;//高さは変えない

		++m_attackframe;
		if (m_attackframe == 120 + m_rand) {
			m_lookatFg = false;
			m_Owner->is_SPECIALMOVE = false;
			m_Owner->m_Shadow->SetLive(true);
		}

		if (m_attackframe >= 150 + m_rand) {
			m_attackPhase = AttackPhase::ATTACK;
			m_attackframe = 0;
			m_rushFg = true;
			m_spinFg = true;
			m_weapon->AttackStart();
			m_AngleAnim.StartRelative({ 0,PI * 8,0 }, 60, 0.0f);// 回転切り
			Sound::GetInstance()->Play(SOUND_SE_SWING);
		}
	}
	// 攻撃フェーズ、回転切りを行う
	if (m_attackPhase == AttackPhase::ATTACK) {
		Move();
		++m_attackframe;

		//m_Owner->m_Rotation.y += PI * 0.2f;
		m_Owner->m_Rotation += m_AngleAnim.UpdateRelative();// 回転切りアニメーション更新

		if (m_attackframe > 90) {
			m_spinFg = false;
			m_weapon->AttackEnd();
			m_attackPhase = AttackPhase::RECOVER;
		}
	}

	if (m_attackPhase == AttackPhase::RECOVER) {
		++m_attackframe;
		if (m_attackframe > 60) {
			m_attackPhase = AttackPhase::END;
		}
	}

	// 終了フェーズ、攻撃終了処理を行う
	if (m_attackPhase == AttackPhase::END) {
		//m_Owner->m_State = NORMAL;
		//m_stateframe = 0;
		//m_attackframe = 0;
		//m_attackcount = 0;
		//m_rushFg = false;
		//m_lookatFg = true;
		//m_attackPhase = AttackPhase::ENTER;

		m_Owner->m_State = NORMAL;
		StateReset();
	}
}

// 行動不能状態更新
void Enemy::Impl::StunUpdate()
{
	if (m_stateframe > 10) {
		m_Owner->m_Velocity_f = 0.0f;//10フレーム経過後ノックバック停止
	}

	Vector3 forward;
	forward.x = sinf(m_Owner->m_ForwardRotation.y);
	forward.y = 0.0f;
	forward.z = cosf(m_Owner->m_ForwardRotation.y);

	//振動させる
	m_Owner->m_Position += m_vib.UpdateMoveDir(forward);

	++m_stateframe;

	// 終了処理
	if (m_stateframe > 180) {
		m_Owner->m_State = NORMAL;
		m_stateframe = 0;
		m_Owner->m_Rotation.x = 0;
		m_weapon->StanceEnd();
		m_lookatFg = true;//lookat復活
	}
}

// 行動不能状態にする
void Enemy::Impl::Stun()
{
	m_Owner->is_notUpdate = false;//動きを再開する
	StateReset();
	m_lookatFg = false;// lookat無効
	m_Owner->m_Velocity_f = -2.0f;//後ろにノックバックする
	m_vib.Start(1.0f, PI * 0.5f);//振動開始
	m_weapon->AttackEnd();
	m_weapon->Stance(10, StanceMode::VERTICAL);// 縦に構える
	m_Owner->m_State = STUN;
	m_Owner->m_Rotation.y = m_Owner->m_ForwardRotation.y;
	/*if (knockbackDir) {
		m_Owner->m_ForwardRotation.y = knockbackDir.value().y;
		m_Owner->m_Rotation.y = m_Owner->m_ForwardRotation.y;

	}*/
	m_Owner->m_Rotation.x -= PI / 8;//少し上に仰け反る
}

// スタン時等の際のリセット処理
void Enemy::Impl::StateReset() {
	m_Owner->m_Rotation.x = 0;
	m_attackcount = 0;
	m_stateframe = 0;
	m_attackframe = 0;
	m_lookatFg = true;//lookatを戻す
	m_rushFg = false;
	m_spinFg = false; // 回転状態フラグ解除
	m_Owner->is_SPECIALMOVE = false;// 特殊移動解除
	move_speed = 0.1f;// 移動速度を戻す
	m_attackPhase = AttackPhase::ENTER;// 攻撃フェーズ初期化
}

void Enemy::Impl::ShotBullet() {
	int r1 = rand();
	int r2 = rand();
	Vector3 ta_pos = Game::GetInstance()->GetObjects<Player>()[0]->GetPosition();
	Vector3 ta_addpos = Vector3((r1 % 20) + 20.0f, 0.0f, (r2 % 20) + 20.0f);
	if (r1 % 2 == 0) { ta_addpos.x *= -1; };
	if (r2 % 2 == 0) { ta_addpos.z *= -1; };
	if (!m_bullet.size()) {
		vector<Bullet*> bullet = Game::GetInstance()->GetObjects<Bullet>();
		m_bullet = bullet;
	}
	for (auto& bu : m_bullet) {
		if (!bu->GetLive()) {
			bu->LookAtShot(ta_pos + ta_addpos, ta_pos, false);
			Sound::GetInstance()->Play(SOUND_SE_ARROWSHOT);
			break;
		}
	}

}

// 距離を取る移動処理
void Enemy::Impl::DistanceMove() {

	Vector3 forwardDir;
	forwardDir.x = sinf(m_Owner->m_ForwardRotation.y);
	forwardDir.z = cosf(m_Owner->m_ForwardRotation.y);
	forwardDir.y = 0.0f;
	forwardDir.Normalize();

	// 敵から離れる方向を取得
	Vector3 awayDir;
	m_awayFg = EnemyManager::GetInstance().NearDistance(m_Owner, &awayDir);

	constexpr float AVOID_RATE = 0.2f; // 0～1 小さいほどゆっくり回る

	Vector3 blendedDir = forwardDir;

	if (m_awayFg)
	{
		blendedDir = Vector3::Lerp(forwardDir, awayDir, AVOID_RATE);
		blendedDir.Normalize();

		m_Owner->m_ForwardRotation.y = atan2f(blendedDir.x, blendedDir.z);
	}
	Move();

}

// 移動処理
void Enemy::Impl::Move() {
	if (m_rushFg) {
		m_Owner->m_Velocity_f = move_speed * 8;
		//m_Owner->m_Rotation.x += 0.2f;
	}
	else {
		m_Owner->m_Velocity_f = move_speed;
		//m_Owner->m_Rotation.x += 0.025f;
	}

	if (m_Owner->m_Rotation.x > PI * 2) m_Owner->m_Rotation.x -= PI * 2;

}

// 弾のセットアップ
void Enemy::Impl::SetProjectile() {
	for (int i = 0; i < projectile_max; ++i) {
		m_projectile.emplace_back(Game::GetInstance()->AddObject<Projectile>());
		m_projectile.back()->SetPl(false);
		m_projectile.back()->SetOwner(m_Owner);
		m_projectile.back()->SetColor({ 0,1,0,1 });
	}
}

// 飛び道具のチャージ開始
void Enemy::Impl::ProjectileCharge(float _offset, float _angle) {
	for (auto& pr : m_projectile)
	{
		if (pr->GetState() == ProjectileSTATE::NOT_ACTIVE) {
			bool follow = true;
			Vector3 pos = m_Owner->m_Position;
			// 現在位置からオフセット分ずらした位置にセット、ずらす方向も指定できる 
			if (_offset != 0) {
				//位置計算
				Vector3 rotOffset;

				// Yaw + Pitch 回転
				rotOffset.x = _offset * cosf(m_Owner->m_Rotation.y) + m_Owner->radius * 2 * sinf(m_Owner->m_Rotation.y);

				rotOffset.y = m_Owner->radius * -0.5f;

				rotOffset.z = -_offset * sinf(m_Owner->m_Rotation.y) + m_Owner->radius * 2 * cosf(m_Owner->m_Rotation.y);

				pos += rotOffset;
				follow = false;//　オフセット指定がある場合は追尾しない
			}

			pr->SetPl(false);
			pr->ChargeStart(m_Owner->m_Position, m_Owner->m_Rotation, 1.0f, follow);
			pr->SetOffset({ 0 ,m_Owner->radius * -0.5f ,m_Owner->radius * 2 });
			break;
		}
	}
}

// 飛び道具のチャージ開始、縦
void Enemy::Impl::ProjectileCharge_VT(float _offset, float _angle) {
	for (auto& pr : m_projectile)
	{
		if (pr->GetState() == ProjectileSTATE::NOT_ACTIVE) {
			bool follow = true;
			Vector3 pos = m_Owner->m_Position;
			// 現在位置からオフセット分ずらした位置にセット、ずらす方向も指定できる 
			if (_offset != 0) {
				//位置計算
				Vector3 rotOffset;

				// Yaw + Pitch 回転
				rotOffset.x = _offset * cosf(m_Owner->m_Rotation.y) + m_Owner->radius * 2 * sinf(m_Owner->m_Rotation.y);

				rotOffset.z = -_offset * sinf(m_Owner->m_Rotation.y) + m_Owner->radius * 2 * cosf(m_Owner->m_Rotation.y);

				pos += rotOffset;
				follow = false;//　オフセット指定がある場合は追尾しない
			}

			Vector3 rot = m_Owner->m_Rotation;
			rot.z += PI * 0.5f;
			pr->SetPl(false);
			pr->ChargeStart(pos, rot, 1.0f, follow);
			pr->SetOffset({ 0 ,0,m_Owner->radius * 2 });
			break;
		}
	}
}

// 飛び道具の最大チャージ
void Enemy::Impl::ProjectileChargeMax(float _offset, float _angle) {
	for (auto& pr : m_projectile)
	{
		if (pr->GetState() == ProjectileSTATE::NOT_ACTIVE) {
			bool follow = true;
			Vector3 pos = m_Owner->m_Position;
			// 現在位置からオフセット分ずらした位置にセット、ずらす方向も指定できる 
			if (_offset != 0) {
				//位置計算
				Vector3 rotOffset;
				// Yaw + Pitch 回転
				rotOffset.x = _offset * cosf(m_Owner->m_Rotation.y) + m_Owner->radius * 2 * sinf(m_Owner->m_Rotation.y);

				rotOffset.y = m_Owner->radius * -0.5f;

				rotOffset.z = -_offset * sinf(m_Owner->m_Rotation.y) + m_Owner->radius * 2 * cosf(m_Owner->m_Rotation.y);

				pos += rotOffset;
				follow = false;//　オフセット指定がある場合は追尾しない
			}

			pr->SetPl(false);
			pr->MaxCharge(pos, m_Owner->m_Rotation, 100, follow);
			pr->SetOffset({ 0 ,m_Owner->radius * -0.5f ,m_Owner->radius * 2 });
			break;
		}
	}
}

// 飛び道具の最大チャージ、縦
void Enemy::Impl::ProjectileChargeMax_VT(float _offset, float _angle) {
	for (auto& pr : m_projectile)
	{
		if (pr->GetState() == ProjectileSTATE::NOT_ACTIVE) {

			bool follow = true;
			Vector3 pos = m_Owner->m_Position;
			// 現在位置からオフセット分ずらした位置にセット、ずらす方向も指定できる 
			if (_offset != 0) {
				//位置計算
				Vector3 rotOffset;

				// Yaw + Pitch 回転
				rotOffset.x = _offset * cosf(m_Owner->m_Rotation.y) + m_Owner->radius * 2 * sinf(m_Owner->m_Rotation.y);

				rotOffset.z = -_offset * sinf(m_Owner->m_Rotation.y) + m_Owner->radius * 2 * cosf(m_Owner->m_Rotation.y);

				pos += rotOffset;
				follow = false;//　オフセット指定がある場合は追尾しない
			}

			Vector3 rot = m_Owner->m_Rotation;
			rot.z += PI * 0.5f;
			pr->SetPl(false);
			pr->MaxCharge(pos, rot, 100, follow);
			pr->SetOffset({ 0 ,0,m_Owner->radius * 2 });
			break;
		}
	}
}

// 飛び道具の発射
void Enemy::Impl::ProjectileShot() {
	for (auto& pr : m_projectile)
	{
		if (pr->GetState() == ProjectileSTATE::STANCE) {
			pr->Shot();
			break;
		}
	}
}

// 飛び道具の発射、全弾
void Enemy::Impl::ProjectileShot_All()
{
	for (auto& pr : m_projectile)
	{
		if (pr->GetState() == ProjectileSTATE::STANCE) {
			pr->Shot();
		}
	}
}

// 突き攻撃処理
void Enemy::Impl::Thrust(ThrustType type) {
	switch (type)
	{
	case ThrustType::Center: m_weapon->Thrust();       break;
	case ThrustType::Left:   m_weapon->Thrust_Left();  break;
	case ThrustType::Right:  m_weapon->Thrust_Right(); break;
	}
}

// 連続突きの関数
bool Enemy::Impl::ManyThrust(int maxcount)
{
	if (m_attackcount == 0)
	{
		Thrust(ThrustType::Center);
		Sound::GetInstance()->Play(SOUND_SE_SWING);
		++m_attackcount;
		return false;
	}

	if (m_weapon->GetState() != Sword::STATE::THRUST)
		return false;

	const int stepIndex = m_attackcount % std::size(ComboThrust);
	const ThrustStep& step = ComboThrust[stepIndex];

	if (m_weapon->GetAttackTime() < step.delay)
		return false;

	Thrust(step.type);
	Sound::GetInstance()->Play(SOUND_SE_SWING);

	++m_attackcount;
	return m_attackcount >= maxcount;
}

// ジャンプ処理
void Enemy::Impl::Jump(float _power) {
	m_Owner->m_Velocity.y = _power;
	m_Owner->is_GROUND = false;
	m_Owner->m_Position.y += 0.1f;
}

int Enemy::Impl::GetHP() {
	return hp;
}

Sword* Enemy::Impl::GetWeapon() {
	return m_weapon;
}

void Enemy::Impl::OnHit(Sword* po) {
	if (!po->GetPl()) return;
	Damage(po->GetAtk());
	return;
}

void Enemy::Impl::OnHit(Bullet* bu) {
	if (!bu->GetPl()) return;
	Damage(bu->GetAtk());
	return;
}

void Enemy::Impl::OnHit(Projectile* pr) {
	if (!pr->GetPl()) return;
	Damage(pr->GetAtk());
	return;
}

void Enemy::Impl::OnHit(Cube* cube) {//箱に当たった時の処理
	// 法線方向への速度成分
	auto& col = m_Owner->GetLastCollision();
	float vn = m_Owner->m_Velocity.Dot(col.normal);

	if (vn >= 0.0f) { return; }

	// 法線方向の速度を打ち消す（めり込み防止）
	m_Owner->m_Velocity -= col.normal * vn;

	// 床・壁・天井の区別
	if (col.normal.y > 0.6f)
	{
		// 床（上向きの法線）
		m_Owner->m_Velocity.y = 0.0f;
		m_Owner->m_Position.y = m_Owner->m_oldPos.y;
		m_Owner->is_GROUND = true;
	}
	else if (col.normal.y < -0.6f)
	{
		// 天井（下向きの法線）
		m_Owner->m_Velocity.y = 0.0f;
	}
	else
	{
		// 壁（ほぼ垂直）
		m_Owner->m_Velocity.x = 0.0f;
		m_Owner->m_Velocity.z = 0.0f;
		m_Owner->m_Position.x = m_Owner->m_oldPos.x;
		m_Owner->m_Position.z = m_Owner->m_oldPos.z;

		hitwall = true;
		
	}


}
