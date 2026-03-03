#include "Renderer.h"
#include "Application.h"

using namespace DirectX::SimpleMath;

// D3D_FEATURE_LEVELはDirect3Dのバージョン
D3D_FEATURE_LEVEL Renderer::m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;

ID3D11Device* Renderer::m_Device{}; // Direct3Dデバイス
ID3D11DeviceContext* Renderer::m_DeviceContext{}; // デバイスコンテキスト
IDXGISwapChain* Renderer::m_SwapChain{}; // スワップチェーン
ID3D11RenderTargetView* Renderer::m_RenderTargetView{}; // レンダーターゲットビュー
ID3D11DepthStencilView* Renderer::m_DepthStencilView{}; // デプスステンシルビュー

ID3D11Buffer* Renderer::m_WorldBuffer{}; // ワールド行列
ID3D11Buffer* Renderer::m_ViewBuffer{}; // ビュー行列
ID3D11Buffer* Renderer::m_ProjectionBuffer{}; // プロジェクション行列

ID3D11Buffer* Renderer::m_LightBuffer{}; //ライト設定(平行光源)
ID3D11Buffer* Renderer::m_MaterialBuffer{};	//マテリアル設定
ID3D11Buffer* Renderer::m_TextureBuffer{}; //UV設定

// デプスステンシルステート
ID3D11DepthStencilState* Renderer::m_DepthStateEnable{};
ID3D11DepthStencilState* Renderer::m_DepthStateDisable{};

ID3D11BlendState* Renderer::m_BlendState[MAX_BLENDSTATE]; // ブレンドステート配列
ID3D11BlendState* Renderer::m_BlendStateATC{}; // 特定のアルファテストとカバレッジ（ATC）用のブレンドステート

ID3D11RasterizerState* Renderer::m_RSCullBack = nullptr;
ID3D11RasterizerState* Renderer::m_RSCullNone = nullptr;

// ここからポストプロセス
ID3D11Texture2D* Renderer::m_PostTexture = nullptr;
ID3D11RenderTargetView* Renderer::m_PostRTV = nullptr;
ID3D11ShaderResourceView* Renderer::m_PostSRV = nullptr;

// ブルームエフェクト用のテクスチャとビュー
ID3D11Texture2D* Renderer::m_BloomTex = nullptr;
ID3D11RenderTargetView* Renderer::m_BloomRTV = nullptr;
ID3D11ShaderResourceView* Renderer::m_BloomSRV = nullptr;

// ブルームエフェクト用PS変数
ID3D11PixelShader* Renderer::m_BrightPassPS = nullptr;
ID3D11PixelShader* Renderer::m_BloomCombinePS = nullptr;

// ブラー用のテクスチャとビュー
ID3D11Texture2D* Renderer::m_BlurTex = nullptr;
ID3D11RenderTargetView* Renderer::m_BlurRTV = nullptr;
ID3D11ShaderResourceView* Renderer::m_BlurSRV = nullptr;

ID3D11PixelShader* Renderer::m_BlurXPS = nullptr;
ID3D11PixelShader* Renderer::m_BlurYPS = nullptr;

ID3D11Buffer* Renderer::m_BlurBuffer = nullptr;

ID3D11Buffer* Renderer::m_FullscreenVB = nullptr;
ID3D11Buffer* Renderer::m_FullscreenIB = nullptr;

// スプライトバッチ
std::unique_ptr<DirectX::SpriteBatch> Renderer::m_SpriteBatch = nullptr;

