#pragma once
#include "EffectBase.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "Material.h"

class EffectObject :
	public EffectBase
{
public:
    EffectObject();
    EffectObject(Camera* cam);
    ~EffectObject();

    // 描画の為の情報（メッシュに関わる情報）
    MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

    // 描画の為の情報（見た目に関わる部分）
    std::vector<Material> m_Materiales;
    std::vector<SUBSET> m_subsets;
    std::vector<Texture*> m_Textures; // テクスチャ

    void Init(LoadedEffectData& data, int _maxlife, DirectX::SimpleMath::Vector3 ta_scale = { -1,-1,-1 }, int _change_flame = 0);
	void Init() {};//オーバーライド用ダミー
    void Update();
    void Draw();
    void Uninit();

};

