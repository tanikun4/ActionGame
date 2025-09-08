#include "common.hlsl"
// ピクセルシェーダーの入力構造体は頂点シェーダーと同じ
struct PS_IN
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
};

// 常に白で描画
float4 ps_main(PS_IN input) : SV_Target
{
    return input.col;

}