#include "EffectManager.h"
#include "Game.h"
#include "StaticMesh.h"
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
		string VSshaderName;
		string PSshaderName;
        int u = 1;
        int v = 1;// テクスチャのUV分割数
	};

	// 読み込むデータの配列、ここにエフェクトリソースを追加していく
	const EffectLoadData g_EffectResources[] = {
		{"assets/texture/gorufu", "assets/model/gorufu/GolfBall_v2.fbx"},
        {"assets/texture/2DEffect/issen_one_2.png","","","", 1,1},
        {"assets/texture/2DEffect/honoo.png","","","",5,24},
        {"assets/texture/2DEffect/hibana.png","","","",5,4},
        {"assets/texture/2DEffect/kemuri_syou.png","","","",5,14},
		{"assets/texture/2DEffect/kemuri_dai.png","","","",5,22},
        {"assets/texture/2DEffect/title_ugoku.png","","","",5,14},

	};

	m_Instance = make_unique<EffectManager>();

    for(auto& g : g_EffectResources)
    {
        m_Instance->m_LoadData.emplace_back(m_Instance->LoadEffect(
            g.textureName, 
            g.modelName, 
            g.u,
            g.v,
            g.VSshaderName,
            g.PSshaderName));
	}

	m_Instance->m_Camera = &Game::GetInstance()->GetCamera();

    //3Dエフェクトのプールを確保
    for (int i = 0; i < EFFECT_POOLSIZE_3D; ++i) {
        m_Instance->m_Effects3D.emplace_back(new EffectObject(m_Instance->m_Camera));
    }

    //2Dエフェクトのプールを確保
    for (int i = 0; i < EFFECT_POOLSIZE_2D; ++i) {
        //2Dのクラスが完成したら書く
        m_Instance->m_Effects2D.emplace_back(new EffectBillBoad(m_Instance->m_Camera));
    }

	m_Instance->m_Shared2D_Data = m_Instance->Init2D();

}

SharedEffect2DData EffectManager::Init2D()
{
    //2Dエフェクト共通データ初期化処理
    
	SharedEffect2DData data;

    // 頂点データ
    std::vector<VERTEX_3D> vertices;

    vertices.resize(4);

    vertices[0].position = Vector3(-0.5f, 0.5f, 0);
    vertices[1].position = Vector3(0.5f, 0.5f, 0);
    vertices[2].position = Vector3(-0.5f, -0.5f, 0);
    vertices[3].position = Vector3(0.5f, -0.5f, 0);

    vertices[0].color = Color(1, 1, 1, 1);
    vertices[1].color = Color(1, 1, 1, 1);
    vertices[2].color = Color(1, 1, 1, 1);
    vertices[3].color = Color(1, 1, 1, 1);

    vertices[0].uv = Vector2(0, 0);
    vertices[1].uv = Vector2(1, 0);
    vertices[2].uv = Vector2(0, 1);
    vertices[3].uv = Vector2(1, 1);

    // 頂点バッファ生成
    data.m_2DVertexBuffer = std::make_unique<VertexBuffer<VERTEX_3D>>();
    data.m_2DVertexBuffer->Create(vertices);

    // インデックスバッファ生成
    std::vector<unsigned int> indices;
    indices.resize(4);

    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 2;
    indices[3] = 3;

    // インデックスバッファ生成
    data.m_2DIndexBuffer = std::make_unique<IndexBuffer>();
    data.m_2DIndexBuffer->Create(indices);

    return data;
}

