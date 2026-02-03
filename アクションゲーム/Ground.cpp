#include "Ground.h"
#include "stb_image.h"


using namespace DirectX::SimpleMath;

// コンストラクタ
Ground::Ground(Camera* cam) :Object(cam)
{

}

// デストラクタ
Ground::~Ground()
{

}

//=======================================
//初期化処理
//=======================================
void Ground::Init()
{

	m_Vertices.resize(6 * m_SizeX * m_SizeZ);

	// インデックスバッファ生成
	std::vector<unsigned int> indices;
	indices.resize(6 * m_SizeX * m_SizeZ);

	for (int z = 0; z < m_SizeZ; z++) {
		for (int x = 0; x < m_SizeX; x++) {
			
			int n = z * m_SizeZ * 6 + x * 6;
			m_Vertices[n + 0].position = Vector3(-0.5f + x - m_SizeX / 2, 0, 0.5f - z + m_SizeZ / 2);
			m_Vertices[n + 1].position = Vector3(0.5f + x - m_SizeX / 2, 0, 0.5f - z + m_SizeZ / 2);
			m_Vertices[n + 2].position = Vector3(-0.5f + x - m_SizeX / 2, 0, -0.5f - z + m_SizeZ / 2);
			m_Vertices[n + 3].position = Vector3(-0.5f + x - m_SizeX / 2, 0, -0.5f - z + m_SizeZ / 2);
			m_Vertices[n + 4].position = Vector3(0.5f + x - m_SizeX / 2, 0, 0.5f - z + m_SizeZ / 2);
			m_Vertices[n + 5].position = Vector3(0.5f + x - m_SizeX / 2, 0, -0.5f - z + m_SizeZ / 2);

			m_Vertices[n + 0].color = Color(1, 1, 1, 1);
			m_Vertices[n + 1].color = Color(1, 1, 1, 1);
			m_Vertices[n + 2].color = Color(1, 1, 1, 1);
			m_Vertices[n + 3].color = Color(1, 1, 1, 1);
			m_Vertices[n + 4].color = Color(1, 1, 1, 1);
			m_Vertices[n + 5].color = Color(1, 1, 1, 1);

			m_Vertices[n + 0].uv = Vector2(0, 0);
			m_Vertices[n + 1].uv = Vector2(1, 0);
			m_Vertices[n + 2].uv = Vector2(0, 1);
			m_Vertices[n + 3].uv = Vector2(0, 1);
			m_Vertices[n + 4].uv = Vector2(1, 0);
			m_Vertices[n + 5].uv = Vector2(1, 1);

			m_Vertices[n + 0].normal = Vector3(0, 1, 0);
			m_Vertices[n + 1].normal = Vector3(0, 1, 0);
			m_Vertices[n + 2].normal = Vector3(0, 1, 0);
			m_Vertices[n + 3].normal = Vector3(0, 1, 0);
			m_Vertices[n + 4].normal = Vector3(0, 1, 0);
			m_Vertices[n + 5].normal = Vector3(0, 1, 0);

			indices[n + 0] = n + 0;
			indices[n + 1] = n + 1;
			indices[n + 2] = n + 2;
			indices[n + 3] = n + 3;
			indices[n + 4] = n + 4;
			indices[n + 5] = n + 5;
		}
	}

	// 読み込む画像ファイルのパス
	const char* filename = "assets/texture/terain_taira.png";

	// 画像データを格納するポインタ
	unsigned char* imageData = nullptr;
	int width, height, channels;

	// グレースケール(1チャネル)で画像を読み込む
	imageData = stbi_load(filename, &width, &height, &channels, 1);
	if (imageData) {
		for (int z = 0; z < m_SizeZ; z++) {
			for (int x = 0; x < m_SizeX; x++) {

				int picX = x * width / m_SizeX;
				int picY = z * height / m_SizeZ;
				unsigned char pixelValue = imageData[picY * width + picX];
				float h = (float)pixelValue / 15.0f;
				int n = z * m_SizeZ * 6 + x * 6;
				m_Vertices[n + 0].position.y = h;
				if (x != 0)m_Vertices[n - 2].position.y = h;
				if (x != 0)m_Vertices[n - 5].position.y = h;
				if (z != 0)m_Vertices[n - m_SizeX * 6 + 2].position.y = h;
				if (z != 0)m_Vertices[n - m_SizeX * 6 + 3].position.y = h;
				if (x != 0 && z != 0)m_Vertices[n - m_SizeX * 6 - 1].position.y = h;
			}
		}


		// 法線ベクトルを更新
		for (int z = 0; z < m_SizeZ; z++) {
			for (int x = 0; x < m_SizeX; x++) {
				int n = z * m_SizeZ * 6 + x * 6;

				// 2つのベクトルを計算
				Vector3 v1 = m_Vertices[n + 1].position - m_Vertices[n + 0].position;
				Vector3 v2 = m_Vertices[n + 2].position - m_Vertices[n + 0].position;
				Vector3 normal = v1.Cross(v2); // 外積を計算
				normal.Normalize();			   // 正規化
				m_Vertices[n + 0].normal = normal;//normal:法線のデータを格納
				m_Vertices[n + 1].normal = normal;
				m_Vertices[n + 2].normal = normal;

				// 2つのベクトルを計算
				v1 = m_Vertices[n + 4].position - m_Vertices[n + 3].position;
				v2 = m_Vertices[n + 5].position - m_Vertices[n + 3].position;
				normal = v1.Cross(v2);	// 外積を計算
				normal.Normalize();		// 正規化

				m_Vertices[n + 3].normal = normal;
				m_Vertices[n + 4].normal = normal;
				m_Vertices[n + 5].normal = normal;

				//スムーズシェーディングの部分
				Vector3 h = (m_Vertices[n + 0].normal + m_Vertices[n + 1].normal + m_Vertices[n + 2].normal
					+ m_Vertices[n + 3].normal + m_Vertices[n + 4].normal + m_Vertices[n + 5].normal) / 6;

				//法線の平均を取り、対応する6頂点の法線データに法線の平均を格納すればスムーズシェーディングになるっぽい。
				m_Vertices[n + 0].normal = h;
				if (x != 0)m_Vertices[n - 2].normal = h;
				if (x != 0)m_Vertices[n - 5].normal = h;
				if (z != 0)m_Vertices[n - m_SizeX * 6 + 2].normal = h;
				if (z != 0)m_Vertices[n - m_SizeX * 6 + 3].normal = h;
				if (x != 0 && z != 0)m_Vertices[n - m_SizeX * 6 - 1].normal = h;

			}
		}

		// メモリを解放
		stbi_image_free(imageData);
	}

	// 頂点バッファ生成
	m_VertexBuffer.Create(m_Vertices);

	// インデックスバッファ生成
	m_IndexBuffer.Create(indices);

	// シェーダオブジェクト生成
	m_Shader.Create("shader/litTextureVS.hlsl", "shader/litTexturePS.hlsl");

	// テクスチャロード
	bool sts = m_Texture.Load("assets/texture/field.jpg");
	assert(sts == true);

	// マテリアル情報取得
	m_Materiale = std::make_unique<Material>();
	MATERIAL mtrl;
	mtrl.Diffuse = Color(1, 1, 1, 1);
	mtrl.Shiness = 1;
	mtrl.TextureEnable = true; //テクスチャを使うか否かのフラグ
	m_Materiale->Create(mtrl);

	//Groundの位置や大きさを調整
	m_Position.y = -0.1f;
	m_Scale.x = 10.0f;
	m_Scale.z = 10.0f;

	// 頂点情報を変換しておく
	Matrix r = Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
	Matrix t = Matrix::CreateTranslation(m_Position.x, m_Position.y, m_Position.z);
	Matrix s = Matrix::CreateScale(m_Scale.x, m_Scale.y, m_Scale.z);
	Matrix worldmtx = s * r * t;
	for (int i = 0; i < m_Vertices.size(); i++) {
		m_Vertices[i].position = Vector3::Transform(m_Vertices[i].position, worldmtx);
		m_Vertices[i].normal = Vector3::Transform(m_Vertices[i].normal, worldmtx);
	}
}


