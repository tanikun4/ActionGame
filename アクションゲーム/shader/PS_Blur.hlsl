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
    float weights[16];
    int sampleCount;
};

SamplerState samp : register(s0);
Texture2D tex : register(t0);

float4 main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(0, 0, 0, 0);
    float2 uvOffset = blurDir / texSize;
    float2 uv = pin.uv - uvOffset * ((sampleCount - 1) / 2.0);

    for (int i = 0; i < sampleCount; ++i)
    {
        color += tex.Sample(samp, uv) * weights[i];
        uv += uvOffset;
    }
    return color;
}