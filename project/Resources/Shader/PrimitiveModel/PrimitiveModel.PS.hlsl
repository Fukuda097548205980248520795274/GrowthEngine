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

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;
    
    // 色
    output.color = gMaterial.color;
    
    return output;
}