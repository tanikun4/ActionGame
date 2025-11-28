#include "Boss.h"
#include "BossImpl.h"

using namespace std;
using namespace DirectX::SimpleMath;

Boss::Boss(Camera* cam)
	: GolfBall(cam)
	, impl(std::make_unique<Impl>(cam, this))
{

}


Boss::~Boss() = default;

void Boss::Init() { impl->Init(); }

void Boss::Update() { impl->Update(); }

void Boss::Draw() { impl->Draw(); }

bool Boss::GetLive() { return impl->GetLive(); }

void Boss::Damage(int atk) { impl->Damage(atk); }

void Boss::ShotBullet() { impl->ShotBullet(); }


int Boss::GetHP() { return impl->GetHP(); }

Pole* Boss::GetWeapon() { return impl->GetWeapon(); }

void Boss::OnHit(Pole* po) { impl->OnHit(po); }

void Boss::OnHit(Bullet* bu) { impl->OnHit(bu); }

void Boss::OnHit(TestCube* cube) { impl->OnHit(cube); }