#include "PrimitiveModel.hlsli"

// 出力ピクセルシェーダ
struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

// マテリアル
struct Material
{
    // 色
    float4 color;
};
ConstantBuffer<Material> gMaterial : register(b0);

// テクスチャ
Texture2D<float4> gTexture : register(t0);

// サンプラー
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャ
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // 色
    output.color = textureColor * gMaterial.color;
    
    return output;
}