#pragma once
#include "EffectBase.h"
#include "MeshRenderer.h"
#include "Texture.h"
#include "Material.h"
class EffectObject :
	public EffectBase
{
public:
    // 描画の為の情報（メッシュに関わる情報）
    MeshRenderer m_MeshRenderer; // 頂点バッファ・インデックスバッファ・インデックス数

    // 描画の為の情報（見た目に関わる部分）
    std::vector<std::unique_ptr<Material>> m_Materiales;
    std::vector<SUBSET> m_subsets;
    std::vector<std::unique_ptr<Texture>> m_Textures; // テクスチャ

    void Init(std::u8string model, std::u8string tex);
    void Update();
    void Draw();
    void Uninit();

};

