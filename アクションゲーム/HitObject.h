#pragma once
#include "Object.h"
#include "ICollider.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "Material.h"
#include "Collision.h"

//当たり判定のあるオブジェクト(壁などの障害物)クラス
class HitObject :
    public Object, public ICollider
{
//protected:
//	//速度
//	DirectX::SimpleMath::Vector3 m_Velocity = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
//
//	//加速度
//	DirectX::SimpleMath::Vector3 m_Acceralation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
//
//	// 描画の為の情報（メッシュに関わる情報）
//	MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数
//
//	// 描画の為の情報（見た目に関わる部分）
//	std::vector<std::unique_ptr<Material>> m_Materiales;
//	std::vector<SUBSET> m_subsets;
//	std::vector<std::unique_ptr<Texture>> m_Textures; // テクスチャ
//
//	float m_Velocity_f = 0.0; // 現在の移動速度（前進後退）
//	DirectX::SimpleMath::Vector3 oldPos;
//	static constexpr float ForwardVelocity = 0.5f; // 前進速度定数
//	static constexpr float BackwardVelocity = 0.5f; // 後退速度定数
//	static constexpr float LeftRightVelocity = 0.5f; // 左右移動速度定数
//
//	const float gravity = 0.07f; //重力
//
//	//前向きベクトル変数
//	DirectX::SimpleMath::Vector3 m_ForwardVector = { 0.0f,0.0f,1.0f };//初期ベクトル
//
//public:
//
//	HitObject();
//	HitObject(Camera* cam); // コンストラクタ
//	~HitObject();//デストラクタ
//
//	void Init(std::u8string modelfilename);
//	void Update();
//	void Draw();
//	void Uninit();
//	bool CheckGround();
//
//	// 状態の設定・取得
//	void SetState(int s);
//	int GetState();
//
//	Collision::ColliderVariant GetCollision();
//	DirectX::SimpleMath::Vector3 GetForwardVector();
//
//	void SetColor(const DirectX::SimpleMath::Vector4& color);//色を変える

};

