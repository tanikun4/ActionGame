#include "common.hlsl"

Texture2D g_Texture : register(t0);
SamplerState g_Sampler : register(s0);

cbuffer BlurBuffer : register(b1)
{
    float2 TexelSize;
    float Padding[2];
};

float4 ps_main(in PS_IN input) : SV_Target
{
    float weights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };

    float3 color = g_Texture.Sample(g_Sampler, input.tex).rgb * weights[0];

    for (int i = 1; i < 5; i++)
    {
        color += g_Texture.Sample(g_Sampler, input.tex + float2(0, TexelSize.y * i)).rgb * weights[i];
        color += g_Texture.Sample(g_Sampler, input.tex - float2(0, TexelSize.y * i)).rgb * weights[i];
    }

    return float4(color, 1.0f);
}