//=======================================
//初期化処理
//=======================================
void Renderer::Init()
{
	HRESULT hr = S_OK;

	// デバイス、スワップチェーン作成
	DXGI_SWAP_CHAIN_DESC swapChainDesc{};
	swapChainDesc.BufferCount = 1; // バックバッファの数を1に設定（ダブルバッファリング）
	swapChainDesc.BufferDesc.Width = Application::GetWidth(); // バッファの幅をウィンドウサイズに合わせる
	swapChainDesc.BufferDesc.Height = Application::GetHeight(); // バッファの高さをウィンドウサイズに合わせる
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // バッファのピクセルフォーマットを設定
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60; // リフレッシュレートを設定（Hz）
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // バッファの使用用途を設定
	swapChainDesc.OutputWindow = Application::GetWindow(); // スワップチェーンのターゲットウィンドウを設定
	swapChainDesc.SampleDesc.Count = 1; // マルチサンプリングの設定（アンチエイリアスのサンプル数とクオリティ）
	swapChainDesc.SampleDesc.Quality = 0; //同上
	swapChainDesc.Windowed = TRUE; // ウィンドウモード（フルスクリーンではなく、ウィンドウモードで実行）

	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, 
		D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, &m_FeatureLevel, &m_DeviceContext);
	if (FAILED(hr)) return;

	// レンダーターゲットビュー作成
	ID3D11Texture2D* renderTarget{};
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&renderTarget);
	if (renderTarget != nullptr)m_Device->CreateRenderTargetView(renderTarget, NULL, &m_RenderTargetView);
	renderTarget->Release();

	// デプスステンシルバッファ作成
	ID3D11Texture2D* depthStencile{};
	D3D11_TEXTURE2D_DESC textureDesc{};
	textureDesc.Width = swapChainDesc.BufferDesc.Width;   // バッファの幅をスワップチェーンに合わせる
	textureDesc.Height = swapChainDesc.BufferDesc.Height; // バッファの高さをスワップチェーンに合わせる
	textureDesc.MipLevels = 1;                            // ミップレベルは1（ミップマップは使用しない）
	textureDesc.ArraySize = 1;                            // テクスチャの配列サイズ（通常1）
	textureDesc.Format = DXGI_FORMAT_D16_UNORM;           // フォーマットは16ビットの深度バッファを使用
	textureDesc.SampleDesc = swapChainDesc.SampleDesc;    // スワップチェーンと同じサンプル設定
	textureDesc.Usage = D3D11_USAGE_DEFAULT;              // 使用方法はデフォルト（GPUで使用）
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;     // 深度ステンシルバッファとして使用
	textureDesc.CPUAccessFlags = 0;                       // CPUからのアクセスは不要
	textureDesc.MiscFlags = 0;                            // その他のフラグは設定なし

	hr = m_Device->CreateTexture2D(&textureDesc, NULL, &depthStencile);
	if (FAILED(hr)) return;


	// デプスステンシルビュー作成
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc{};
	depthStencilViewDesc.Format = textureDesc.Format; // デプスステンシルバッファのフォーマットを設定
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D; // ビューの次元を2Dテクスチャとして設定（2Dテクスチャ用のデプスステンシルビュー）
	depthStencilViewDesc.Flags = 0; // 特別なフラグは設定しない（デフォルトの動作）
	if (depthStencile != nullptr)m_Device->CreateDepthStencilView(depthStencile, &depthStencilViewDesc, &m_DepthStencilView);
	depthStencile->Release();

	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, m_DepthStencilView); // レンダーターゲットとデプスステンシルビューを設定

	// ビューポート設定
	D3D11_VIEWPORT viewport;
	viewport.Width = (FLOAT)Application::GetWidth();   // ビューポートの幅
	viewport.Height = (FLOAT)Application::GetHeight(); // ビューポートの高さ
	viewport.MinDepth = 0.0f;                          // 深度範囲の最小値
	viewport.MaxDepth = 1.0f;                          // 深度範囲の最大値
	viewport.TopLeftX = 0;                             // ビューポートの左上隅のX座標
	viewport.TopLeftY = 0;                             // ビューポートの左上隅のY座標）
	m_DeviceContext->RSSetViewports(1, &viewport);

	
	// ラスタライザステート設定
	D3D11_RASTERIZER_DESC rasterizerDesc{};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	//rasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	//rasterizerDesc.CullMode = D3D11_CULL_NONE;
	//rasterizerDesc.CullMode = D3D11_CULL_FRONT;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;

	//ID3D11RasterizerState* rs;
	//hr = m_Device->CreateRasterizerState(&rasterizerDesc, &rs);
	
	//m_DeviceContext->RSSetState(rs);

	// 通常用（裏面カリング）
	hr = m_Device->CreateRasterizerState(&rasterizerDesc, &m_RSCullBack);
	if (FAILED(hr)) return;

	// 両面描画用
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	hr = m_Device->CreateRasterizerState(&rasterizerDesc, &m_RSCullNone);
	if (FAILED(hr)) return;

	// デフォルトは通常
	m_DeviceContext->RSSetState(m_RSCullBack);

	// ブレンド ステート生成
	D3D11_BLEND_DESC BlendDesc;
	ZeroMemory(&BlendDesc, sizeof(BlendDesc));                   // BlendDesc構造体をゼロで初期化し、メモリをクリア
	BlendDesc.AlphaToCoverageEnable = FALSE;                     // アルファ・トゥ・カバレッジを無効化（透明度をカバレッジとして利用しない）
	BlendDesc.IndependentBlendEnable = TRUE;                     // 各レンダーターゲットに対して個別のブレンド設定を有効化
	BlendDesc.RenderTarget[0].BlendEnable = FALSE;               // ブレンドを無効に設定（不透明な描画）
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;  // ソース（描画するピクセル）のアルファ値を使用
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; // デスティネーション（既存のピクセル）の逆アルファ値を使用
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;      // ソースとデスティネーションを加算する操作
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;   // ソースのアルファ値をそのまま使用
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO; // デスティネーションのアルファ値を無視
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD; // アルファ値に対して加算操作を行う
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL; // レンダーターゲットのカラーチャンネル書き込みマスク

	hr = m_Device->CreateBlendState(&BlendDesc, &m_BlendState[0]);
	if (FAILED(hr)) return;

	// ブレンド ステート生成 (アルファ ブレンド用)
	//BlendDesc.AlphaToCoverageEnable = TRUE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	hr = m_Device->CreateBlendState(&BlendDesc, &m_BlendState[1]);
	if (FAILED(hr)) return;

	// ブレンド ステート生成 (加算合成用)
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	hr = m_Device->CreateBlendState(&BlendDesc, &m_BlendState[2]);
	if (FAILED(hr)) return;

	// ブレンド ステート生成 (減算合成用)
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_REV_SUBTRACT;
	hr = m_Device->CreateBlendState(&BlendDesc, &m_BlendState[3]);
	if (FAILED(hr)) return;

	SetBlendState(BS_ALPHABLEND);


	// デプスステンシルステート設定
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc{};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	depthStencilDesc.StencilEnable = FALSE;

	hr = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStateEnable); //深度有効ステート
	if (FAILED(hr)) return;

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStateDisable); //深度無効ステート
	if (FAILED(hr)) return;

	m_DeviceContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);

	// サンプラーステート設定
	D3D11_SAMPLER_DESC samplerDesc{};
	//samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	//samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // ブルーム用
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* samplerState{};
	hr = m_Device->CreateSamplerState(&samplerDesc, &samplerState);
	if (FAILED(hr)) return;

	m_DeviceContext->PSSetSamplers(0, 1, &samplerState);

	// 定数バッファ生成
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(Matrix);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = 0;
	bufferDesc.MiscFlags = 0;
	bufferDesc.StructureByteStride = sizeof(float);

	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_WorldBuffer);
	m_DeviceContext->VSSetConstantBuffers(0, 1, &m_WorldBuffer);
	if (FAILED(hr)) return;

	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_ViewBuffer);
	m_DeviceContext->VSSetConstantBuffers(1, 1, &m_ViewBuffer);
	if (FAILED(hr)) return;

	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_ProjectionBuffer);
	m_DeviceContext->VSSetConstantBuffers(2, 1, &m_ProjectionBuffer);
	if (FAILED(hr)) return;

	bufferDesc.ByteWidth = sizeof(LIGHT);
	m_Device->CreateBuffer(&bufferDesc, NULL, &m_LightBuffer);
	m_DeviceContext->VSSetConstantBuffers(3, 1, &m_LightBuffer);

	//　ライト初期化
	LIGHT light{};
	light.Enable = true;
	light.Direction = Vector4(0.5f, -1.0f, 0.8f, 0.0f); //方向
	light.Direction.Normalize();
	light.Diffuse = Color(1.5f, 1.5f, 1.5f, 1.0f); //並行光源の強さと色
	light.Ambient = Color(0.2f, 0.2f, 0.2f, 1.0f); //環境光の強さと色
	SetLight(light);

	bufferDesc.ByteWidth = sizeof(MATERIAL);
	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_MaterialBuffer);
	m_DeviceContext->VSSetConstantBuffers(4, 1, &m_MaterialBuffer);
	m_DeviceContext->PSSetConstantBuffers(4, 1, &m_MaterialBuffer);
	if (FAILED(hr)) return;

	// マテリアル初期化
	MATERIAL material{};
	material.Diffuse = Color(1.0f, 0.0f, 0.0f, 1.0f);
	material.Ambient = Color(1.0f, 1.0f, 1.0f, 1.0f);
	SetMaterial(material);

	bufferDesc.ByteWidth = sizeof(Matrix);
	hr = m_Device->CreateBuffer(&bufferDesc, NULL, &m_TextureBuffer);
	m_DeviceContext->VSSetConstantBuffers(5, 1, &m_TextureBuffer);
	if (FAILED(hr)) return;

	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&renderTarget);
	D3D11_TEXTURE2D_DESC backBufferDesc;
	renderTarget->GetDesc(&backBufferDesc); // 実際の画面サイズを取得
	renderTarget->Release();

	// ポストプロセス用RT
	D3D11_TEXTURE2D_DESC postDesc{};
	postDesc.Width = Application::GetWidth();
	postDesc.Height = Application::GetHeight();
	postDesc.MipLevels = 1;
	postDesc.ArraySize = 1;
	postDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	postDesc.SampleDesc.Count = 1;
	postDesc.BindFlags =
		D3D11_BIND_RENDER_TARGET |
		D3D11_BIND_SHADER_RESOURCE;

	postDesc.Usage = D3D11_USAGE_DEFAULT;
	postDesc.CPUAccessFlags = 0;
	postDesc.MiscFlags = 0;

	// フルスクリーンクアッド
	Vertex2D quad[4] = {
		{ {-1,  1, 0}, {0,0} }, // 左上
		{ { 1,  1, 0}, {1,0} }, // 右上
		{ {-1, -1, 0}, {0,1} }, // 左下
		{ { 1, -1, 0}, {1,1} }, // 右下
	};
	D3D11_BUFFER_DESC bd{};
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(quad);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA init{};
	init.pSysMem = quad;
	m_Device->CreateBuffer(&bd, &init, &m_FullscreenVB);

	UINT indices[6] = { 0,1,2,2,1,3 };
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(indices);
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	init.pSysMem = indices;
	m_Device->CreateBuffer(&bd, &init, &m_FullscreenIB);


	hr = m_Device->CreateTexture2D(&postDesc, nullptr, &m_PostTexture);
	if (FAILED(hr)) return;

	hr = m_Device->CreateRenderTargetView(m_PostTexture, nullptr, &m_PostRTV);
	if (FAILED(hr)) return;

	hr = m_Device->CreateShaderResourceView(m_PostTexture, nullptr, &m_PostSRV);
	if (FAILED(hr)) return;

	// ブルーム用RT
	hr = m_Device->CreateTexture2D(&postDesc, nullptr, &m_BloomTex);
	if (FAILED(hr)) return;

	hr = m_Device->CreateRenderTargetView(m_BloomTex, nullptr, &m_BloomRTV);
	if (FAILED(hr)) return;

	hr = m_Device->CreateShaderResourceView(m_BloomTex, nullptr, &m_BloomSRV);
	if (FAILED(hr)) return;

	// ブラー用RT
	hr = m_Device->CreateTexture2D(&postDesc, nullptr, &m_BlurTex);
	if (FAILED(hr)) return;

	hr = m_Device->CreateRenderTargetView(m_BlurTex, nullptr, &m_BlurRTV);
	if (FAILED(hr)) return;

	hr = m_Device->CreateShaderResourceView(m_BlurTex, nullptr, &m_BlurSRV);
	if (FAILED(hr)) return;

	// ブラー用定数バッファ
	D3D11_BUFFER_DESC blurDesc{};
	blurDesc.ByteWidth = (sizeof(BlurBuffer) + 15) & ~15;
	blurDesc.Usage = D3D11_USAGE_DEFAULT;
	blurDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	m_Device->CreateBuffer(&blurDesc, nullptr, &m_BlurBuffer);

	// スプライトバッチの初期化
	m_SpriteBatch = std::make_unique<DirectX::SpriteBatch>(m_DeviceContext);

	// UV初期化
	SetUV(0, 0, 1, 1);

    // ブルーム用ピクセルシェーダー読み込み
	CreatePixelShader(&m_BrightPassPS, "shader/BrightPassPS.cso");
	CreatePixelShader(&m_BloomCombinePS, "shader/BloomCombinePS.cso");

	// ブラー用ピクセルシェーダー読み込み
	CreatePixelShader(&m_BlurXPS, "shader/BlurXPS.cso");
	CreatePixelShader(&m_BlurYPS, "shader/BlurYPS.cso");
}