// エフェクトリソース読込関数
LoadedEffectData EffectManager::LoadEffect(
    const string textureName,
    const string modelName,
    const int texDivX,
    const int texDivY,
    string VSshaderName,
    string PSshaderName)
{
    LoadedEffectData data;

	// モデル名が空でない場合、3Dエフェクト用の読み込みを行う
    if (!modelName.empty())//3Dエフェクト用読み込み
    {

        //  メッシュ読み込み
        auto mesh = make_unique<StaticMesh>();

        mesh->Load(modelName, textureName);
        data.mesh = move(mesh);


        // マテリアル取得
        std::vector<MATERIAL> mats = data.mesh->GetMaterials();

        for (auto& m : mats)
        {
            auto mat = make_unique<Material>();
            mat->Create(m);
            data.materials.emplace_back(move(mat));
        }

        // テクスチャ取得
        auto loadedTextures = data.mesh->GetTextures();

        for (auto& t : loadedTextures)
        {
            // StaticMesh内は unique_ptr<Texture> を返すなら moveするだけ
            data.textures.emplace_back(move(t));
        }

		// シェーダー名が空の場合、デフォルトシェーダー名を設定
        if(VSshaderName.empty()) 
        { //VS
			VSshaderName = "shader/litTextureVS.hlsl"; //3Dデフォルトシェーダー名 VS
		}

        if (PSshaderName.empty()) 
        { //PS
            PSshaderName = "shader/litTexturePS.hlsl"; //3Dデフォルトシェーダー名 PS
        }
    }
	else {//2Dエフェクト用読み込み
		// テクスチャ単体の読み込み
        auto tex = std::make_unique<Texture>();
        tex->Load(textureName);
        data.textures.emplace_back(std::move(tex));

        // シェーダー名が空の場合、デフォルトシェーダー名を設定
        if (VSshaderName.empty())
        { //VS
            VSshaderName = "shader/unlitTextureVS.hlsl"; //2Dデフォルトシェーダー名 VS
        }

        if (PSshaderName.empty())
        { //PS
            PSshaderName = "shader/unlitTexturePS.hlsl"; //2Dデフォルトシェーダー名 PS
        }
    }

    // シェーダー生成
    data.shader = make_unique<Shader>();
    data.shader->Create(VSshaderName, PSshaderName);

	// テクスチャのUV分割数を保存
    data.texture_uv = Int2(texDivX, texDivY);
    
    return data;
}

// 更新
void EffectManager::Update()
{
    //3D
    for (auto& obj : m_Instance->m_Effects3D) {
		if(obj->GetLive())
            obj->Update();
    }

    //2D
    for (auto& obj : m_Instance->m_Effects2D) {
        if (obj->GetLive())
            obj->Update();
    }
}

// 描画
void EffectManager::Draw()
{
    //3D
    for (auto& obj : m_Instance->m_Effects3D) {
        if (obj->GetLive())
            obj->Draw();
    }

    //2D
    for (auto& obj : m_Instance->m_Effects2D) {
        if (obj->GetLive())
            obj->Draw();
    }
}

// 終了処理
void EffectManager::Uninit()
{

    // m_Effects に格納された全ての EffectObject を削除
    for (auto& obj : m_Instance->m_Effects3D) {
		obj->Uninit();
        delete obj;
    }
    m_Instance->m_Effects3D.clear();

    for (auto& obj : m_Instance->m_Effects2D) {
        obj->Uninit();
        delete obj;
    }
    m_Instance->m_Effects2D.clear();

    // m_LoadData は unique_ptr が含まれているので自動解放される
    m_Instance->m_LoadData.clear();

}

void EffectManager::EffectsReset()
{
    //3Dエフェクトリセット
    for (auto& obj : m_Instance->m_Effects3D) {
		obj->SetLive(false);
    }
    //2Dエフェクトリセット
    for (auto& obj : m_Instance->m_Effects2D) {
        obj->SetLive(false);
    }
}

