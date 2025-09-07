#pragma once
#include "Object.h"
#include "TestCube.h"
#include "ICollider.h"
#include "Collision.h"

class Weapon :
    public Object , public ICollider
{
private:
    int atk;
    int flamecount;
	bool pl = false;//プレイヤーの武器かどうか
public:
    Weapon(Camera* cam);//コンストラクタ
    Weapon();
    ~Weapon();//デストラクタ
    void HitObject(Object* ob) override {
        ob->OnHit(this);
    }
    virtual Collision::ColliderVariant GetCollision();
    void SetAtk(int a) {
        atk = a;
    }
    int GetAtk() {
        return atk;
	}
    void SetPl(bool p) {
        pl = p;
	}
    bool GetPl() {
        return pl;
	}
};

