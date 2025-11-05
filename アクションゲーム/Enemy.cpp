#include "Enemy.h"
#include "Player.h"
#include <memory>
#include "StaticMesh.h"
#include "utility.h"
#include "Collision.h"
#include "Game.h"
#include "Ground.h"
#include "Pole.h"
#include "Arrow.h"
#include "sound.h"

using namespace std;
using namespace DirectX::SimpleMath;

Enemy::Enemy(Camera* cam) : GolfBall(cam) 
{

}


Enemy::~Enemy() 
{

}

void Enemy::Init() {
	GBInit(u8"assets/model/gorufu/GolfBall_v2_red.fbx");
	m_Velocity_f = 0.0f;//はじめに移動速度を0にする
	hp = 10;
}

void Enemy::Update() {
	if (hp <= 0) {
		if (m_arrow) {
			m_arrow->SetState(0);
			m_arrow = nullptr;
		}
		return;
	}
	int r = rand();
	SetArrow();
	if (m_State == 0) {
		if (m_Position.y > 0.0f)
		{
			//flamecount += (r % 3) + 1;
		}
		if (flamecount > 90) {
			m_Velocity_f = ForwardVelocity;
			switch (r % 5) {
			case 0://上移動
				m_Rotation.y = PI;
				break;
			case 1://下移動
				m_Rotation.y = 0.0f;
				break;
			case 2://左移動
				m_Rotation.y = PI / 2;
				break;
			case 3://右移動
				m_Rotation.y = 3 * PI / 2;
				break;
			case 4://攻撃
				m_Velocity_f = 0.0f;//移動をリセット
				Attack();//攻撃関数
				break;
			}
			m_State = 1;
			flamecount = 0;
		}
	}
	else if (m_State == 1) {
		flamecount++;
		if (flamecount > 60) {
			m_State = 0;
			flamecount = 0;
			m_Velocity_f = 0.0f;//移動速度を0にする
		}
		if (m_Position.x > stagesize.x - radius * 3 || m_Position.x < -stagesize.x
			|| m_Position.z > stagesize.y|| m_Position.z < -stagesize.y + radius * 2)
		{
			if (m_Position.x > stagesize.x - radius * 3) {
				m_Position.x = stagesize.x - radius * 3;
			}
			if (m_Position.x < -stagesize.x) {
				m_Position.x = -stagesize.x;
			}
			if (m_Position.z > stagesize.y) {
				m_Position.z = stagesize.y;
			}
			if (m_Position.z < -stagesize.y + radius) {
				m_Position.z = -stagesize.y + radius;
			}
			m_State = 0;
			flamecount = 0;
			if (m_Position.y > -10) {
				m_Position.y = radius;
			}
			m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
			m_Velocity_f = 0.0f;
		}
	}
	else if (m_State == 2) {
		m_Position -= hitbackrotation * 1.0f;
		//m_Position.y = 0.0f;
		flamecount++;
		if (flamecount > 30) {
			m_State = 0;
		}
	}

	if (m_State != 2) {
		HitCheck();
	}

	// 下に落ちた時はhpを0にする
	if (m_Position.y < -100)
	{
		hp = 0;
		m_Position = Vector3(0.0f, 10.0f, 0.0f);
		m_Velocity = Vector3(0.0f, 0.0f, 0.0f);
	}
	GBUpdate();
}

void Enemy::Damage(int atk) {
	hp -= atk;
	m_State = 2;
	flamecount = 0.0f;
	m_Velocity_f = 0.0f;//移動速度を0にする
}

void Enemy::SetArrow() {
	if (!m_arrow) 
	{
		vector<Arrow*> arrow = Game::GetInstance()->GetObjects<Arrow>();
		for (auto& ar : arrow) {
			if (ar->GetState() == 0) {
				m_arrow = ar;
				break;
			}
		}
	}
	if (m_arrow) {
		m_arrow->EPose(m_Rotation.y, radius, m_Position);
	}
}

void Enemy::Attack() {
	if (!m_arrow)
	{
		vector<Arrow*> arrow = Game::GetInstance()->GetObjects<Arrow>();
		for (auto& ar : arrow) {
			if (ar->GetState() == 0) {
				m_arrow = ar;
				break;
			}
		}
	}
	if (m_arrow) {
		m_arrow->EShot(m_Rotation.y, radius, m_Position);
		m_arrow = nullptr;
	}
}

void Enemy::Draw() {
	if (hp <= 0) return;
	GBDraw();
}

bool Enemy::GetLive() {
	if (hp > 0) {
		return true;
	}
	return false;
}

bool Enemy::HitCheck() {
	/*vector<Pole*> pole = Game::GetInstance()->GetObjects<Pole>();
	Collision::Sphere balCollision = { m_Position, radius };
	if (pole[0]->GetState() == 1) {
		if (CheckHit(pole[0]->hitbox, balCollision)) {
			vector<Player*> player = Game::GetInstance()->GetObjects<Player>();
			hitbackrotation = -1.0f * player[0]->GetForwardVector();
			Damage(pole[0]->atk);
			Sound::GetInstance()->Play(SOUND_SE_SWORDHIT);
			return true;
		}
	}
	vector<Arrow*> arrow = Game::GetInstance()->GetObjects<Arrow>();
	for (auto& ar : arrow) {
		if (ar->GetId() == 1) {
			if (CheckHit(ar->hitbox, balCollision)) {
				hitbackrotation = ar->GetForwardVector();
				Damage(ar->GetAtk());
				Sound::GetInstance()->Play(SOUND_SE_ARROWHIT);
				return true;
			}
		}
	}*/
	return false;
}

void Enemy::SetStageSize(DirectX::XMFLOAT2 _stagesize) {
	stagesize = _stagesize;
}