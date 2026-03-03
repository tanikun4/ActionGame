#include "common.hlsl"

Texture2D SceneTex : register(t0);
Texture2D BloomTex : register(t1);

SamplerState g_SamplerState : register(s0);

float BloomPower = 1.0f;

float4 ps_main(in PS_IN input) : SV_Target
{
    float4 scene = SceneTex.Sample(g_SamplerState, input.tex);
    float4 bloom = BloomTex.Sample(g_SamplerState, input.tex);

    return scene + bloom * BloomPower;
}
