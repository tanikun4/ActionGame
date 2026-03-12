#include "common.hlsl"

Texture2D SceneTex : register(t0);
Texture2D BloomTex : register(t1);

SamplerState Samp : register(s0);

cbuffer BloomParam : register(b0)
{
    float BloomIntensity;
    float Padding[3];
};

float4 ps_main(float4 pos : SV_POSITION, float2 uv : TEXCOORD) : SV_TARGET
{
    float3 scene = SceneTex.Sample(Samp, uv).rgb;
    float3 bloom = BloomTex.Sample(Samp, uv).rgb;

    float3 result = scene + bloom * BloomIntensity;

    return float4(result, 1);
}