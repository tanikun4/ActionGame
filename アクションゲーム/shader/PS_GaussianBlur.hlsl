struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

cbuffer Param : register(b0)
{
	float2 texSize; // テクスチャサイズ
	float2 blurDir; // ブラーをかける方向
};

cbuffer Blur : register(b1)
{
	float4 weight[2]; // 重み
};

SamplerState samp : register(s0);
Texture2D tex : register(t0);

float4 main(PS_IN pin) : SV_TARGET
{
	// フィルターに基づいて色を取得
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);	// 出力色の初期化
	float2 uvOffset = 1.0f / texSize;				// 1ピクセルのUV上での移動量
	uvOffset *= blurDir;							// 指定された方向のみ移動させる
	float2 uv = pin.uv - uvOffset * 2;				// 初期位置からずれたUV座標を計算
	
	// フィルターを適用して色を計算
	for (int i = 0; i < 2; ++i)
	{
		// テクスチャから色をサンプリングしてフィルターをかける
		color += tex.Sample(samp, uv) * weight[i].x;
		uv += uvOffset;
		color += tex.Sample(samp, uv) * weight[i].y;
		uv += uvOffset;
		color += tex.Sample(samp, uv) * weight[i].z;
		uv += uvOffset;
		color += tex.Sample(samp, uv) * weight[i].w;
		uv += uvOffset;
	}
	
	uv = pin.uv - uvOffset * 2; // uv再計算 今度はマイナス方向にずらしていく
	// フィルターを適用して色を計算
	for (int j = 0; j < 2; ++j)
	{
		// テクスチャから色をサンプリングしてフィルターをかける
		color += tex.Sample(samp, uv) * weight[j].x;
		uv -= uvOffset;
		color += tex.Sample(samp, uv) * weight[j].y;
		uv -= uvOffset;
		color += tex.Sample(samp, uv) * weight[j].z;
		uv -= uvOffset;
		color += tex.Sample(samp, uv) * weight[j].w;
		uv -= uvOffset;
	}

	return color;
}