#include "Sprite.hlsli"

// 頂点入力
struct VertexShaderInput
{
    // 位置
    float4 position : POSITION0;
    
    // テクスチャ座標
    float2 texcoord : TEXCOORD0;
};

struct Transformation
{
    // WVP行列
    float4x4 worldViewProjection;
    
    // アンカーポイント
    float2 anchor;
    
    // テクスチャサイズ
    float2 textureSize;
    
    // テクスチャ左上
    float2 textureLeftTop;
    
    // テクスチャ右下
    float2 textureRightBottom;
};
ConstantBuffer<Transformation> gTransformation : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    float4 localPos;
    localPos.x = input.position.x - gTransformation.anchor.x;
    localPos.y = input.position.y - gTransformation.anchor.y;
    localPos.z = input.position.z;
    localPos.w = input.position.w;
    
    output.position = mul(localPos, gTransformation.worldViewProjection);
    
    
    float2 uvPixel = gTransformation.textureLeftTop + (input.texcoord * gTransformation.textureRightBottom);
    float2 uv = uvPixel / gTransformation.textureSize;
    
    output.texcoord = uv;
    
    return output;
}