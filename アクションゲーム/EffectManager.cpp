#include "EffectManager.h"
#include "Game.h"
using namespace std;

unique_ptr<EffectManager> EffectManager::m_Instance;

// コンストラクタ
EffectManager::EffectManager()
{

}

// デストラクタ
EffectManager::~EffectManager()
{
	//m_Instance = nullptr;
}

// 初期化
void EffectManager::Init()
{
	//初回読込にしか使わないのでここに記述

	// エフェクト読込用の構造体
	struct EffectLoadData {
		std::string textureName;// テクスチャ名、3Dの場合はテクスチャディレクトリ名
		std::string modelName;
		int texDivX = 1;
		int texDivY = 1;
		std::string VSshaderName = "shader/litTextureVS.hlsl";
		std::string PSshaderName = "shader/litTexturePS.hlsl";
	};

	const EffectLoadData g_EffectResources[] = {
		{"assets/texture/gorufu", "assets / model / gorufu / GolfBall_v2.fbx"}
	};

	m_Instance = make_unique<EffectManager>();
}

void EffectManager::LoadEffect(std::string modelfilename, std::string texturefilename)
{
	LoadEffectData data;

	// メッシュ読み込み
	StaticMesh staticmesh;

	//3Dモデルデータ
	std::u8string modelFile = modelfilename;

	//テクスチャディレクトリ
	std::string texDirectory = "assets/texture/gorufu";

	//Meshを読み込む
	std::string tmpStr1(reinterpret_cast<const char*>(modelFile.c_str()), modelFile.size());
	staticmesh.Load(tmpStr1, texDirectory);

	m_MeshRenderer.Init(staticmesh);

	// シェーダオブジェクト生成
	data.shader->Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	// サブセット情報取得
	m_subsets = staticmesh.GetSubsets();

	// テクスチャ情報取得
	m_Textures = staticmesh.GetTextures();

	// マテリアル情報取得	
	std::vector<MATERIAL> materials = staticmesh.GetMaterials();

	// マテリアル数分ループ
	for (int i = 0; i < materials.size(); i++)
	{
		// マテリアルオブジェクト生成
		std::unique_ptr<Material> m = std::make_unique<Material>();

		// マテリアル情報をセット
		m->Create(materials[i]);

		// マテリアルオブジェクトを配列に追加
		m_Materiales.push_back(std::move(m));
	}
}

// 更新
void EffectManager::Update()
{

}

// 描画
void EffectManager::Draw()
{

}

// 終了処理
void EffectManager::Uninit()
{

}

// インスタンスを取得
EffectManager* EffectManager::GetInstance()
{
	return m_Instance.get();
}
