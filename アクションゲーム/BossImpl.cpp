#include "BossImpl.h"

#include "Player.h"
#include "Ground.h"
#include "Game.h"
#include "Pole.h"
#include "Collision.h"
#include "Arrow.h"
#include "Bullet.h"
#include "DebugUI.h"
#include "EffectManager.h"

using namespace std;
using namespace DirectX::SimpleMath;

Boss::Impl::Impl(Camera* cam, Boss* owner)
	: m_Owner(owner)
	, m_Camera(cam)
{
	m_weapon = Game::GetInstance()->AddObject<Pole>();
}


Boss::Impl::~Impl()
{

}

void Boss::Impl::DebugBossStatus() {//ボスの状態を操作する
	ImGui::Begin("BossStatus");
	static bool update = true;
	static bool death;

	ImGui::Checkbox("Update", &update);
	ImGui::Checkbox("Slow", &m_slowFg);
	ImGui::SliderInt("Slowrate", &slow_rate, 1, 59);
	if (ImGui::Button("BOSS DEATH"))
		hp = 0;

	if (ImGui::Button("BOSSHP MAX"))
		hp = 50;

	static int debug_attack_kind = -1;
	ImGui::SliderInt("AttackKind", &debug_attack_kind, -1, KIND_MAX - 1);
	if (ImGui::Button("BOSSATTACK")) {
		m_State = ATTACK;
		framecount = 0;
		m_Owner->m_Velocity_f = 0;
		attack_kind = debug_attack_kind;
	}

	if (update) {
		notUpdate = false;
	}
	else {
		notUpdate = true;
	}

	ImGui::End();
}

void Boss::Impl::Init() {
	m_Owner->GBInit(u8"assets/model/Character/boss.fbx");
	m_Owner->m_Position = Vector3(0.0f, 50.0f, -50.0f);
	m_Owner->m_Velocity_f = 0.0f;//はじめに移動速度を0にする
	hp = 50;
	def = 0;
	m_Owner->m_Scale.x = 2;
	m_Owner->m_Scale.y = 2;
	m_Owner->m_Scale.z = 2;
	m_Owner->radius *= 2;

	//丸影の大きさをセット
	m_Owner->m_Shadow->SetBaseScale(18 * m_Owner->m_Scale.x);
	
	DebugUI::RedistDebugFunction([this]() { DebugBossStatus(); });
}

void Boss::Impl::Update() {
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

	switch (m_State) {
	case NORMAL:
		Move();
		if (framecount > 360) {
			m_State = ATTACK;
			framecount = 0;
			m_Owner->m_Velocity_f = 0;
			attack_kind = (rand() % 2) + 1;
		}
		if (framecount % 90 == 0 && framecount != 0) {
			ShotBullet();
		}
		++framecount;
		break;
	case ATTACK:
		AttackUpdate();
		break;
	}


	if (m_lookatFg)
	  LookAt(Game::GetInstance()->GetObjects<Player>()[0]->GetPosition());
	
	if (inviFg) {
		++invicount;
	}
	if (invicount > 30) {
		inviFg = false;
		invicount = 0;
		m_Owner->SetColor(Vector4(1, 0, 0, 1));
	}
	if (m_weapon)
		m_weapon->Update(m_Owner->m_Position, m_Owner->radius, m_Owner->m_Rotation, 1.0f);

	if (!(attack_kind == ROTATESWING))
		m_Owner->m_Rotation.y = m_Owner->m_ForwardRotation.y;

	m_Owner->GBUpdate();
}

void Boss::Impl::Draw()
{
	m_Owner->GBDraw();
}

bool Boss::Impl::GetLive() {
	if (hp > 0) {
		return true;
	}
	return false;
}

void Boss::Impl::Damage(int _atk) {
	if (inviFg)  return;
	//防御力分ダメージ軽減
	_atk -= def;
	//ダメージがマイナスにならないように補正
	if (_atk < 0) _atk = 0;

	hp -= _atk;
	invicount = 0.0f;
	inviFg = true;
	m_Owner->m_Velocity_f = 0.0f;//移動速度を0にする
	m_Owner->SetColor(Vector4(1, 1, 1, 0.5));

	//左から右へ移動するエフェクト再生
	Vector3 pos = m_Owner->m_Position;
	pos = EffectManager::ToCameraEffectPos(pos, m_Owner->radius * m_Owner->m_Scale.x);
	pos -= m_Camera->GetRightVector() * m_Owner->radius;

	//エフェクトパラメーター構造体作成
	EffectParams param;
	param.pos = pos;
	param.pos_amount = { m_Owner->radius * 2, 0 ,0};
	param.scale = m_Owner->m_Scale * 10;
	param.maxLife = 12;

	// エフェクト再生
	EffectManager::GetInstance()->Play(EFFECT_SLASH, param);

	// SE再生
	Sound::GetInstance()->Play(SOUND_SE_SWORDHIT);
}