//=======================================
//終了処理
//=======================================
void Renderer::Uninit()
{
	m_WorldBuffer->Release();
	m_ViewBuffer->Release();
	m_ProjectionBuffer->Release();

	m_LightBuffer->Release();
	m_MaterialBuffer->Release();

	m_DeviceContext->ClearState();
	m_RenderTargetView->Release();
	m_SwapChain->Release();
	m_DeviceContext->Release();
	m_Device->Release();

	if (m_RSCullBack) {
		m_RSCullBack->Release();
		m_RSCullBack = nullptr;
	}

	if (m_RSCullNone) {
		m_RSCullNone->Release();
		m_RSCullNone = nullptr;
	}

}

//=======================================
//描画開始
//=======================================
void Renderer::Begin()
{


	ID3D11ShaderResourceView* nullSRV[8] = {};
	m_DeviceContext->PSSetShaderResources(0, 8, nullSRV);

	SetFullViewport();

	float clearColor[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	//m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, clearColor);
	//m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// 描画先をポストRTへ
	m_DeviceContext->OMSetRenderTargets(1, &m_PostRTV, m_DepthStencilView);

	m_DeviceContext->ClearRenderTargetView(m_PostRTV, clearColor);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
}

//=======================================
//描画終了
//=======================================
void Renderer::End()
{

	// ブルーム描画
	DrawBloom();

	// ここでポストプロセス描画
	DrawPostProcess();


	//m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);

	//D3D11_VIEWPORT vp{};
	//vp.Width = (float)Application::GetWidth();
	//vp.Height = (float)Application::GetHeight();
	//vp.MinDepth = 0.0f;
	//vp.MaxDepth = 1.0f;
	//m_DeviceContext->RSSetViewports(1, &vp);

	// フルスクリーンスプライト描画
	//m_SpriteBatch->Begin();
	//m_SpriteBatch->Draw(m_PostSRV, DirectX::XMFLOAT2(0, 0));
	//m_SpriteBatch->End();

	SetFullViewport();

	m_SwapChain->Present(1, 0);

	/*float green[4] = { 0, 1, 0, 1 };
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView, green);*/
}

//=======================================
// ポストプロセス描画
//=======================================
//void Renderer::DrawPostProcess()
//{
//
//	m_DeviceContext->PSSetShader(nullptr, nullptr, 0);
//
//	SetFullViewport();
//
//	SetDepthEnable(false);
//
//	// バックバッファに戻す
//	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
//
//	// フルスクリーン用
//	SetWorldViewProjection2D();
//
//	// SRVセット（さっき描いた画面）
//	m_DeviceContext->PSSetShaderResources(0, 1, &m_PostSRV);
//
//	m_SpriteBatch->Begin(
//		DirectX::SpriteSortMode_Deferred);
//
//	// ポストテクスチャを画面に描く
//	m_SpriteBatch->Draw(
//		m_PostSRV,
//		DirectX::SimpleMath::Vector2(0, 0));
//
//	m_SpriteBatch->End();
//
//    // Bloom加算
//	SetBlendState(BS_ADDITIVE);  // 加算ブレンド
//
//	m_SpriteBatch->Begin(DirectX::SpriteSortMode_Deferred);
//
//	m_SpriteBatch->Draw(m_BloomSRV, Vector2(0, 0));
//
//	m_SpriteBatch->End();
//
//	SetBlendState(BS_ALPHABLEND);
//
//	// バックバッファへ戻す
//	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
//
//	// ビューポートをフルサイズへ戻す
//	D3D11_VIEWPORT vp{};
//	vp.TopLeftX = 0;
//	vp.TopLeftY = 0;
//	vp.Width = (float)Application::GetWidth();
//	vp.Height = (float)Application::GetHeight();
//	vp.MinDepth = 0.0f;
//	vp.MaxDepth = 1.0f;
//	m_DeviceContext->RSSetViewports(1, &vp);
//
//	// SRV解除
//	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
//	m_DeviceContext->PSSetShaderResources(0, 1, nullSRV);
//}

void Renderer::DrawPostProcess()
{
	// バックバッファへ
	m_DeviceContext->OMSetRenderTargets(1, &m_RenderTargetView, nullptr);
	SetFullViewport();
	SetDepthEnable(false);

	// 1. 元の画面を描画
	m_DeviceContext->PSSetShader(m_DefaultPostPS, nullptr, 0); // 単純なテクスチャ表示PS
	m_DeviceContext->PSSetShaderResources(0, 1, &m_PostSRV);
	DrawFullScreenQuad();

	// 2. ブルームを加算
	SetBlendState(BS_ADDITIVE); // 加算ブレンド
	m_DeviceContext->PSSetShaderResources(0, 1, &m_BloomSRV);
	DrawFullScreenQuad();

	// 後片付け
	SetBlendState(BS_ALPHABLEND);
	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	m_DeviceContext->PSSetShaderResources(0, 1, nullSRV);
}

void Renderer::DrawBloom()
{
	SetDepthEnable(false);
	float clear[4] = { 0, 0, 0, 0 };

	// --- 1. 輝度抽出 ---
	m_DeviceContext->OMSetRenderTargets(1, &m_BloomRTV, nullptr);
	m_DeviceContext->ClearRenderTargetView(m_BloomRTV, clear);

	m_DeviceContext->PSSetShader(m_BrightPassPS, nullptr, 0);
	m_DeviceContext->PSSetShaderResources(0, 1, &m_PostSRV);
	DrawFullScreenQuad();

	// --- 2. ガウス重みの計算 (サンプルから移植) ---
	float weight[8];
	float total = 0;
	float sigma = 8.0f;
	for (int x = 0; x < 8; x++) {
		weight[x] = expf(-0.5f * (float)(x * x) / sigma);
		total += 2.0f * weight[x];
	}
	for (int i = 0; i < 8; i++) weight[i] /= total;

	// --- 3. X方向ブラー ---
	m_DeviceContext->OMSetRenderTargets(1, &m_BlurRTV, nullptr);
	m_DeviceContext->ClearRenderTargetView(m_BlurRTV, clear);

	BlurBuffer buf{};
	buf.TexelSize = Vector2(1.0f / Application::GetWidth(), 0.0f);
	m_DeviceContext->UpdateSubresource(m_BlurBuffer, 0, nullptr, &buf, 0, 0);

	// 重みテーブルを定数バッファ(スロット1)に送る
	m_DeviceContext->UpdateSubresource(m_WeightBuffer, 0, nullptr, weight, 0, 0);

	m_DeviceContext->PSSetShader(m_BlurXPS, nullptr, 0);
	m_DeviceContext->PSSetConstantBuffers(1, 1, &m_BlurBuffer);
	m_DeviceContext->PSSetConstantBuffers(2, 1, &m_WeightBuffer);
	m_DeviceContext->PSSetShaderResources(0, 1, &m_BloomSRV);
	DrawFullScreenQuad();

	// --- 4. Y方向ブラー ---
	m_DeviceContext->OMSetRenderTargets(1, &m_BloomRTV, nullptr);
	m_DeviceContext->ClearRenderTargetView(m_BloomRTV, clear);

	buf.TexelSize = Vector2(0.0f, 1.0f / Application::GetHeight());
	m_DeviceContext->UpdateSubresource(m_BlurBuffer, 0, nullptr, &buf, 0, 0);

	m_DeviceContext->PSSetShader(m_BlurYPS, nullptr, 0);
	m_DeviceContext->PSSetShaderResources(0, 1, &m_BlurSRV);
	DrawFullScreenQuad();
}

void Renderer::SetFullViewport()
{
	D3D11_VIEWPORT vp{};
	vp.Width = (FLOAT)Application::GetWidth();
	vp.Height = (FLOAT)Application::GetHeight();
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	m_DeviceContext->RSSetViewports(1, &vp);
}

//=======================================
// 深度ステンシルの有効・無効を設定
//=======================================
void Renderer::SetDepthEnable(bool Enable)
{
	if (Enable) 
	{
		// 深度テストを有効にするステンシルステートをセット
		m_DeviceContext->OMSetDepthStencilState(m_DepthStateEnable, NULL);
	}
	else
	{
		// 深度テストを無効にするステンシルステートをセット
		m_DeviceContext->OMSetDepthStencilState(m_DepthStateDisable, NULL);
	}
}

//=======================================
// アルファテストとカバレッジ（ATC）の有効・無効を設定
//=======================================
void Renderer::SetATCEnable(bool Enable)
{
	// ブレンドファクター（透明度などの調整に使用）
	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	if (Enable)
	{
		// アルファテストとカバレッジ (ATC) を有効にするブレンドステートをセット
		m_DeviceContext->OMSetBlendState(m_BlendStateATC, blendFactor, 0xffffffff);
	}
	else 
	{
		// 通常のブレンドステートをセット
		m_DeviceContext->OMSetBlendState(m_BlendState[0], blendFactor, 0xffffffff);
	}
}

//=======================================
//
//=======================================
void Renderer::SetWorldViewProjection2D()
{
	// サンプルコードに近い設定：幅1, 高さ1の正射影行列
	// これにより、Size(1,1)の板ポリゴンを描画すれば画面全体を覆えるようになります
	Matrix world = Matrix::Identity;
	Matrix view = Matrix::Identity;
	Matrix projection = DirectX::XMMatrixOrthographicLH(1.0f, 1.0f, 0.0f, 1.0f);

	world = world.Transpose();
	view = view.Transpose();
	projection = projection.Transpose();

	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &world, 0, 0);
	m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &view, 0, 0);
	m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &projection, 0, 0);
}

