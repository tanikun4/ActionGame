#include "Player.h"
#include <memory>
#include "StaticMesh.h"
#include "utility.h"
#include "Collision.h"
#include "Game.h"
#include "Ground.h"
#include "Pole.h"
#include "Arrow.h"
#include "Enemy.h"
#include "sound.h"
#include "Boss.h"
#include "WeaponManager.h"

using namespace std;
using namespace DirectX::SimpleMath;
extern Input input;

// コンストラクタ
Player::Player(Camera* cam) : GolfBall(cam)
{
	m_arrow = nullptr;
}

// デストラクタ
Player::~Player()
{

}

void Player::Init() {
	GBInit(u8"assets/model/gorufu/GolfBall_v2.fbx");
	m_Position = Vector3(0.0f, 50.0f, 0.0f);
	m_pole = Game::GetInstance()->AddObject<Pole>();
	m_pole->SetPl(true);
	hp = 9;
	flamecount = 30;
}

void Player::Update() {
	m_Velocity_f = 0.0f;//はじめに移動速度を0にする
	//状態ごとの処理
	switch (m_State) {//0:通常時、1:近接攻撃中,2:遠距離攻撃中,3:ダメージ中,4:回避状態5:ガード状態
	case 0:
		Move();
		Attack();
		Guard();
		break;
	case 1:
		if (m_pole->GetState() == 0) {
			m_State = 0;
		}
		break;
	case 2:
		if (Input::GetKeyPress(VK_A))
		{
			m_Rotation.y -= 0.1;

		}
		else if (Input::GetKeyPress(VK_D))
		{
			m_Rotation.y += 0.1;
		}

		Charge();

		if (Input::GetKeyRelease(VK_SHIFT)) {
			Shot();
			m_State = 0;
			Sound::GetInstance()->Play(SOUND_SE_ARROWSHOT);
		}
		break;
	case 3:
		// 現在の座標を計算
		m_Position -= m_ForwardVector;
		//m_Position.y = 0.0f;
		if (flamecount > 10) {
			m_State = 0;
			InviFg = false;
		}
		break;
	case 4:
		m_Position += m_ForwardVector * speed * 2.0f;
		//m_Position.y = 0.0f;
		rollcount++;
		if (rollcount > 20) {
			m_State = 0;
			InviFg = false;
			rollcount = 0;
		}
		break;
	case 5:
		Guard();
		break;
	}

	if (m_State != 4) {
		if (rollcount < rollcooldown) {
			++rollcount;
		}
	}

	if (flamecount < 30) {
		++flamecount;
	}
	else {
		//CheckHit();
	}
	// 下に落ちた時はダメージを受けてリスポーン
	if (m_Position.y < -100)
	{
		hp -= 1;
		m_Position = Vector3(0.0f, 50.0f, 0.0f);
		m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	}
	m_pole->Update(m_Position, radius, m_Rotation,1.7f);
	GBUpdate();
}

void Player::Move() {
	//キー入力による移動
	if (Input::GetKeyPress(VK_W))
	{
		// 前進
		if (Input::GetKeyPress(VK_A)) {
			m_Rotation.y = 3 * PI / 4;
		}
		else if (Input::GetKeyPress(VK_D)) {
			m_Rotation.y = 5 * PI / 4;
		}
		else {
			m_Rotation.y = PI;
		}
		m_Velocity_f = speed;
	}
	else if (Input::GetKeyPress(VK_S) == true)
	{
		// 後退
		if (Input::GetKeyPress(VK_A)) {
			m_Rotation.y = PI / 4;
		}
		else if (Input::GetKeyPress(VK_D)) {
			m_Rotation.y = 7 * PI / 4;
		}
		else {
			m_Rotation.y = 0.0f;
		}
		m_Velocity_f = speed;
	}
	else if (Input::GetKeyPress(VK_A) == true)
	{
		//左移動
		m_Rotation.y = PI / 2;
		m_Velocity_f = speed;

	}
	else if (Input::GetKeyPress(VK_D) == true)
	{
		//右移動
		m_Rotation.y = 3 * PI / 2;
		m_Velocity_f = speed;

	}
	if (Input::GetKeyTrigger(VK_CONTROL)) {
		if (rollcount >= rollcooldown) {
			DodgeRoll();
		}
	}
}

