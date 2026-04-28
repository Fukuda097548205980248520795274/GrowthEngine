#include "MotionVector.hlsli"

// 頂点シェーダ入力
struct VertexShaderInput
{
    float4 position : POSITION0;
};

// 座標変換
struct Transformation
{
    // 今のWVP行列
    float4x4 currentWVP;
    
    // 前のWVP行列
    float4x4 prevWVP;
};
ConstantBuffer<Transformation> gTransformation : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // 現在の座標を計算
    output.position = mul(input.position, gTransformation.currentWVP);
    output.currentPos = output.position;
    
    // 前のフレームでの座標を計算
    output.prevPos = mul(input.position, gTransformation.prevWVP);
    
    return output;
}