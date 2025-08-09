#pragma once
#include "Object.h"
#include "TestCube.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "Material.h"
#include "Weapon.h"

//-----------------------------------------------------------------------------
// Poleクラス
//-----------------------------------------------------------------------------
class Pole :public Weapon
{
private:

	// 描画の為の情報（メッシュに関わる情報）
	MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

	// 描画の為の情報（見た目に関わる部分）
	std::vector<std::unique_ptr<Material>> m_Materiales;
	std::vector<SUBSET> m_subsets;
	std::vector<std::unique_ptr<Texture>> m_Textures; // テクスチャ

	int m_State = 0; // 0:非表示・1:方向選択・2:パワー選択
	int swing_time = 0;
public:

	Pole(Camera* cam); // コンストラクタ
	Pole(); // コンストラクタ
	~Pole(); // デストラクタ

	TestCube hitbox;
	int atk = 3;//攻撃力
	void Init();
	void Update();
	void Update(DirectX::SimpleMath::Vector3 position, float radius,DirectX::SimpleMath::Vector3 rotation);
	void Draw();
	void Uninit();

	// 位置の設定
	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::SimpleMath::Vector3 pos);
	
	void Swing();
	int GetState();//状態を返す
};

