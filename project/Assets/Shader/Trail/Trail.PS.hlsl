#include "Trail.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャの色を取得
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // テクスチャの色を出力
    output.color = textureColor;
    
    return output;
}