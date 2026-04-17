#include	"Cube.h"

using namespace DirectX::SimpleMath;

// コンストラクタ
Cube::Cube(Camera* cam) : Object(cam)
{

}

Cube::Cube()
{

}

// デストラクタ
Cube::~Cube()
{

}

void Cube::Init() {
	InitCube();
}

//=======================================
//初期化処理
//=======================================
void Cube::InitCube()
{
	// 頂点データ
	std::vector<VERTEX_3D>	vertices;

	vertices.resize(24);

	vertices[0].position = Vector3(-10, 10,  10);
	vertices[1].position = Vector3( 10,  10,  10);
	vertices[2].position = Vector3(-10, 10, -10);
	vertices[3].position = Vector3( 10,  10, -10);
	
	vertices[0].color = Color(1, 1, 1, 1);
	vertices[1].color = Color(1, 1, 1, 1);
	vertices[2].color = Color(1, 1, 1, 1);
	vertices[3].color = Color(1, 1, 1, 1);
	//1
	vertices[0].uv = Vector2(0, 0);
	vertices[1].uv = Vector2(0.33f, 0);
	vertices[2].uv = Vector2(0, 0);
	vertices[3].uv = Vector2(0.33f, 0.5f);

	vertices[0].normal = Vector3(0, 1, 0);
	vertices[1].normal = Vector3(0, 1, 0);
	vertices[2].normal = Vector3(0, 1, 0);
	vertices[3].normal = Vector3(0, 1, 0);

	vertices[4].position = Vector3(-10, 10, -10);
	vertices[5].position = Vector3(10, 10, -10);
	vertices[6].position = Vector3(-10, -10, -10);
	vertices[7].position = Vector3(10, -10, -10);

	vertices[4].color = Color(1, 1, 1, 1);
	vertices[5].color = Color(1, 1, 1, 1);
	vertices[6].color = Color(1, 1, 1, 1);
	vertices[7].color = Color(1, 1, 1, 1);

	//3
	vertices[4].uv = Vector2(0.66f, 0);
	vertices[5].uv = Vector2(1, 0);
	vertices[6].uv = Vector2(0.66f, 0.5f);
	vertices[7].uv = Vector2(1, 0.5f);

	vertices[4].normal = Vector3(0, 0, -1);
	vertices[5].normal = Vector3(0, 0, -1);
	vertices[6].normal = Vector3(0, 0, -1);
	vertices[7].normal = Vector3(0, 0, -1);

	vertices[8].position = Vector3(-10, -10, 10);
	vertices[9].position = Vector3(-10, 10, 10);
	vertices[10].position = Vector3(-10, -10, -10);
	vertices[11].position = Vector3(-10, 10, -10);

	vertices[8].color = Color(1, 1, 1, 1);
	vertices[9].color = Color(1, 1, 1, 1);
	vertices[10].color = Color(1, 1, 1, 1);
	vertices[11].color = Color(1, 1, 1, 1);
	
	//2
	vertices[8].uv = Vector2(0.33f, 0);
	vertices[9].uv = Vector2(0.66f, 0);
	vertices[10].uv = Vector2(0.33f, 0.5f);
	vertices[11].uv = Vector2(0.66f, 0.5f);

	vertices[8].normal = Vector3(-1, 0, 0);
	vertices[9].normal = Vector3(-1, 0, 0);
	vertices[10].normal = Vector3(-1, 0, 0);
	vertices[11].normal = Vector3(-1, 0, 0);

	vertices[12].position = Vector3(10, -10, 10);
	vertices[13].position = Vector3(10, 10, 10);
	vertices[14].position = Vector3(-10, -10, 10);
	vertices[15].position = Vector3(-10, 10, 10);

	vertices[12].color = Color(1, 1, 1, 1);
	vertices[13].color = Color(1, 1, 1, 1);
	vertices[14].color = Color(1, 1, 1, 1);
	vertices[15].color = Color(1, 1, 1, 1);

	//4
	vertices[12].uv = Vector2(0, 0.5f);
	vertices[13].uv = Vector2(0.33f, 0.5f);
	vertices[14].uv = Vector2(0, 1);
	vertices[15].uv = Vector2(0.33f, 1);

	vertices[12].normal = Vector3(0, 0, 1);
	vertices[13].normal = Vector3(0, 0, 1);
	vertices[14].normal = Vector3(0, 0, 1);
	vertices[15].normal = Vector3(0, 0, 1);

	vertices[16].position = Vector3(10, -10, -10);
	vertices[17].position = Vector3(10, 10, -10);
	vertices[18].position = Vector3(10, -10, 10);
	vertices[19].position = Vector3(10, 10, 10);

	vertices[16].color = Color(1, 1, 1, 1);
	vertices[17].color = Color(1, 1, 1, 1);
	vertices[18].color = Color(1, 1, 1, 1);
	vertices[19].color = Color(1, 1, 1, 1);

	//5
	vertices[16].uv = Vector2(0.33f, 0.5f);
	vertices[17].uv = Vector2(0.66f, 0.5f);
	vertices[18].uv = Vector2(0.33f, 1);
	vertices[19].uv = Vector2(0.66f, 1);

	vertices[16].normal = Vector3(1, 0, 0);
	vertices[17].normal = Vector3(1, 0, 0);
	vertices[18].normal = Vector3(1, 0, 0);
	vertices[19].normal = Vector3(1, 0, 0);

	vertices[20].position = Vector3(10, -10, 10);
	vertices[21].position = Vector3(-10, -10, 10);
	vertices[22].position = Vector3(10, -10, -10);
	vertices[23].position = Vector3(-10, -10, -10);

	vertices[20].color = Color(1, 1, 1, 1);
	vertices[21].color = Color(1, 1, 1, 1);
	vertices[22].color = Color(1, 1, 1, 1);
	vertices[23].color = Color(1, 1, 1, 1);

	//6
	vertices[20].uv = Vector2(0.66f, 0.5f);
	vertices[21].uv = Vector2(0.66f, 1);
	vertices[22].uv = Vector2(1, 0.5f);
	vertices[23].uv = Vector2(1, 1);

	vertices[20].normal = Vector3(0, -1, 0);
	vertices[21].normal = Vector3(0, -1, 0);
	vertices[22].normal = Vector3(0, -1, 0);
	vertices[23].normal = Vector3(0, -1, 0);

	// UV座標の設定、上記のはサイコロ用の物なので、普通に0～1で割り当てる場合は以下のようになる
	for (int face = 0; face < 6; ++face)
	{
		int base = face * 4; // 1 面につき 4 頂点
		vertices[base + 0].uv = Vector2(0, 0);
		vertices[base + 1].uv = Vector2(1, 0);
		vertices[base + 2].uv = Vector2(0, 1);
		vertices[base + 3].uv = Vector2(1, 1);
	}

	// 頂点バッファ生成
	m_VertexBuffer.Create(vertices);

	m_Vertices = vertices;//頂点データを保存

	// インデックスバッファ生成
	std::vector<unsigned int> indices;
	indices.resize(36);

	indices = {
		0, 1, 2,
		1, 3, 2,
		4, 5, 6,
		5, 7, 6,
		8, 9, 10,
		9, 11, 10,
		12, 13, 14,
		13, 15, 14,
		16, 17, 18,
		17, 19, 18,
		20, 21, 22,
		21, 23, 22,
	};

	// インデックスバッファ生成
	m_IndexBuffer.Create(indices);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/unlitTextureVS.hlsl", "shader/unlitTexturePS.hlsl");

	//m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");//マテリアルを持つモデルの場合はこっち

	// テクスチャロード
	bool sts = m_Texture.Load("assets/texture/wall/wall2.jpg");//壁のテクスチャ
	assert(sts == true);

}


