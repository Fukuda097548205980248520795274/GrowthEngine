#include "Line.hlsli"

// 入力頂点
struct VertexShaderInput
{
    float4 position : POSITION0;
};

struct Transformation
{
    float4x4 viewProjection;
};
ConstantBuffer<Transformation> gTransformation : register(b0);

struct Color
{
    float4 color;
};
StructuredBuffer<Color> gColor : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint vertexID : SV_VertexID)
{
    VertexShaderOutput output;
    
    output.position = mul(input.position, gTransformation.viewProjection);
    output.color = gColor[vertexID / 2].color;
    
    return output;
}