//=======================================
// ワールド行列を設定
//=======================================
void Renderer::SetWorldMatrix(Matrix* WorldMatrix)
{
	Matrix world;
	world = WorldMatrix->Transpose(); // 転置

	// ワールド行列をGPU側へ送る
	m_DeviceContext->UpdateSubresource(m_WorldBuffer, 0, NULL, &world, 0, 0);
}

//=======================================
// ビュー行列を設定
//=======================================
void Renderer::SetViewMatrix(Matrix* ViewMatrix)
{
	Matrix view;
	view = ViewMatrix->Transpose(); // 転置

	// ビュー行列をGPU側へ送る
	m_DeviceContext->UpdateSubresource(m_ViewBuffer, 0, NULL, &view, 0, 0);
}

//=======================================
// プロジェクション行列を設定
//=======================================
void Renderer::SetProjectionMatrix(Matrix* ProjectionMatrix)
{
	Matrix projection;
	projection = ProjectionMatrix->Transpose(); // 転置

	// プロジェクション行列をGPU側へ送る
	m_DeviceContext->UpdateSubresource(m_ProjectionBuffer, 0, NULL, &projection, 0, 0);
}

//=======================================
// ライトを設定
//=======================================
void Renderer::SetLight(LIGHT Light)
{
	// ライトの設定をGPU側へ送る
	m_DeviceContext->UpdateSubresource(m_LightBuffer, 0, NULL, &Light, 0, 0);
}

