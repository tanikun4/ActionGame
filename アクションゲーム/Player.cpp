#include "Player.h"
#include "PlayerImpl.h"

Player::Player(Camera* cam)
    : GolfBall(cam)
    , impl(std::make_unique<Impl>(cam, this))
{
}

Player::~Player() = default;

void Player::Init() { impl->Init(); }
void Player::Update() { impl->Update(); }
void Player::Uninit() { impl->Uninit(); }

void Player::SetDemoMode(bool _demomode) { impl->SetDemoMode(_demomode); }

int Player::GetHP() { return impl->GetHP(); }
Pole* Player::GetWeapon() { return impl->GetWeapon(); }

void Player::OnHit(Boss* bo) { impl->OnHit(bo); }
void Player::OnHit(Pole* po) { impl->OnHit(po); }
void Player::OnHit(Bullet* bu) { impl->OnHit(bu); }
void Player::OnHit(Projectile* pr) { impl->OnHit(pr); }
void Player::OnHit(TestCube* cube) { impl->OnHit(cube); }
