#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_SamplerState : register(s0);

float Threshold = 0.5f;

// ピクセルシェーダーのエントリポイント
// ブルーム用、明るい部分だけを抽出するシェーダー
float4 ps_main(in PS_IN input) : SV_Target
{
    float4 color = g_Texture.Sample(g_SamplerState, input.tex);

    float brightness = dot(color.rgb, float3(0.299, 0.587, 0.114));

    if (brightness > Threshold)
        return color;
    
    return float4(0, 0, 0, 1);

}
