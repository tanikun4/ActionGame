#include "Boss.h"
#include "Player.h"
#include "Ground.h"
#include "Game.h"
#include "Pole.h"
#include "Collision.h"
#include "Arrow.h"
#include "Bullet.h"
#include "DebugUI.h"

using namespace std;
using namespace DirectX::SimpleMath;

Boss::Boss(Camera* cam) : GolfBall(cam)
{

}


Boss::~Boss()
{

}

void Boss::DebugBossStatus() {//ƒ{ƒX‚Ìó‘Ô‚ð‘€ì‚·‚é
	ImGui::Begin("BossStatus");
	static int select = 0;
	ImGui::RadioButton("Not_Update", &select, 1);
	ImGui::RadioButton("Update", &select, 0);
	if (select == 1) {
		notUpdate = true;
	}
	else {
		notUpdate = false;
	}
	ImGui::End();
}

void Boss::Init() {
	GBInit(u8"assets/model/gorufu/GolfBall_v2_red.fbx");
	m_Position = Vector3(0.0f, 50.0f, -50.0f);
	m_weapon = Game::GetInstance()->AddObject<Pole>();
	m_Velocity_f = 0.0f;//‚Í‚¶‚ß‚ÉˆÚ“®‘¬“x‚ð0‚É‚·‚é
	hp = 50;
	m_Scale.x = 2;
	m_Scale.y = 2;
	m_Scale.z = 2;
	radius *= 2;
	DebugUI::RedistDebugFunction([this]() { DebugBossStatus(); });
}

void Boss::Update() {
	if (hp <= 0 || notUpdate) { return; };
	switch (m_State) {
	case 0:
		LookAt(Game::GetInstance()->GetObjects<Player>()[0]->GetPosition());
		Move();
		if (flamecount > 360) {
			m_State = 1;
			flamecount = 0;
		}
		if (flamecount % 90 == 0 && flamecount != 0) {
			ShotBullet();
		}
		break;
	case 1:
		Attack();
		break;
	}

	++flamecount;


	if (inviFg) {
		++invicount;
	}
	if (invicount > 30) {
		inviFg = false;
		invicount = 0;
		SetColor(Vector4(1, 0, 0, 1));
	}
	/*if (m_State != 2) {
		HitCheck();
	}*/
	m_weapon->Update(m_Position, radius, m_Rotation,1.0f);
	//m_weapon->SetPosition(m_Position);
	GBUpdate();
}

void Boss::Draw()
{
	GBDraw();
}

bool Boss::GetLive() {
	if (hp > 0) {
		return true;
	}
	return false;
}

void Boss::Damage(int atk) {
	if (inviFg)  return;
	hp -= atk;
	invicount = 0.0f;
	inviFg = true;
	m_Velocity_f = 0.0f;//ˆÚ“®‘¬“x‚ð0‚É‚·‚é
	SetColor(Vector4(0, 1, 1, 0.5));
	Sound::GetInstance()->Play(SOUND_SE_SWORDHIT);
}

bool Boss::HitCheck() {
	//vector<Pole*> pole = Game::GetInstance()->GetObjects<Pole>();
	//Collision::Sphere balCollision = { m_Position, radius };
	//if (pole[0]->GetState() == 1) {
	//	if (CheckHit(pole[0]->hitbox, balCollision)) {
	//		//vector<Player*> player = Game::GetInstance()->GetObjects<Player>();
	//		//hitbackrotation = -1.0f * player[0]->GetForwardVector();
	//		Damage(pole[0]->atk);
	//		Sound::GetInstance()->Play(SOUND_SE_SWORDHIT);
	//		return true;
	//	}
	//}
	//vector<Arrow*> arrow = Game::GetInstance()->GetObjects<Arrow>();
	//for (auto& ar : arrow) {
	//	if (ar->GetId() == 1) {
	//		if (CheckHit(ar->hitbox, balCollision)) {
	//			hitbackrotation = ar->GetForwardVector();
	//			Damage(ar->GetAtk());
	//			Sound::GetInstance()->Play(SOUND_SE_ARROWHIT);
	//			return true;
	//		}
	//	}
	//}
	return false;
}

void Boss::LookAt(Vector3 ta_pos) {

	// atan2‚ðŽg—p‚µ‚ÄŠp“x‚ð‹‚ß‚é
	m_destrot.y = atan2f((ta_pos.x - m_Position.x), (ta_pos.z - m_Position.z));

	// Œ»Ý‚ÌŒü‚«‚Æ‚Ì·•ª‚ðŒvŽZ‚·‚é
	float fDiffRotY = m_destrot.y - m_Rotation.y;

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
	m_Rotation.y += fDiffRotY * rotate_speed * 2;
	if (m_Rotation.y > PI)
	{
		m_Rotation.y -= PI * 2.0f;
	}
	if (m_Rotation.y < -PI)
	{
		m_Rotation.y += PI * 2.0f;
	}
}

void Boss::Attack() {
	m_weapon->Swing();
	m_State = 0;
}

void Boss::ShotBullet() {
	int r = rand();
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

void Boss::Move() {
	int r = rand();
	m_Velocity_f = ForwardVelocity / 2;
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

//bool Boss::HitCheckPole(Pole* pole) {
//	if (inviFg) { return false; }
//	if (pole->GetState() == 1) {
//		Collision::Sphere balCollision = { m_Position, radius };
//		if (CheckHit(pole->hitbox, balCollision)) {
//			//vector<Player*> player = Game::GetInstance()->GetObjects<Player>();
//			//hitbackrotation = -1.0f * player[0]->GetForwardVector();
//			Damage(pole->atk);
//			Sound::GetInstance()->Play(SOUND_SE_SWORDHIT);
//			return true;
//		}
//	}
//	return false;
//}

int Boss::GetHP() {
	return hp;
}

Pole* Boss::GetWeapon() {
	return m_weapon;
}

void Boss::OnHit(Pole* po) {
	if (!po->GetPl()) return;
	Damage(po->atk);
	return;
}

void Boss::OnHit(Bullet* bu) {
	if (!bu->GetPl()) return;
	Damage(bu->GetAtk());
	return;
}