//=======================================
//更新処理
//=======================================
void Ground::Update()
{
	
}

//=======================================
//描画処理
//=======================================
void Ground::Draw()
{
	// SRT情報作成
	Matrix r = Matrix::CreateFromYawPitchRoll(
		m_Rotation.y,
		m_Rotation.x,
		m_Rotation.z);

	Matrix t = Matrix::CreateTranslation(
		m_Position.x,
		m_Position.y,
		m_Position.z);

	Matrix s = Matrix::CreateScale(
		m_Scale.x,
		m_Scale.y,
		m_Scale.z);

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
	m_Materiale->SetGPU();

	// カメラの設定を指定
	m_Camera->SetCamera(0);

	devicecontext->DrawIndexed(
		6 * m_SizeX * m_SizeZ,							// 描画するインデックス数（四角形なんで４）
		0,							// 最初のインデックスバッファの位置
		0);
}

//=======================================
//終了処理
//=======================================
void Ground::Uninit()
{

}

//=======================================
//頂点情報を取得
//=======================================
std::vector<VERTEX_3D> Ground::GetVertices()
{
	return m_Vertices;
}

DirectX::XMFLOAT2 Ground::GetGroundSize()
{
	return { m_Scale.x * m_SizeX,  m_Scale.z * m_SizeZ};
}