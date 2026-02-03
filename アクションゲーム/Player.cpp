#include "Player.h"
#include "PlayerImpl.h"

Player::Player(Camera* cam)
    : BallObject(cam)
    , impl(std::make_unique<Impl>(cam, this))
{
}

Player::~Player() = default;

void Player::Init() { impl->Init(); }
void Player::Update() { impl->Update(); }
void Player::Draw() { impl->Draw(); }
void Player::Uninit() { impl->Uninit(); }

void Player::SetGauge() { impl->SetGauge(); }

void Player::SetDemoMode(const bool _demomode) { impl->SetDemoMode(_demomode); }

int Player::GetHP() { return impl->GetHP(); }
Sword* Player::GetWeapon() { return impl->GetWeapon(); }
std::vector<Texture2D*> Player::GetGauge() { return impl->GetGauge(); }

void Player::OnHit(Enemy* en) { impl->OnHit(en); }
void Player::OnHit(Boss* bo) { impl->OnHit(bo); }
void Player::OnHit(Sword* po) { impl->OnHit(po); }
void Player::OnHit(Bullet* bu) { impl->OnHit(bu); }
void Player::OnHit(Projectile* pr) { impl->OnHit(pr); }
void Player::OnHit(TestCube* cube) { impl->OnHit(cube); }