//エフェクト再生関数
void EffectManager::Play(int _id,
    int _maxlife,
    Vector3 _pos,
	Vector3 _rot,
    Vector3 _first_scale,
    Vector3 _ta_scale,
    int _change_flame)
{

    ////2D部分制作後、プール方式に変更する
    //if (m_Instance->m_LoadData[_id].mesh != nullptr) {

    //    for (auto& e : m_Instance->m_Effects3D) {
    //        //非生存エフェクトオブジェクトを発見
    //        if (!e->GetLive()) {
    //            //ロード済みデータと引数を使い、エフェクトオブジェクト初期化
    //            e->SetPosition(_pos);//位置を設定
    //            e->SetRotation(_rot);//回転を設定
    //            e->SetScale(_first_scale);//最初のスケールを設定後、Initでスケール変化率を計算するので先に行う必要あり
				//e->Init(m_Instance->m_LoadData[_id], _maxlife, _ta_scale, _change_flame);//3Dエフェクト用Initを呼ぶ
				//break;//1つだけ再生したいのでループを抜ける
    //        }
    //    }
    //}
    //else {
    //    for (auto& e : m_Instance->m_Effects2D) {
    //        //非生存エフェクトオブジェクトを発見
    //        if (!e->GetLive()) {
    //            //ロード済みデータと引数を使い、エフェクトオブジェクト初期化
    //            e->SetPosition(_pos);//位置を設定
    //            e->SetRotation(_rot);//回転を設定
    //            e->SetScale(_first_scale);//最初のスケールを設定後、Initでスケール変化率を計算するので先に行う必要あり
    //            e->Init(m_Instance->m_LoadData[_id], m_Instance->m_Shared2D_Data, _maxlife, _ta_scale,_change_flame);//2Dエフェクト用Initを呼ぶ
    //            break;//1つだけ再生したいのでループを抜ける
    //        }
    //    }
    //}
}

//エフェクト再生関数、EffectParams版
void EffectManager::Play(int _id,
    EffectParams _param)
{

    if (m_LoadData[_id].mesh != nullptr) {// 3D初期化

        for (auto& e : m_Effects3D) {
            //非生存エフェクトオブジェクトを発見
            if (!e->GetLive()) {
                //ロード済みデータと引数を使い、エフェクトオブジェクト初期化
                e->SetPosition(_param.pos);//位置を設定
                e->SetRotation(_param.rot);//回転を設定
                e->SetScale(_param.scale);//最初のスケールを設定後、Initでスケール変化率を計算するので先に行う必要あり
                e->Init(m_LoadData[_id], _param.maxLife, 
                    _param.pos_amount, _param.change_posFrame,
                    _param.rot_amount, _param.change_rotFrame,
                    _param.scale_amount, _param.change_scaleFrame,
                    _param.endless);//3Dエフェクト用Initを呼ぶ
                break;//1つだけ再生したいのでループを抜ける
            }
        }
    }
    else { // 2D初期化
        for (auto& e : m_Effects2D) {
            //非生存エフェクトオブジェクトを発見
            if (!e->GetLive()) {
                //ロード済みデータと引数を使い、エフェクトオブジェクト初期化
                e->SetPosition(_param.pos);//位置を設定
                e->SetRotation(_param.rot);//回転を設定
                e->SetScale(_param.scale);//最初のスケールを設定後、Initでスケール変化率を計算するので先に行う必要あり
                e->Init(m_LoadData[_id], m_Shared2D_Data, _param.maxLife,
                    _param.pos_amount, _param.change_posFrame,
                    _param.rot_amount, _param.change_rotFrame,
                    _param.scale_amount, _param.change_scaleFrame,
                    _param.endless);//2Dエフェクト用Initを呼ぶ
                break;//1つだけ再生したいのでループを抜ける
            }
        }
    }
}

// _posからカメラ方向に_dist分近づいた位置を返す
Vector3 EffectManager::ToCameraEffectPos(Vector3 _pos, float _dist) {
    //エフェクト再生の位置調整
    Vector3 forward = m_Instance->m_Camera->GetForwardVector();
    Vector3 toCamera = m_Instance->m_Camera->GetPosition() - _pos;
    toCamera.Normalize();

    // forward を toCamera 方向へ投影し、横ズレ除去
    Vector3 adjustedForward = toCamera * forward.Dot(toCamera);

    // 長さを維持したい場合は再正規化
    adjustedForward.Normalize();

    // 対象座標からdist分前方にずらすした座標を返す
    Vector3 pos = _pos - adjustedForward * _dist;
    return pos;
}

void EffectManager::SetCamera(Camera* cam)
{
    m_Camera = cam;
    for(auto& e : m_Effects3D)
    {
        e->SetCamera(cam);
	}
    for(auto& e : m_Effects2D)
    {
        e->SetCamera(cam);
	}
}

// インスタンスを取得
EffectManager* EffectManager::GetInstance()
{
	return m_Instance.get();
}

