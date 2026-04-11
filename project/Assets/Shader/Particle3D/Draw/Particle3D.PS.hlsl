#include "Particle3D.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

Texture2D<float4> gColorTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャの色をサンプリング
    float4 textureColor = gColorTexture.Sample(gSampler, input.texcoord);
    
    // パーティクルの色とテクスチャの色を掛け合わせる
    output.color = input.color * textureColor;
    
    return output;
}