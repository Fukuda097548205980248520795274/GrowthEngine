#include "OutlineRender.hlsli"

// 頂点シェーダーの入力構造体
struct VertexShaderInput
{
    float4 position : POSITION0;
};

// 座標変換用の定数バッファ
struct Transformation
{
    float4x4 worldViewProjection;
};
ConstantBuffer<Transformation> gTransformation : register(b0);

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    
    // 頂点の位置をワールド・ビュー・プロジェクション行列で変換
    output.position = mul(input.position, gTransformation.worldViewProjection);
   
    return output;
}