#include "PrefabSprite.hlsli"

// 頂点入力
struct VertexShaderInput
{
    float4 position : POSITION0;
    float2 texcoord : TEXCOORD0;
};

struct Sprite
{
    // WVP行列
    float4x4 worldViewProjection;
    
    // アンカーポイント
    float2 anchor;
    
    // 色
    float4 color;
    
    // UVトランスフォーム
    float4x4 uvTransform;
};
StructuredBuffer<Sprite> gSprite : register(t0);

VertexShaderOutput main(VertexShaderInput input , uint instanceID : SV_InstanceID)
{
    VertexShaderOutput output;
    
    float4 localPos;
    localPos.x = input.position.x - gSprite[instanceID].anchor.x;
    localPos.y = input.position.y + gSprite[instanceID].anchor.y;
    localPos.z = input.position.z;
    localPos.w = input.position.w;
    
    output.position = mul(localPos, gSprite[instanceID].worldViewProjection);
    output.texcoord = input.texcoord;
    output.color = gSprite[instanceID].color;
    output.uvTransform = gSprite[instanceID].uvTransform;
    
    return output;
}