#include "PrefabPrimitive.hlsli"

// 入力頂点
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct Primitive
{
    float4x4 worldViewProjection;
    float4x4 world;
    float4x4 worldInverseTranspose;
    float4 color;
    float4x4 uvTransform;
    float environment;
};
StructuredBuffer<Primitive> gPrimitive : register(t0);

VertexShaderOutput main(VertexShaderInput input, uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    // 同次クリップ空間に変換
    output.position = mul(input.position, gPrimitive[instanceID].worldViewProjection);
    
    // UV座標を渡す
    output.texcoord = input.texcoord;
    
    // ワールド座標に変換
    output.worldPosition = mul(input.position, gPrimitive[instanceID].world).xyz;
    
    // 非均一スケールでワールド座標に変換
    output.normal = normalize(mul(input.normal, (float3x3) gPrimitive[instanceID].worldInverseTranspose));
    
    // 色
    output.color = gPrimitive[instanceID].color;
    
    // UVトランスフォーム
    output.uvTransform = gPrimitive[instanceID].uvTransform;
    
    // 環境
    output.environment = gPrimitive[instanceID].environment;
    
    return output;
}