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

}


Boss::Impl::~Impl()
{

}

void Boss::Impl::DebugBossStatus() {//ƒ{ƒX‚Ìó‘Ô‚ð‘€ì‚·‚é
	ImGui::Begin("BossStatus");
	static bool update = true;
	static bool death;
	ImGui::Checkbox("Update", &update);
	ImGui::Checkbox("BOSSDEATH", &death);
	if (update) {
		notUpdate = false;
	}
	else {
		notUpdate = true;
	}
	if (death) {
		hp = 0;
	}
	ImGui::End();
}

void Boss::Impl::Init() {
	m_Owner->GBInit(u8"assets/model/gorufu/GolfBall_v2_red.fbx");
	m_Owner->m_Position = Vector3(0.0f, 50.0f, -50.0f);
	m_weapon = Game::GetInstance()->AddObject<Pole>();
	m_Owner->m_Velocity_f = 0.0f;//‚Í‚¶‚ß‚ÉˆÚ“®‘¬“x‚ð0‚É‚·‚é
	hp = 50;
	m_Owner->m_Scale.x = 2;
	m_Owner->m_Scale.y = 2;
	m_Owner->m_Scale.z = 2;
	m_Owner->radius *= 2;
	DebugUI::RedistDebugFunction([this]() { DebugBossStatus(); });
}

void Boss::Impl::Update() {
	if (hp <= 0 || notUpdate) { return; };
	switch (m_State) {
	case NORMAL:
		LookAt(Game::GetInstance()->GetObjects<Player>()[0]->GetPosition());
		Move();
		if (flamecount > 360) {
			m_State = ATTACK;
			flamecount = 0;
			m_Owner->m_Velocity_f = 0;
			attack_kind = 2;//rand() % KIND_MAX;
		}
		if (flamecount % 90 == 0 && flamecount != 0) {
			ShotBullet();
		}
		++flamecount;
		break;
	case ATTACK:
		AttackUpdate();
		break;
	}


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
		m_Owner->m_Rotation = m_Owner->m_ForwardRotation;

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

void Boss::Impl::Damage(int atk) {
	if (inviFg)  return;
	hp -= atk;
	invicount = 0.0f;
	inviFg = true;
	m_Owner->m_Velocity_f = 0.0f;//ˆÚ“®‘¬“x‚ð0‚É‚·‚é
	m_Owner->SetColor(Vector4(0, 1, 1, 0.5));

	//¶‚©‚ç‰E‚ÖˆÚ“®‚·‚éƒGƒtƒFƒNƒgÄ¶
	Vector3 pos = m_Owner->m_Position;
	pos = m_Owner->ToCameraEffectPos(pos, m_Owner->radius * m_Owner->m_Scale.x);
	pos -= m_Camera->GetRightVector() * m_Owner->radius;

	EffectParams param;
	param.pos = pos;
	param.pos_amount = { m_Owner->radius * 2, 0 ,0};
	param.scale = m_Owner->m_Scale * 5;
	param.maxLife = 12;

	EffectManager::Play(SLASH, param);

	Sound::GetInstance()->Play(SOUND_SE_SWORDHIT);
}

void Boss::Impl::LookAt(Vector3 ta_pos) {

	// atan2‚ðŽg—p‚µ‚ÄŠp“x‚ð‹‚ß‚é
	m_destrot.y = atan2f((ta_pos.x - m_Owner->m_Position.x), (ta_pos.z - m_Owner->m_Position.z));

	// Œ»Ý‚ÌŒü‚«‚Æ‚Ì·•ª‚ðŒvŽZ‚·‚é
	float fDiffRotY = m_destrot.y - m_Owner->m_ForwardRotation.y;

	// •â³i|‚P‚W‚O`‚P‚W‚O‚Ì”ÍˆÍj
	if (fDiffRotY > PI)
	{
		fDiffRotY -= PI * 2.0f;
	}
	if (fDiffRotY < -PI)
	{
		fDiffRotY += PI * 2.0f;
	}

	// ‰ñ“]Šp“xŒvŽZ
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
	LookAt(Game::GetInstance()->GetObjects<Player>()[0]->GetPosition());
	int weapon_state = m_weapon->GetState();
	switch (attack_kind) {
	case SWING:
		if (weapon_state == Pole::STATE::NORMAL) {
			m_weapon->StanceStart();
		}
		else if (weapon_state == Pole::STATE::STANCE && m_weapon->GetStanceTime() > 60) {
			m_weapon->Swing();
		}
		else if (weapon_state == Pole::STATE::SWING && m_weapon->GetSwingTime() > 18) {
			m_State = NORMAL;
			flamecount = 0;
			m_weapon->SwingEnd();
		}
		break;
	case SHOT:

		break;

	case ROTATESWING://‰ñ“]Ø‚è
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

		if (attack_time > 600) {
			m_weapon->AttackEnd();
			m_State = NORMAL;
			flamecount = 0;
			attack_time = 0;
			attack_kind = NONE;//UŒ‚I—¹
		}

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

void Boss::Impl::Move() {
	m_Owner->m_Velocity_f = ForwardVelocity / 2;
	if (m_State == 0) {

	}
	//else if (m_State == 1) {
	//	flamecount++;
	//	if (flamecount > 60) {
	//		m_State = 0;
	//		flamecount = 0;
	//		m_Velocity_f = 0.0f;//ˆÚ“®‘¬“x‚ð0‚É‚·‚é
	//	}
	//	if (m_Position.x > stagesize.x - radius * 3 || m_Position.x < -stagesize.x//—Ž‰º–hŽ~
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
	Damage(po->atk);
	return;
}

void Boss::Impl::OnHit(Bullet* bu) {
	if (!bu->GetPl()) return;
	Damage(bu->GetAtk());
	return;
}

void Boss::Impl::OnHit(TestCube* cube) {//” ‚É“–‚½‚Á‚½Žž‚Ìˆ—
	// –@ü•ûŒü‚Ö‚Ì‘¬“x¬•ª
	auto& col = m_Owner->GetLastCollision();
	float vn = m_Owner->m_Velocity.Dot(col.normal);

	if (vn >= 0.0f) { return; }

	// –@ü•ûŒü‚Ì‘¬“x‚ð‘Å‚¿Á‚·i‚ß‚èž‚Ý–hŽ~j
	m_Owner->m_Velocity -= col.normal * vn;

	// °E•ÇE“Vˆä‚Ì‹æ•Ê
	if (col.normal.y > 0.6f)
	{
		// °iãŒü‚«‚Ì–@üj
		m_Owner->m_Velocity.y = 0.0f;
		m_Owner->m_Position.y = m_Owner->m_oldPos.y;
		m_Owner->is_GROUND = true;
	}
	else if (col.normal.y < -0.6f)
	{
		// “Vˆäi‰ºŒü‚«‚Ì–@üj
		m_Owner->m_Velocity.y = 0.0f;
	}
	else
	{
		// •Çi‚Ù‚Ú‚’¼j
		m_Owner->m_Velocity.x = 0.0f;
		m_Owner->m_Velocity.z = 0.0f;
		m_Owner->m_Position = m_Owner->m_oldPos;
	}

}
