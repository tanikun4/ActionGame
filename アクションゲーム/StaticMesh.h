#pragma once

#include	<simplemath.h>
#include	<string>
#include	<vector>
#include	<memory>
#include	"Texture.h"
#include	"Mesh.h"
#include	"renderer.h"

class StaticMesh : public Mesh {
public:
	void Load(std::string filename, std::string texturedirectory="");

	const std::vector<MATERIAL>& GetMaterials() {
		return m_materials;
	}

	const std::vector<SUBSET>& GetSubsets() {
		return m_subsets;
	}

	const std::vector<std::string>& GetTextureNames() {
		return m_texturenames;
	}

	std::vector<std::unique_ptr<Texture>> GetTextures() {
		return std::move(m_textures);
	}

	const DirectX::SimpleMath::Vector3 const GetBaseSize() { return m_BaseSize; }
	const DirectX::SimpleMath::Vector3 const GetBoundsMin() { return m_BoundsMin; }
	const DirectX::SimpleMath::Vector3 const GetBoundsMax() { return m_BoundsMax; }

private:
	DirectX::SimpleMath::Vector3 m_BoundsMin;
	DirectX::SimpleMath::Vector3 m_BoundsMax;
	DirectX::SimpleMath::Vector3 m_BaseSize; // max - min
	std::vector<MATERIAL> m_materials;					// マテリアル情報
	std::vector<std::string> m_texturenames;			// テクスチャ名
	std::vector<SUBSET> m_subsets;						// サブセット情報
	std::vector<std::unique_ptr<Texture>>	m_textures;	// テクスチャ群
};