void Cube::Update() {
	UpdateCube();
}

//=======================================
//更新処理
//=======================================
void Cube::UpdateCube()
{


	m_fLength[0] = m_Scale.x * m_length.x;
	m_fLength[1] = m_Scale.y * m_length.y;
	m_fLength[2] = m_Scale.z * m_length.z;
	
	// 透明オフ → オンになった瞬間だけ発動
	if (m_Invisible && !m_PrevInvisible) {
		inviFrame = 60;
	}

	// タイマー減少
	if (inviFrame > 0) {
		--inviFrame;
	}

	// 状態保存
	m_PrevInvisible = m_Invisible;

}

void Cube::Draw()
{
	if (m_Invisible || inviFrame > 0) return;// 透明状態のときは描画しない、最低でも60フレームは透明状態になるようにする
	DrawCube();
}

//=======================================
//描画処理
//=======================================
void Cube::DrawCube()
{
	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll( m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation( m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale( m_Scale.x, m_Scale.y, m_Scale.z);

	Matrix worldmtx;
	worldmtx = s * r * t;
	Renderer::SetWorldMatrix(&worldmtx); // GPUにセット

	// 描画の処理
	ID3D11DeviceContext* devicecontext;
	devicecontext = Renderer::GetDeviceContext();

	// トポロジーをセット（プリミティブタイプ）
	devicecontext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_Shader.SetGPU();
	m_VertexBuffer.SetGPU();
	m_IndexBuffer.SetGPU();
	m_Texture.SetGPU();

	// カメラの設定を指定
	m_Camera->SetCamera(0);

	devicecontext->DrawIndexed(
		36,							// 描画するインデックス数
		0,							// 最初のインデックスバッファの位置
		0);
}

//=======================================
//終了処理
//=======================================
void Cube::Uninit()
{

}

float Cube::GetLen(int elem) const {
	return m_fLength[elem];
}

Collision::ColliderVariant Cube::GetCollision() const {
	return Collision::OBB{ m_Position, m_Rotation, m_length * m_Scale};
}

Collision::OBB Cube::GetOBB() const {
	return Collision::OBB{ m_Position, m_Rotation, m_length * m_Scale};
}

void Cube::SetColor(const DirectX::SimpleMath::Vector4& color)
{
	for (auto& v : m_Vertices)
		v.color = color;
	m_VertexBuffer.Modify(m_Vertices);
}