//=======================================
//マテリアルを設定
//=======================================
void Renderer::SetMaterial(MATERIAL Material)
{
	m_DeviceContext->UpdateSubresource(m_MaterialBuffer, 0, NULL, &Material, 0, 0);
}

//=======================================
//UV情報を設定
//=======================================
void Renderer::SetUV(float u, float v, float uw, float vh)
{
	// UVの行列作成
	Matrix mat = Matrix::CreateScale(uw, vh, 1.0f);
	mat *= Matrix::CreateTranslation(u, v, 0.0f).Transpose();

	m_DeviceContext->UpdateSubresource(m_TextureBuffer, 0, NULL, &mat, 0, 0);
}

//=======================================
//頂点シェーダー作成
//=======================================
void Renderer::CreateVertexShader(ID3D11VertexShader** VertexShader, ID3D11InputLayout** VertexLayout, const char* FileName)
{
	FILE* file; // ファイルを開くためのポインタ
	long int fsize; // ファイルサイズを格納する変数
	fopen_s(&file, FileName, "rb"); // シェーダーファイルをReadBinaryモードで開く
	assert(file); // ファイルが正常に開けたかどうかを確認

	fsize = _filelength(_fileno(file)); // ファイルのサイズを取得
	unsigned char* buffer = new unsigned char[fsize]; // ファイルサイズに基づいてバッファを確保
	fread(buffer, fsize, 1, file); // ファイルからバッファにデータを読み込む
	fclose(file); // 読み込み完了後、ファイルを閉じる

	// デバイスを使って頂点シェーダーを作成
	m_Device->CreateVertexShader(buffer, fsize, NULL, VertexShader);

	// 頂点レイアウト（入力レイアウト）の定義
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		// 頂点の位置情報（3つのfloat値）
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	0,		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 頂点の法線ベクトル情報（3つのfloat値）
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,		0,	4 * 3,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 頂点の色情報（4つのfloat値：RGBA）
		{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0,	4 * 6,	D3D11_INPUT_PER_VERTEX_DATA, 0 },
		// 頂点のテクスチャ座標情報（2つのfloat値）
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,			0,	4 * 10, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	UINT numElements = ARRAYSIZE(layout); // レイアウトの要素数を取得

	// デバイスを使って頂点レイアウトを作成
	m_Device->CreateInputLayout(layout, numElements, buffer, fsize, VertexLayout);

	delete[] buffer; // バッファのメモリを解放
}

//=======================================
//ピクセルシェーダー作成
//=======================================
void Renderer::CreatePixelShader(ID3D11PixelShader** PixelShader, const char* FileName)
{
	FILE* file; // ファイルを開くためのポインタ
	long int fsize; // ファイルサイズを格納する変数
	fopen_s(&file, FileName, "rb"); // シェーダーファイルをReadBinaryモードで開く
	assert(file); // ファイルが正常に開けたかどうかを確認

	fsize = _filelength(_fileno(file)); // ファイルのサイズを取得
	unsigned char* buffer = new unsigned char[fsize]; // ファイルサイズに基づいてバッファを確保
	fread(buffer, fsize, 1, file); // ファイルからバッファにデータを読み込む
	fclose(file); // 読み込み完了後、ファイルを閉じる

	// デバイスを使ってピクセルシェーダーを作成
	m_Device->CreatePixelShader(buffer, fsize, NULL, PixelShader);
	
	delete[] buffer; // バッファのメモリを解放
}
