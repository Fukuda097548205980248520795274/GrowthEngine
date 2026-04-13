#include "../Render2D/Render2D.hlsli"

struct PixelShaderOutput
{
    float4 color : SV_TARGET0;
};

struct Material
{
    // 色
    float4 color;
};
ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float> gTexture : register(t0);
SamplerState gSampler : register(s0);

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // テクスチャカラー
    float textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // テクスチャカラーが0.5以下なら描画しない
    if(textureColor <= 0.5f)
    {
        discard;
    }
    
    output.color = gMaterial.color * textureColor;
    
    return output;
}