#include "../Fullscreen/Fullscreen.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct Param
{
    // 影の色
    float4 shadowColor;
    
    // 影のずれ幅
    float2 shadowOffset;
    
    // ぼかしの強さ
    float blurSize;
};
ConstantBuffer<Param> gParam : register(b0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 影のUV座標を計算する
    float2 shadowUV = input.texcoord - gParam.shadowOffset;
    
    // 影のアルファ値を計算する
    float shadowAlpha = 0;
    
    // 十字方向と中央の計5点をサンプリングする簡易的なブラー
    shadowAlpha += gTexture.Sample(gSampler, shadowUV + float2(gParam.blurSize, 0)).a;
    shadowAlpha += gTexture.Sample(gSampler, shadowUV + float2(-gParam.blurSize, 0)).a;
    shadowAlpha += gTexture.Sample(gSampler, shadowUV + float2(0, gParam.blurSize)).a;
    shadowAlpha += gTexture.Sample(gSampler, shadowUV + float2(0, -gParam.blurSize)).a;
    shadowAlpha += gTexture.Sample(gSampler, shadowUV).a;
    
    shadowAlpha /= 5.0; // 5点で割って平均化する
    
    // 影の色に計算したアルファ値を掛ける
    float4 shadowCol = gParam.shadowColor;
    shadowCol.a *= shadowAlpha;

    // 元のテクスチャの色を取得する
    float4 mainCol = gTexture.Sample(gSampler, input.texcoord);
    
    // アルファブレンドの公式: 背景(影)と前面(スプライト)を合成
    output.color.rgb = lerp(shadowCol.rgb, mainCol.rgb, mainCol.a);
    output.color.a = max(mainCol.a, shadowCol.a); // アルファ値の合成
    
    return output;
}