#include "PrimitiveModel.hlsli"

// 入力頂点シェーダ
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

// 座標変換行列
struct TransformationMatrix
{
    float4x4 worldViewProjection;
    float4x4 world;
    float4x4 worldInverseTranspose;
};
ConstantBuffer<TransformationMatrix> gTransformation : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // 同次クリップ空間に変換
    output.position = mul(input.position, gTransformation.worldViewProjection);
    
    // UV座標を渡す
    output.texcoord = input.texcoord;
    
    // ワールド座標に変換
    output.worldPosition = mul(input.position, gTransformation.world).xyz;
    
    // 非均一スケールでワールド座標に変換
    output.normal = normalize(mul(input.normal, (float3x3) gTransformation.worldInverseTranspose));
    
    return output;
}