void Boss::Impl::LookAt(Vector3 ta_pos) {

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
	m_Owner->m_ForwardRotation.y += fDiffRotY * rotate_speed * 2;
	if (m_Owner->m_ForwardRotation.y > PI)
	{
		m_Owner->m_ForwardRotation.y -= PI * 2.0f;
	}
	if (m_Owner->m_ForwardRotation.y < -PI)
	{
		m_Owner->m_ForwardRotation.y += PI * 2.0f;
	}
}

void Boss::Impl::AttackUpdate() {
	int weapon_state = m_weapon->GetState();
	switch (attack_kind) {
	case NONE:
		m_State = NORMAL;
		break;
	case SWING:// 横振り
		if (weapon_state == Pole::STATE::NORMAL) {
			m_weapon->StanceStart();
		}
		else if (weapon_state == Pole::STATE::STANCE && m_weapon->GetStanceTime() > 60) {
			m_weapon->Swing();
		}
		else if (weapon_state == Pole::STATE::SWING && m_weapon->GetAttackTime() > 18) {
			m_State = NORMAL;
			framecount = 0;
			m_weapon->SwingEnd();
		}
		break;

	case ROTATESWING://回転切り
		if (weapon_state == Pole::STATE::NORMAL) {
			m_weapon->StanceStart();
		}
		else if (weapon_state == Pole::STATE::STANCE && m_weapon->GetStanceTime() > 90) {
			m_weapon->AttackStart();
			attack_time = 0;
		}
		else if (weapon_state == Pole::STATE::ATTACK) {
			m_Owner->m_Rotation.y += PI / 20;
			Move();
			++attack_time;
		}

		if (attack_time % 30 == 0) {
			Sound::GetInstance()->Play(SOUND_SE_ROTATEATTACK);
		}

		if (attack_time > 300) {
			m_weapon->AttackEnd();
			m_State = NORMAL;
			framecount = 0;
			attack_time = 0;
			attack_kind = NONE;//攻撃終了
		}

		break;
	
	case SWING_VERTICAL://縦振り
		if (weapon_state == Pole::STATE::NORMAL) {
			m_weapon->Stance_Vertical();
			rotate_speed = 0.1f;
		}
		else if (weapon_state == Pole::STATE::STANCE && m_weapon->GetStanceTime() > 120 && !m_rushFg) {
			m_lookatFg = false;
			m_ta_pos = Game::GetInstance()->GetObjects<Player>()[0]->GetPosition();
			m_ta_pos.y = m_Owner->m_Position.y;//高さはそのまま
			m_rushFg = true;

		}

		if (weapon_state == Pole::STATE::STANCE && m_rushFg) {
			//近づいたら振る
			Move();
			framecount++;
			if (framecount > 3) {
				// 土煙エフェクト再生
				EffectParams   param;
				param.pos = m_Owner->m_Position;
				param.scale = m_Owner->m_Scale * 20;
				param.maxLife = 30;
				EffectManager::GetInstance()->Play(EFFECT_TUTIKEMURI, param);
				framecount = 0;
			}

			if (fabs(m_Owner->m_Position.x - m_ta_pos.x) < m_Owner->radius * 2 &&
				fabs(m_Owner->m_Position.z - m_ta_pos.z) < m_Owner->radius * 2) {
				m_weapon->Swing_Vertical();
				m_rushFg = false;
				m_Owner->m_Velocity_f = 0.0f;//移動速度を0にする
				framecount = 0;
			}
		}

		if (weapon_state == Pole::STATE::SWING && m_weapon->GetAttackTime() == 10) {
			//エフェクトパラメーター構造体作成
			EffectParams param;
			param.scale = m_Owner->m_Scale * 15;
			param.maxLife = 60;
			// エフェクト再生
			m_weapon->TipToEffect(EFFECT_TUTIKEMURI_BIG, param);
			Sound::GetInstance()->Play(SOUND_SE_SWINGVERTICAL);
		}

		if (weapon_state == Pole::STATE::SWING && m_weapon->GetAttackTime() > 18) {
			m_State = NORMAL;
			framecount = 0;
			m_weapon->SwingEnd();
			m_lookatFg = true;
			rotate_speed = 0.01f;

		}

		break;
	case MANY_THRUST://連続突き
		Move();
		if (weapon_state == Pole::STATE::NORMAL) {
			m_weapon->Thrust();
			attack_count++;
			rotate_speed = 0.05f;
			Sound::GetInstance()->Play(SOUND_SE_SWING);
		}

		if (weapon_state == Pole::STATE::THRUST && m_weapon->GetAttackTime() > 4) {
			attack_count++;
			int attack_count_remaind = attack_count % 3;
			if (attack_count_remaind == 0) {
				m_weapon->Thrust();
			}
			else if (attack_count_remaind == 1) {
				m_weapon->Thrust_Left();
			}
			else {
				m_weapon->Thrust_Right();
			}
			Sound::GetInstance()->Play(SOUND_SE_SWING);
		}

		if (attack_count > 90) {
			m_weapon->ThrustEnd();
			m_State = NORMAL;
			rotate_speed = 0.1f;
			attack_count = 0;
		}
		break;

	case KIND_MAX:
		m_State = NORMAL;
		break;
	}
}

