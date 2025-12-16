#pragma once
#include "Golfball.h"
#include <vector>
class Arrow;
class Pole;
class Bullet;

class Boss :
    public GolfBall
{
private:
	//pimplパターン、実装を隠す
	//Implクラスの宣言
	class Impl;
	std::unique_ptr<Impl> impl;
public:
	Boss(Camera * cam); // コンストラクタ
	~Boss();//デストラクタ

	void Init();
	void Update();
	void Draw();
	void Damage(int _atk);

	void SetDEF(int _def);
	void ShotBullet();
	bool GetLive();
	int GetHP();
	Pole* GetWeapon();
	void HitObject(Object* ob) override {
		ob->OnHit(this);
	}
	void OnHit(Pole* pole);
	void OnHit(Bullet* bu);
	void OnHit(TestCube* cube);
};

