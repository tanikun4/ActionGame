struct PS_IN
{
    float4 pos : SV_POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

// ライトの情報
cbuffer Light : register(b0)
{
    float4 lightColor;
    float4 lightAmbient;
    float4 lightDir;
};

// テクスチャ
Texture2D tex : register(t0);
Texture2D ramptex : register(t1);
SamplerState samp : register(s0);

float4 ps_main(PS_IN pin) : SV_TARGET
{
    float4 color = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    float3 N = normalize(pin.normal);
    float3 L = normalize(lightDir.xyz);
    L = -L;
    
    float diffuse = saturate(dot(N, L));
    
    #if 1
        float2 rampUV = float2(diffuse * 0.98f + 0.01f, 0.5f);
        diffuse = ramptex.Sample(samp, rampUV).r;
    #else
        // diffuseの値を階調化(0～1 → 0, 0.5,1.0)
        diffuse *= 2.0f; // 一度数値の範囲を広げる
        // 切り捨ての関係上、2になるのは1の時だけ
        // (0.9 * 2 = 1.8, 切り捨ては1となる)
        // 数値に下駄をはかす
        diffuse += 0.5f;
        // 小数を切り捨てた値を取得
        diffuse = floor(diffuse);
        // 広げた範囲と同等の数値で割る
        diffuse /= 2.0f;
    #endif
    
    //diffuse = max(diffuse, 2.8f);
    
    // テクスチャの色も考慮して陰をつける
    float4 texColor = tex.Sample(samp, pin.uv);
    color.rgb = texColor.rgb * (diffuse + lightAmbient.rgb);
    
    return color;
    //return float4(pin.uv, 0, 1);
}