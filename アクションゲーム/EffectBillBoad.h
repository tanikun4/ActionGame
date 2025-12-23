#pragma once
#include "EffectBase.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "Material.h"

// EffectBillBoadクラス、2Dエフェクト用のテクスチャ表示クラス
class EffectBillBoad: public EffectBase
{
protected:
	// 描画の為の情報（メッシュに関わる情報）
	IndexBuffer* m_IndexBuffer; // インデックスバッファ
	VertexBuffer<VERTEX_3D>* m_VertexBuffer; // 頂点バッファ

	std::vector<VERTEX_3D> m_Vertices;//頂点データ

	// 描画の為の情報（見た目に関わる部分）
	Texture* m_Texture; // テクスチャ
	std::unique_ptr<Material> m_Materiale; //マテリアル(こちらは個別で持つ)

	//DirectX::SimpleMath::Vector4 m_color{ 1.0f,1.0f,1.0f,1.0f };
	// UV座標の情報
	float m_NumU = 1;
	float m_NumV = 1;
	float m_SplitX = 1;
	float m_SplitY = 1;

	int m_animframe = 4; //アニメーション遷移フレーム

public:

	EffectBillBoad(Camera* cam); // コンストラクタ
	EffectBillBoad();
	~EffectBillBoad(); // デストラクタ

	void Init(LoadedEffectData& data, SharedEffect2DData& shared_data, int _maxlife, 
		DirectX::SimpleMath::Vector3 _pos_amount, int _pos_changeframe,
		DirectX::SimpleMath::Vector3 _rot_amount, int _rot_changeframe,
		DirectX::SimpleMath::Vector3 _scale_amount, int _scale_changeframe,
		bool _endless);//本来の初期化処理
	void Init() {};//オーバーライド用ダミー定義
	void Update();
	void Draw();
	void Uninit();

	// テクスチャを指定
	void SetTexture(const char* imgname);

	// UV座標を指定
	void SetUV(const float& nu, const float& nv, const float& sx, const float& sy);

	// 色を指定
	void SetColor(const DirectX::SimpleMath::Vector4& color);
};
