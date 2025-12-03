#include "EffectManager.h"
#include "Game.h"
using namespace std;
using namespace DirectX::SimpleMath;

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
		string textureName;// テクスチャ名、3Dの場合はテクスチャディレクトリ名
		string modelName;
		int texDivX = 1;
		int texDivY = 1;
		string VSshaderName = "shader/litTextureVS.hlsl";
		string PSshaderName = "shader/litTexturePS.hlsl";
	};

	const EffectLoadData g_EffectResources[] = {
		{"assets/texture/gorufu", "assets/model/gorufu/GolfBall_v2.fbx"}
	};

	m_Instance = make_unique<EffectManager>();

    for(auto& g : g_EffectResources)
    {
        m_Instance->m_LoadData.emplace_back(m_Instance->LoadEffect(
            g.textureName, 
            g.modelName, 
            g.texDivX,
            g.texDivY,
            g.VSshaderName,
            g.PSshaderName));
	}

	m_Instance->m_Camera = &Game::GetInstance()->GetCamera();
}

// エフェクトリソース読込関数
LoadedEffectData EffectManager::LoadEffect(
    const string textureName,
    const string modelName,
    const int texDivX,
    const int texDivY,
    const string VSshaderName,
    const string PSshaderName)
{
    LoadedEffectData data;

    //  メッシュ読み込み
    auto mesh = make_unique<StaticMesh>();

    mesh->Load(modelName,textureName);
    data.mesh = move(mesh);

    // テクスチャ取得
    auto loadedTextures = mesh->GetTextures();

    for (auto& t : loadedTextures)
    {
        // StaticMesh内は unique_ptr<Texture> を返すなら moveするだけ
        data.textures.emplace_back(move(t));
    }

    // マテリアル取得
    std::vector<MATERIAL> mats = mesh->GetMaterials();

    for (auto& m : mats)
    {
        auto mat = make_unique<Material>();
        mat->Create(m);
        data.materials.push_back(move(mat));
    }

    // シェーダー生成

    data.shader = make_unique<Shader>();
    data.shader->Create(VSshaderName, PSshaderName);

    return data;
}

// 更新
void EffectManager::Update()
{
    for (auto& obj : m_Instance->m_Effects) {
        obj->Update();
    }
}

// 描画
void EffectManager::Draw()
{
    for (auto& obj : m_Instance->m_Effects) {
       obj->Draw();
    }
}

// 終了処理
void EffectManager::Uninit()
{

    // m_Effects に格納された全ての EffectObject を削除
    for (auto& obj : m_Instance->m_Effects) {
		obj->Uninit();
        delete obj;
    }
    m_Instance->m_Effects.clear();

    // m_LoadData は unique_ptr が含まれているので自動解放される
    m_Instance->m_LoadData.clear();

}

//エフェクト再生関数
void EffectManager::Play(int _id,
    int _maxlife,
    Vector3 _pos,
	Vector3 _rot,
    Vector3 _first_scale,
    Vector3 _ta_scale)
{
    //エフェクトオブジェクト生成
    EffectObject* effect = new EffectObject(m_Instance->m_Camera);
    //ロード済みデータと引数を使い、エフェクトオブジェクト初期化
	effect->SetPosition(_pos);//位置を設定
	effect->SetRotation(_rot);//回転を設定
	effect->SetScale(_first_scale);//最初のスケールを設定後、Initでスケール変化率を計算するので先に行う必要あり
    effect->Init(m_Instance->m_LoadData[_id],_maxlife,_ta_scale);
    //エフェクトオブジェクト配列に追加
    m_Instance->m_Effects.emplace_back(effect);
}

// インスタンスを取得
EffectManager* EffectManager::GetInstance()
{
	return m_Instance.get();
}

