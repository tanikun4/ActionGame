#include "common.hlsl"

PS_IN main(in VS_IN input)
{
    PS_IN output;

    //position
    matrix wvp;
    wvp = mul(World, View);
    wvp = mul(wvp, Projection);
    output.pos = mul(input.pos, wvp);

    //color
    // ライト計算なし、法線も無視
    output.col = input.col; // 頂点カラーだけを渡す

    // texture
    output.tex = input.tex;

    return output;
}
