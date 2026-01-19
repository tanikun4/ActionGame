#pragma once
#include "Object.h"
#include "ICollider.h"
#include "Collision.h"
#include "Texture.h"
#include "Material.h"
#include "MeshRenderer.h"

class Weapon :
    public Object , public ICollider
{
protected:
    //速度
    DirectX::SimpleMath::Vector3 m_Velocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

    float m_Velocity_f = 0.0; // 現在の移動速度

    //前向きベクトル変数
    DirectX::SimpleMath::Vector3 m_ForwardVector = { 0.0f,0.0f,1.0f };//初期ベクトル

    // 描画の為の情報（メッシュに関わる情報）
    MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

    // 描画の為の情報（メッシュに関わる情報）
    IndexBuffer	 m_IndexBuffer; // インデックスバッファ
    VertexBuffer<VERTEX_3D>	m_VertexBuffer; // 頂点バッファ

    // 描画の為の情報（見た目に関わる部分）
    std::vector<std::unique_ptr<Material>> m_Materiales;
    std::vector<SUBSET> m_subsets;
    std::vector<std::unique_ptr<Texture>> m_Textures; // テクスチャ


    DirectX::SimpleMath::Vector3 m_offset{};//位置の補正値

    DirectX::SimpleMath::Vector3 offset_debug{};//位置の補正値(デバッグ用)


    DirectX::SimpleMath::Vector3 angle_debug{};//角度の補正値(デバッグ用)

    int atk = 1;//攻撃力
    int framecount = 0;//タイマー変数
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

	void SetOffset(DirectX::SimpleMath::Vector3 off) { m_offset = off; }

	void SetOffsetDebug(DirectX::SimpleMath::Vector3 off) { offset_debug = off; }

    void SetAngleDebug(DirectX::SimpleMath::Vector3 an) { angle_debug = an; }
};

