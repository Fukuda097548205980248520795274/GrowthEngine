#include "Trail.hlsli"

struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float4 color : COLOR0;
};

// 座標変換
struct Transformation
{
    float4x4 worldViewProjection;
};
ConstantBuffer<Transformation> gTransformation : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    output.position = mul(input.position, gTransformation.worldViewProjection);
    output.texcoord = input.texcoord;
    output.color = input.color;
    
    return output;
}