void Player::DodgeRoll() {
	m_State = 4;
	rollcount = 0;
	InviFg = true;
}

void Player::Attack() {
	if (Input::GetKeyTrigger(VK_SPACE)) {
		m_pole->Swing();
		m_State = 1;
		Sound::GetInstance()->Play(SOUND_SE_SWING);
	}
	if (Input::GetKeyTrigger(VK_SHIFT)) {
		Charge();
		m_State = 2;
		Sound::GetInstance()->Play(SOUND_SE_ARROWCHARGE);
	}
}

void Player::Charge() {
	if (!m_arrow) {
		vector<Arrow*> arrow = Game::GetInstance()->GetObjects<Arrow>();
		for (auto& ar : arrow) {
			if (ar->GetState() == 0) {
				m_arrow = ar;
				break;
			}
		}
		if (!m_arrow) {
			return;
		}
	}
	m_arrow->PCharge(m_Rotation.y,radius,m_Position);
}

void Player::Shot() {
	if (!m_arrow) {
		vector<Arrow*> arrow = Game::GetInstance()->GetObjects<Arrow>();
		for (auto& ar : arrow) {
			if (ar->GetState() == 0) {
				m_arrow = ar;
				break;
			}
		}
		if (!m_arrow) {
			return;
		}
	}
	m_arrow->PShot(m_Rotation.y, radius, m_Position);
	m_arrow = nullptr;
}

void Player::CheckHit() {
	Collision::Sphere balCollision = { m_Position, radius };
	/*vector<Enemy*> enemy = Game::GetInstance()->GetObjects<Enemy>();
	for (auto& en : enemy) {
		if (en->GetLive()) {
			Collision::Sphere enbalCollision = { en->GetPosition(), en->GetRadius() };
			if (Collision::CheckHit(enbalCollision, balCollision)) {
				Damage(1);
				Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
				return;
			}
		}
	}*/

	Boss* boss = Game::GetInstance()->GetObjects<Boss>()[0];
	Collision::Sphere bossbalCollision = { boss->GetPosition(), boss->GetRadius() };
	if (Collision::CheckHit(bossbalCollision, balCollision)) {
		Damage(1);
		Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
		return;
	}

	vector<Arrow*> arrow = Game::GetInstance()->GetObjects<Arrow>();
	for (auto& ar : arrow) {
		if (ar->GetId() == 2) {
			if (Collision::CheckHit(ar->hitbox, balCollision)) {
				Damage(ar->GetAtk());
				Sound::GetInstance()->Play(SOUND_SE_ARROWHIT);
				return;
			}
		}
	}
	return;
}

void Player::CheckHitPole(Pole* pole) {
	if (m_State == 3) { return; }
	Collision::Sphere balCollision = { m_Position , radius };
	if (pole->GetState() == 1) {
		if (Collision::CheckHit(pole->hitbox, balCollision)) {
			Damage(2);
			Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
		}
	}
	return;
}

void Player::Damage(int atk) {
	if (InviFg == false) {
		hp -= atk;
		m_State = 3;
		flamecount = 0;
		InviFg = true;
		Sound::GetInstance()->Play(SOUND_SE_PLAYERHIT);
	}
}

int Player::GetHP() {
	return hp;
}

Pole* Player::GetWeapon() {
	return m_pole;
}

void Player::Guard() {
	if (Input::GetKeyTrigger(VK_E)) {
		m_State = 5;
		GuardFg = true;
	}
	if (Input::GetButtonRelease(VK_E)) {
		m_State = 0;
		GuardFg = false;
	}
	return;
}

void Player::OnHit(Boss* bo) {
	Damage(1);
	return;
}

void Player::OnHit(Pole* po) {
	const int damage = 2;
	if (po->GetPl()) return;
	Damage(damage);
}

