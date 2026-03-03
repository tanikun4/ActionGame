struct PS_IN
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 color : COLOR0;
};

SamplerState samp : register(s0);
Texture2D tex : register(t0);

float4 main(PS_IN pin) : SV_TARGET
{
    float offsetU = 1.5f / 1280.0f;
    float offsetV = 1.5f / 720.0f;
	// フィルターに基づいて色を取得
	float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);	// 出力色の初期化
	
	// ベースピクセルカラー
	color = tex.Sample(samp, pin.uv);

	// 周囲8方向ピクセルをサンプリング
    color += tex.Sample(samp, pin.uv + float2(0.0f, offsetV)); // 上
    color += tex.Sample(samp, pin.uv + float2(offsetU, offsetV)); // 右上
    color += tex.Sample(samp, pin.uv + float2(offsetU, 0.0f)); // 右
    color += tex.Sample(samp, pin.uv + float2(offsetU, -offsetV)); // 右下
    color += tex.Sample(samp, pin.uv + float2(0.0f, -offsetV)); // 下
    color += tex.Sample(samp, pin.uv + float2(-offsetU, -offsetV)); // 左下
    color += tex.Sample(samp, pin.uv + float2(-offsetU, 0.0f)); // 左
    color += tex.Sample(samp, pin.uv + float2(-offsetU, offsetV)); // 左上
	
	return color /= 9.0f;
}