void Boss::Impl::ShotBullet() {
	int r1 = rand();
	int r2 = rand();
	Vector3 ta_pos = Game::GetInstance()->GetObjects<Player>()[0]->GetPosition();
	Vector3 ta_addpos = Vector3((r1 % 20) + 20, 0, (r2 % 20) + 20);
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

void Boss::Impl::Move(){
	if (m_rushFg) {
		m_Owner->m_Velocity_f = move_speed * 8;
		//m_Owner->m_Rotation.x += 0.2f;
	}
	else {
		m_Owner->m_Velocity_f = move_speed;
		//m_Owner->m_Rotation.x += 0.025f;
	}

	if (m_Owner->m_Rotation.x > PI * 2) m_Owner->m_Rotation.x -= PI * 2;

	if (m_State == 0) {

	}
	//else if (m_State == 1) {
	//	flamecount++;
	//	if (flamecount > 60) {
	//		m_State = 0;
	//		flamecount = 0;
	//		m_Velocity_f = 0.0f;//移動速度を0にする
	//	}
	//	if (m_Position.x > stagesize.x - radius * 3 || m_Position.x < -stagesize.x//落下防止
	//		|| m_Position.z > stagesize.y || m_Position.z < -stagesize.y + radius * 2)
	//	{
	//		if (m_Position.x > stagesize.x - radius * 3) {
	//			m_Position.x = stagesize.x - radius * 3;
	//		}
	//		if (m_Position.x < -stagesize.x) {
	//			m_Position.x = -stagesize.x;
	//		}
	//		if (m_Position.z > stagesize.y) {
	//			m_Position.z = stagesize.y;
	//		}
	//		if (m_Position.z < -stagesize.y + radius) {
	//			m_Position.z = -stagesize.y + radius;
	//		}
	//		m_State = 0;
	//		flamecount = 0;
	//		if (m_Position.y > -10) {
	//			m_Position.y = radius;
	//		}
	//		m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	//		m_Velocity_f = 0.0f;
	//	}
	//}
	//else if (m_State == 2) {
	//	m_Position -= hitbackrotation * 1.0f;
	//	//m_Position.y = 0.0f;
	//	flamecount++;
	//	if (flamecount > 30) {
	//		m_State = 0;
	//	}
	//}

}

int Boss::Impl::GetHP() {
	return hp;
}

Pole* Boss::Impl::GetWeapon() {
	return m_weapon;
}

void Boss::Impl::OnHit(Pole* po) {
	if (!po->GetPl()) return;
	Damage(po->GetAtk());
	return;
}

void Boss::Impl::OnHit(Bullet* bu) {
	if (!bu->GetPl()) return;
	Damage(bu->GetAtk());
	return;
}

void Boss::Impl::OnHit(TestCube* cube) {//箱に当たった時の処理
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
		m_Owner->m_Position = m_Owner->m_oldPos;

		// 突進中なら縦振りを強制発動(はまり防止)
		if (m_rushFg) {
		    
		    m_weapon->Swing_Vertical();
		    m_rushFg = false;
		    m_Owner->m_Velocity_f = 0.0f;//移動速度を0にする
		    
		}
	}

}
