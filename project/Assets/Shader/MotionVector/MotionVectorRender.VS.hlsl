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
    
    // 残像マスク（0.0f: 残像なし, 1.0f: 完全残像）
    float afterImageMask;
    
    // モーションブラーマスク（0.0f: ブラーなし, 1.0f: 完全ブラー）
    float motionBlurMask;
    
    float2 padding; // 16バイトアラインメントのためのパディング
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
    
    // ブラーマスクを設定
    output.blurMask = float2(gTransformation.afterImageMask, gTransformation.motionBlurMask);
    
    return output;
}